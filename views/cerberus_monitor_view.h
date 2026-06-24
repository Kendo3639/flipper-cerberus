/**
 * Cerberus - live monitor view.
 *
 * Draws the watch screen: a title bar, one signal meter per watched band
 * (433 / 868 / 915), and a status bar that flips to a flashing alert banner the
 * moment a head starts barking.
 */
#pragma once

#include <gui/view.h>
#include "../helpers/cerberus_subghz.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CerberusMonitorView CerberusMonitorView;

typedef enum {
    CerberusMonitorEventResetFloor, // user asked to recalibrate (OK)
} CerberusMonitorEvent;

typedef void (*CerberusMonitorCallback)(CerberusMonitorEvent event, void* context);

CerberusMonitorView* cerberus_monitor_view_alloc(void);
void cerberus_monitor_view_free(CerberusMonitorView* view);
View* cerberus_monitor_view_get_view(CerberusMonitorView* view);

void cerberus_monitor_view_set_callback(
    CerberusMonitorView* view,
    CerberusMonitorCallback callback,
    void* context);

/** Push a fresh radio snapshot in for drawing. */
void cerberus_monitor_view_update(CerberusMonitorView* view, const CerberusSnapshot* snap);

#ifdef __cplusplus
}
#endif
