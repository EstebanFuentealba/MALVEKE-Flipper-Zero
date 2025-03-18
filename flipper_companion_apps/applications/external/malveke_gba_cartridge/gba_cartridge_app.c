#include "gba_cartridge_app.h"

bool gba_cartridge_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    GBACartridge* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void gba_cartridge_app_tick_event_callback(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

// Exit the app if the back button is pressed
bool gba_cartridge_app_navigation_event_callback(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

GBACartridge* gba_cartridge_app_app_alloc() {
    GBACartridge* app = malloc(sizeof(GBACartridge));
    furi_assert(app != NULL); // Ensure allocation succeeded
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn on backlight to facilitate testing
    notification_message(app->notification, &sequence_display_backlight_on);

    // Set up the view dispatcher and scene manager
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&gba_cartridge_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, gba_cartridge_app_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, gba_cartridge_app_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, gba_cartridge_app_custom_event_callback);
    app->submenu = variable_item_list_alloc();

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    // Initialize UART interfaces
    app->uart = usart_init(app);
    app->lp_uart = lp_uart_init(app);

    // Add views to the dispatcher
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBACartridgeViewIdMenu,
        variable_item_list_get_view(app->submenu));
    app->gba_cartridge_startscreen = gba_cartridge_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBACartridgeViewIdStartscreen,
        gba_cartridge_startscreen_get_view(app->gba_cartridge_startscreen));
    app->gba_cartridge_scene_1 = gba_cartridge_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBACartridgeViewIdScene1,
        gba_cartridge_scene_1_get_view(app->gba_cartridge_scene_1));

    // Optionally enable 5V power if required (experimental)
#ifdef ENABLE_5V
    furi_hal_power_enable_otg();
    furi_delay_ms(1);
#endif
    return app;
}

void gba_cartridge_app_app_free(GBACartridge* app) {
    furi_assert(app);
    // Remove views from the dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdStartscreen);
    variable_item_list_free(app->submenu);

    // Free the view dispatcher and scene manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Free UART interfaces
    uart_free(app->uart);
    uart_free(app->lp_uart);

    // Close system records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    app->gui = NULL;
    app->notification = NULL;
    app->storage = NULL;

    furi_string_free(app->file_path);

    // Free the main application structure
    free(app);
}

int32_t gba_cartridge_app(void* p) {
    UNUSED(p);
    // Disable the expansion protocol to avoid interference with UART handling
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    GBACartridge* app = gba_cartridge_app_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Start with the start screen scene
    scene_manager_next_scene(app->scene_manager, GBACartridgeSceneStartscreen);

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    gba_cartridge_app_app_free(app);

    // Restore the expansion protocol state
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
