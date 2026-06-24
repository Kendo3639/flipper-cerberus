#include "cerberus_monitor_view.h"
#include "../helpers/cerberus_detector.h"

#include <gui/canvas.h>
#include <input/input.h>
#include <stdio.h>

#define RSSI_FLOOR_VIEW (-110)
#define RSSI_CEIL_VIEW  (-25)

struct CerberusMonitorView {
    View* view;
    CerberusMonitorCallback callback;
    void* context;
};

typedef struct {
    CerberusSnapshot snap;
    bool has_data;
    uint8_t frame; // animation counter for blinking
} CerberusMonitorModel;

// Map a dBm reading onto a [0..width] bar length.
static uint8_t cerberus_rssi_to_w(int16_t rssi, uint8_t width) {
    int v = rssi;
    if(v < RSSI_FLOOR_VIEW) v = RSSI_FLOOR_VIEW;
    if(v > RSSI_CEIL_VIEW) v = RSSI_CEIL_VIEW;
    int span = RSSI_CEIL_VIEW - RSSI_FLOOR_VIEW;
    int fill = (v - RSSI_FLOOR_VIEW) * width / span;
    if(fill < 0) fill = 0;
    if(fill > width) fill = width;
    return (uint8_t)fill;
}

static void cerberus_draw_meter(
    Canvas* canvas,
    uint8_t row,
    const CerberusBand* band,
    const CerberusBandStatus* status,
    bool active,
    bool blink) {
    const uint8_t row_top = 13 + row * 12;
    const uint8_t cy = row_top + 4;
    const bool alarm = status->threat != CerberusThreatNone;
    const bool invert = alarm && blink;

    if(invert) {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_box(canvas, 0, row_top - 1, 128, 12);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_set_color(canvas, ColorBlack);
    }

    // active head indicator (a little right-pointing arrow)
    if(active) {
        canvas_draw_line(canvas, 0, cy - 3, 0, cy + 3);
        canvas_draw_line(canvas, 1, cy - 2, 1, cy + 2);
        canvas_draw_line(canvas, 2, cy - 1, 2, cy + 1);
        canvas_draw_dot(canvas, 3, cy);
    }

    // band label
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 6, row_top + 8, band->label);

    // signal bar
    const uint8_t bx = 24, bw = 70, by = row_top + 1, bh = 7;
    canvas_draw_frame(canvas, bx - 1, by - 1, bw + 2, bh + 2);
    uint8_t fill = cerberus_rssi_to_w(status->rssi, bw);
    if(fill) canvas_draw_box(canvas, bx, by, fill, bh);

    // peak-hold notch riding just under the bar
    uint8_t px = bx + cerberus_rssi_to_w(status->peak, bw);
    if(px > bx) {
        canvas_draw_line(canvas, px, by + bh, px, by + bh);
        canvas_draw_dot(canvas, px, by + bh + 1);
    }

    // right-hand readout: either the dBm value or a flashing threat tag
    char buf[12];
    if(alarm) {
        snprintf(buf, sizeof(buf), "!%s", cerberus_threat_short(status->threat));
    } else {
        snprintf(buf, sizeof(buf), "%d", status->rssi);
    }
    canvas_draw_str_aligned(canvas, 127, row_top + 8, AlignRight, AlignBottom, buf);
}

static void cerberus_monitor_draw(Canvas* canvas, void* model) {
    CerberusMonitorModel* m = model;
    const CerberusSnapshot* s = &m->snap;
    const bool blink = ((m->frame / 3) & 1) != 0;

    canvas_clear(canvas);

    // --- Title bar ---
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0, 128, 11);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 9, "CERBERUS");

    canvas_set_font(canvas, FontSecondary);
    char mode[12];
    if(s->hop) {
        snprintf(mode, sizeof(mode), "HOP");
    } else {
        snprintf(mode, sizeof(mode), "PIN %u", (unsigned)cerberus_bands[s->active_band].mhz);
    }
    canvas_draw_str_aligned(canvas, 126, 9, AlignRight, AlignBottom, mode);
    canvas_set_color(canvas, ColorBlack);

    if(!m->has_data || !s->running) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignCenter, "waking the watchdog...");
        return;
    }

    // --- Three band meters ---
    for(uint8_t i = 0; i < CERBERUS_BAND_COUNT; i++) {
        bool active = (i == s->active_band);
        cerberus_draw_meter(canvas, i, &cerberus_bands[i], &s->band[i], active, blink);
    }
    canvas_set_color(canvas, ColorBlack);

    // --- Strongest live threat across the heads ---
    CerberusThreat worst = CerberusThreatNone;
    uint8_t worst_band = 0;
    for(uint8_t i = 0; i < CERBERUS_BAND_COUNT; i++) {
        if(s->band[i].threat > worst) {
            worst = s->band[i].threat;
            worst_band = i;
        }
    }

    // --- Status bar ---
    if(worst != CerberusThreatNone) {
        if(blink) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_box(canvas, 0, 51, 128, 13);
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_frame(canvas, 0, 51, 128, 13);
        }
        canvas_set_font(canvas, FontPrimary);
        char banner[26];
        snprintf(
            banner,
            sizeof(banner),
            "! %s %u !",
            cerberus_threat_name(worst),
            (unsigned)cerberus_bands[worst_band].mhz);
        canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignBottom, banner);
        canvas_set_color(canvas, ColorBlack);
    } else {
        canvas_draw_line(canvas, 0, 50, 127, 50);
        canvas_set_font(canvas, FontSecondary);

        char left[16];
        uint32_t up = s->uptime_s;
        snprintf(left, sizeof(left), "UP %02lu:%02lu", (unsigned long)(up / 60), (unsigned long)(up % 60));
        canvas_draw_str(canvas, 2, 61, left);

        char right[18];
        snprintf(
            right,
            sizeof(right),
            "%u %ub/s",
            (unsigned)cerberus_bands[s->active_band].mhz,
            (unsigned)s->band[s->active_band].bursts);
        canvas_draw_str_aligned(canvas, 127, 61, AlignRight, AlignBottom, right);
    }
}

static bool cerberus_monitor_input(InputEvent* event, void* context) {
    CerberusMonitorView* view = context;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(view->callback) view->callback(CerberusMonitorEventResetFloor, view->context);
        return true;
    }
    return false; // let Back propagate to the navigation handler
}

CerberusMonitorView* cerberus_monitor_view_alloc(void) {
    CerberusMonitorView* view = malloc(sizeof(CerberusMonitorView));
    view->callback = NULL;
    view->context = NULL;
    view->view = view_alloc();
    view_set_context(view->view, view);
    view_allocate_model(view->view, ViewModelTypeLocking, sizeof(CerberusMonitorModel));
    view_set_draw_callback(view->view, cerberus_monitor_draw);
    view_set_input_callback(view->view, cerberus_monitor_input);

    with_view_model(
        view->view,
        CerberusMonitorModel * model,
        {
            memset(model, 0, sizeof(CerberusMonitorModel));
            model->has_data = false;
        },
        true);
    return view;
}

void cerberus_monitor_view_free(CerberusMonitorView* view) {
    furi_assert(view);
    view_free(view->view);
    free(view);
}

View* cerberus_monitor_view_get_view(CerberusMonitorView* view) {
    furi_assert(view);
    return view->view;
}

void cerberus_monitor_view_set_callback(
    CerberusMonitorView* view,
    CerberusMonitorCallback callback,
    void* context) {
    furi_assert(view);
    view->callback = callback;
    view->context = context;
}

void cerberus_monitor_view_update(CerberusMonitorView* view, const CerberusSnapshot* snap) {
    furi_assert(view);
    with_view_model(
        view->view,
        CerberusMonitorModel * model,
        {
            model->snap = *snap;
            model->has_data = true;
            model->frame++;
        },
        true);
}
