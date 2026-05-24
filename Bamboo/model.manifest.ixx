export module bamboo.model.manifest;

import std;
import bamboo.types;
import bamboo.model.base;
import bamboo.model.event;

namespace bamboo {

    export struct BinaryFiles : std::vector<std::wstring> {};

    export struct Control {
        enum Type : i32 {
            _0,
            joystick1,
            joystick2,
            joystick3,
            joystick4,
            keyboard
        };

        Type type;
        std::vector<i32> keys;
    };

    export struct Controls : std::vector<Control> {};

    export struct MenuItem;

    export struct MenuItems : std::vector<MenuItem> {};

    struct MenuItem {
        enum Flag {
            grayed, // Options
            _1,
            _2,
            checked, // Options
            parent,
            _5,
            _6,
            last
        };

        Flags<u16> flags;
        u16 id;
        std::wstring name;
        MenuItems children;
    };

    export struct MenuAccel {
        enum Flag {
            _0,
            _1,
            shift, // Accelerator
            ctrl,  // Accelerator
            alt,   // Accelerator
            _5,
            _6,
            last
        };

        Flags<u16> flags;
        i16 key;
        u16 id;
    };

    export struct MenuAccels : std::vector<MenuAccel> {};

    export struct MenuImage {
        u16 id;
        u32 image; // Bitmap
    };

    export struct MenuImages : std::vector<MenuImage> {};

    export struct MenuBar {
        i32 size;
        i32 header_size;
        i32 item_offset;
        i32 item_size;
        i32 accel_offset;
        i32 accel_size;
        MenuItems items;
        MenuAccels accels;
        i32 window_menu_index;
        MenuImages images;
    };

    export struct Qualifier {
        std::wstring name;
        u32 icon;
    };

    export struct Qualifiers : std::vector<Qualifier> {};

    export struct Extension {
        u32 handle;
        std::wstring filename;
        std::wstring name;
        u32 magic_num;
        std::wstring subtype;
        i32 is_unicode;
    };

    export struct Extensions : std::vector<Extension> {};

    export struct Manifest {
        enum Flag {
            maximized_on_boot_up,                    // Window
            resize_display_to_fill_window_size,      // Window
            change_resolution_mode,                  // Window
            allow_user_to_switch_to_from_fullscreen, // Window
            heading,                                 // Window
            heading_when_maximized,                  // Window
            has_menu_bar,                            // Window
            menu_displayed_on_boot_up,               // Window
            no_minimize_box,                         // Window
            no_maximize_box,                         // Window
            no_thick_frame,                          // Window
            do_not_center_frame_area_in_window,      // Window
            disable_close_button,                    // Window
            hidden_at_start,                         // Window
            multiple_document_interface,             // Window
            keep_screen_ratio,                       // Window
            antialiasing_when_resizing,              // Window
            _17,
            right_to_left_reading, // Window
            right_to_left_layout,  // Window
            _20,
            fit_inside, // Window
            _22,
            _23,
            _24,
            _25,
            _26,
            _27,
            _28,
            _29,
            _30,
            _31,
            multisamples,                                    // Runtime
            machine_independent_speed,                       // Runtime
            play_sounds_over_frames,                         // Runtime
            do_not_mute_samples_when_application_lost_focus, // Runtime
            do_not_stop_screen_saver_when_input_event,       // Runtime
            _37,
            _38,
            enable_visual_themes,                       // Windows
            vsync,                                      // Runtime
            run_when_minimized,                         // Runtime
            run_while_resizing,                         // Runtime
            enable_debugger_keyboard_shortcuts,         // Settings
            do_not_show_debugger,                       // Settings
            do_not_share_data_if_run_as_subapplication, // Runtime
            direct3d9,                                  // Runtime
            direct3d8,                                  // Runtime
            _48,
            _49,
            _50,
            _51,
            _52,
            do_not_ignore_destroy_if_too_far_option_if_inactive_if_too_far_is_set_to_no, // Runtime
            disable_ime,                                                                 // Windows
            reduce_cpu_usage,                                                            // Windows
            _56,
            use_high_performance_gpu,                 // Windows
            enable_profiling,                         // Settings
            do_not_start_profiling_at_start_of_frame, // Settings
            direct3d11,                               // Runtime
            premultiplied_alpha,                      // Runtime
            do_not_optimize_events,                   // Settings
            record_slowest_app_loops                  // Settings
        };

        std::wstring app_name;    // About
        std::wstring author;      // About
        std::wstring description; // About
        std::wstring copyright;   // About
        std::wstring company;     // About
        std::wstring version;     // About
        i32 window_width;         // Window
        i32 window_height;        // Window
        Color border_color;       // Window
        Flags<u64> options;
        std::wstring help_file;          // About
        i32 init_score;                  // Runtime
        i32 init_lives;                  // Runtime
        i32 frame_rate;                  // Runtime
        i32 build_type;                  // Settings
        std::wstring build_filename;     // Settings
        std::wstring command_line;       // Settings
        std::wstring about;              // About
        BinaryFiles binary_files;        // Data Elements - Binary Data
        Controls controls;               // Runtime
        MenuBar menu_bar;                // Window
        Values global_numbers;           // Values
        Values global_strings;           // Values
        GlobalEventBlocks global_events; // Events
        i32 graphic_mode;                // Settings
        std::vector<u32> window_icons;   // About
        Qualifiers qualifiers;           // Events
        Extensions extensions;           // Data Elements - Extensions
    };

}
