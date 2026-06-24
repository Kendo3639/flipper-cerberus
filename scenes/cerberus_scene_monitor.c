#include "../cerberus_i.h"

// Bridges an OK press on the monitor view to a noise-floor recalibration.
static void cerberus_scene_monitor_view_callback(CerberusMonitorEvent event, void* context) {
    Cerberus* app = context;
    if(event == CerberusMonitorEventResetFloor) {
        cerberus_subghz_reset_floor(app->worker);
        notification_message(app->notifications, &sequence_blink_blue_100);
    }
}

void cerberus_scene_monitor_on_enter(void* context) {
    Cerberus* app = context;

    cerberus_monitor_view_set_callback(app->monitor, cerberus_scene_monitor_view_callback, app);

    // Push the current config to the radio and unleash the worker.
    cerberus_apply_config(app);
    cerberus_subghz_start(app->worker);

    view_dispatcher_switch_to_view(app->view_dispatcher, CerberusViewMonitor);
}

bool cerberus_scene_monitor_on_event(void* context, SceneManagerEvent event) {
    Cerberus* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        CerberusSnapshot snap;
        cerberus_subghz_get_snapshot(app->worker, &snap);
        cerberus_monitor_view_update(app->monitor, &snap);
        consumed = true;
    }

    return consumed;
}

void cerberus_scene_monitor_on_exit(void* context) {
    Cerberus* app = context;
    cerberus_subghz_stop(app->worker);
    cerberus_monitor_view_set_callback(app->monitor, NULL, NULL);
}
