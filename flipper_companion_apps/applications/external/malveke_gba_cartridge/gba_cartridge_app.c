#include "gba_cartridge_app.h"

// Existing functions…
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

// Leave app if back button pressed
bool gba_cartridge_app_navigation_event_callback(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

GBACartridge* gba_cartridge_app_app_alloc() {
    GBACartridge* app = malloc(sizeof(GBACartridge));
    furi_assert(app != NULL);
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on to facilitate testing
    notification_message(app->notification, &sequence_display_backlight_on);

    // Scene additions
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

    app->uart = usart_init(app);
    app->lp_uart = lp_uart_init(app);

    view_dispatcher_add_view(app->view_dispatcher, GBACartridgeViewIdMenu, variable_item_list_get_view(app->submenu));
    app->gba_cartridge_startscreen = gba_cartridge_startscreen_alloc();
    view_dispatcher_add_view(app->view_dispatcher, GBACartridgeViewIdStartscreen, gba_cartridge_startscreen_get_view(app->gba_cartridge_startscreen));
    app->gba_cartridge_scene_1 = gba_cartridge_scene_1_alloc();
    view_dispatcher_add_view(app->view_dispatcher, GBACartridgeViewIdScene1, gba_cartridge_scene_1_get_view(app->gba_cartridge_scene_1));

    // Optionally enable 5V power if required (experimental)
#ifdef ENABLE_5V
    furi_hal_power_enable_otg();
    furi_delay_ms(1);
#endif
    return app;
}

void gba_cartridge_app_app_free(GBACartridge* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdStartscreen);
    variable_item_list_free(app->submenu);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Free UART interfaces
    uart_free(app->uart);
    uart_free(app->lp_uart);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    app->gui = NULL;
    app->notification = NULL;
    app->storage = NULL;

    furi_string_free(app->file_path);

    free(app);
}

int32_t gba_cartridge_app(void* p) {
    UNUSED(p);
    // Disable expansion protocol to avoid interference with UART handling
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    GBACartridge* app = gba_cartridge_app_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, GBACartridgeSceneStartscreen); // Start with start screen

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    gba_cartridge_app_app_free(app);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}

/* ===================== NEW FUNCTIONALITY =====================
   Backup and Restoration for GBA Saves (RAM) and Games (ROM)
   This follows the same conventions as the GB/GBC app.
   Assumed external helper functions and types:
   - CartridgeInfo structure with fields: title, rom_size, ram_size.
   - gba_cartridge_get_info(GBACartridge* app, CartridgeInfo* info);
   - UART command functions: uart_send_command(), uart_read(), uart_write().
   - File system functions: storage_file_open(), storage_file_write(), storage_file_read(), storage_file_close().
   - Notification messages via notification_message().
   ============================================================= */

// Assume these helper functions and types exist:
typedef struct {
    char title[32];
    size_t rom_size;
    size_t ram_size;
} CartridgeInfo;

extern bool gba_cartridge_get_info(GBACartridge* app, CartridgeInfo* info);
extern bool uart_send_command(Usart* uart, const char* command);
extern int uart_read(Usart* uart, uint8_t* buffer, size_t length, uint32_t timeout_ms);
extern int uart_write(Usart* uart, const uint8_t* buffer, size_t length);

// File system helper API (assumed to be provided by the MALVEKE framework)
typedef struct {
    // Implementation-specific file handle
} FileHandle;

extern FileHandle* storage_file_open(Storage* storage, const char* path, const char* mode);
extern int storage_file_write(FileHandle* file, const uint8_t* buffer, size_t length);
extern int storage_file_read(FileHandle* file, uint8_t* buffer, size_t length);
extern void storage_file_close(FileHandle* file);

// Helper function to construct file paths for backups/restores
static void gba_construct_file_path(char* out_path, size_t max_len, const char* folder, const char* title, const char* ext) {
    snprintf(out_path, max_len, "/sd/apps_data/malveke/%s/%s%s", folder, title, ext);
}

// NEW: Function to backup ROM (game data)
bool gba_cartridge_backup_rom(GBACartridge* app) {
    furi_assert(app);

    CartridgeInfo info;
    if(!gba_cartridge_get_info(app, &info)) {
        notification_message(app->notification, "Error: Failed to get cartridge info");
        return false;
    }
    
    char file_path[128];
    gba_construct_file_path(file_path, sizeof(file_path), "roms", info.title, ".gba");

    FileHandle* file = storage_file_open(app->storage, file_path, "wb");
    if(!file) {
        notification_message(app->notification, "Error: Unable to open ROM backup file");
        return false;
    }
    
    // Send command to initiate ROM backup using existing protocol
    if(!uart_send_command(app->uart, "gba_backup_rom")) {
        notification_message(app->notification, "Error: Failed to initiate ROM backup");
        storage_file_close(file);
        return false;
    }
    
    size_t total_bytes = info.rom_size;
    size_t chunk_size = 512;
    uint8_t buffer[512];
    size_t bytes_received = 0;
    
    while(bytes_received < total_bytes) {
        size_t bytes_to_read = (total_bytes - bytes_received < chunk_size) ? (total_bytes - bytes_received) : chunk_size;
        int ret = uart_read(app->uart, buffer, bytes_to_read, 1000); // 1000 ms timeout
        if(ret != (int)bytes_to_read) {
            notification_message(app->notification, "Error: Incomplete ROM data received");
            storage_file_close(file);
            return false;
        }
        if(storage_file_write(file, buffer, bytes_to_read) != (int)bytes_to_read) {
            notification_message(app->notification, "Error: Writing ROM backup failed");
            storage_file_close(file);
            return false;
        }
        bytes_received += bytes_to_read;
        // Optionally update progress indicator here
    }
    
    storage_file_close(file);
    notification_message(app->notification, "ROM backup completed successfully");
    return true;
}

// NEW: Function to backup RAM (save data)
bool gba_cartridge_backup_ram(GBACartridge* app) {
    furi_assert(app);

    CartridgeInfo info;
    if(!gba_cartridge_get_info(app, &info)) {
        notification_message(app->notification, "Error: Failed to get cartridge info");
        return false;
    }
    
    char file_path[128];
    gba_construct_file_path(file_path, sizeof(file_path), "rams", info.title, ".sav");

    FileHandle* file = storage_file_open(app->storage, file_path, "wb");
    if(!file) {
        notification_message(app->notification, "Error: Unable to open RAM backup file");
        return false;
    }
    
    if(!uart_send_command(app->uart, "gba_backup_ram")) {
        notification_message(app->notification, "Error: Failed to initiate RAM backup");
        storage_file_close(file);
        return false;
    }
    
    size_t total_bytes = info.ram_size;
    size_t chunk_size = 256;
    uint8_t buffer[256];
    size_t bytes_received = 0;
    
    while(bytes_received < total_bytes) {
        size_t bytes_to_read = (total_bytes - bytes_received < chunk_size) ? (total_bytes - bytes_received) : chunk_size;
        int ret = uart_read(app->uart, buffer, bytes_to_read, 1000);
        if(ret != (int)bytes_to_read) {
            notification_message(app->notification, "Error: Incomplete RAM data received");
            storage_file_close(file);
            return false;
        }
        if(storage_file_write(file, buffer, bytes_to_read) != (int)bytes_to_read) {
            notification_message(app->notification, "Error: Writing RAM backup failed");
            storage_file_close(file);
            return false;
        }
        bytes_received += bytes_to_read;
        // Optionally update progress indicator here
    }
    
    storage_file_close(file);
    notification_message(app->notification, "RAM backup completed successfully");
    return true;
}

// NEW: Function to restore RAM (save data) to the cartridge
bool gba_cartridge_restore_ram(GBACartridge* app) {
    furi_assert(app);

    CartridgeInfo info;
    if(!gba_cartridge_get_info(app, &info)) {
        notification_message(app->notification, "Error: Failed to get cartridge info");
        return false;
    }
    
    char file_path[128];
    gba_construct_file_path(file_path, sizeof(file_path), "rams", info.title, ".sav");

    FileHandle* file = storage_file_open(app->storage, file_path, "rb");
    if(!file) {
        notification_message(app->notification, "Error: Unable to open RAM restore file");
        return false;
    }
    
    if(!uart_send_command(app->uart, "gba_restore_ram")) {
        notification_message(app->notification, "Error: Failed to initiate RAM restore");
        storage_file_close(file);
        return false;
    }
    
    size_t total_bytes = info.ram_size;
    size_t chunk_size = 256;
    uint8_t buffer[256];
    size_t bytes_sent = 0;
    
    while(bytes_sent < total_bytes) {
        size_t bytes_to_send = (total_bytes - bytes_sent < chunk_size) ? (total_bytes - bytes_sent) : chunk_size;
        int read_bytes = storage_file_read(file, buffer, bytes_to_send);
        if(read_bytes != (int)bytes_to_send) {
            notification_message(app->notification, "Error: Reading RAM restore file failed");
            storage_file_close(file);
            return false;
        }
        if(uart_write(app->uart, buffer, bytes_to_send) != (int)bytes_to_send) {
            notification_message(app->notification, "Error: Sending RAM restore data failed");
            storage_file_close(file);
            return false;
        }
        bytes_sent += bytes_to_send;
        // Optionally update progress indicator here
    }
    
    storage_file_close(file);
    notification_message(app->notification, "RAM restore completed successfully");
    return true;
}

// NEW: Function to restore ROM (game data) to the cartridge
bool gba_cartridge_restore_rom(GBACartridge* app) {
    furi_assert(app);

    CartridgeInfo info;
    if(!gba_cartridge_get_info(app, &info)) {
        notification_message(app->notification, "Error: Failed to get cartridge info");
        return false;
    }
    
    char file_path[128];
    gba_construct_file_path(file_path, sizeof(file_path), "roms", info.title, ".gba");

    FileHandle* file = storage_file_open(app->storage, file_path, "rb");
    if(!file) {
        notification_message(app->notification, "Error: Unable to open ROM restore file");
        return false;
    }
    
    if(!uart_send_command(app->uart, "gba_restore_rom")) {
        notification_message(app->notification, "Error: Failed to initiate ROM restore");
        storage_file_close(file);
        return false;
    }
    
    size_t total_bytes = info.rom_size;
    size_t chunk_size = 512;
    uint8_t buffer[512];
    size_t bytes_sent = 0;
    
    while(bytes_sent < total_bytes) {
        size_t bytes_to_send = (total_bytes - bytes_sent < chunk_size) ? (total_bytes - bytes_sent) : chunk_size;
        int read_bytes = storage_file_read(file, buffer, bytes_to_send);
        if(read_bytes != (int)bytes_to_send) {
            notification_message(app->notification, "Error: Reading ROM restore file failed");
            storage_file_close(file);
            return false;
        }
        if(uart_write(app->uart, buffer, bytes_to_send) != (int)bytes_to_send) {
            notification_message(app->notification, "Error: Sending ROM restore data failed");
            storage_file_close(file);
            return false;
        }
        bytes_sent += bytes_to_send;
        // Optionally update progress indicator here
    }
    
    storage_file_close(file);
    notification_message(app->notification, "ROM restore completed successfully");
    return true;
}
