export module bamboo.model;

import std;
import bamboo.types;

export import bamboo.model.base;
export import bamboo.model.resource;
export import bamboo.model.event;
export import bamboo.model.object;
export import bamboo.model.frame;

namespace bamboo {

    export struct PreviewImage {
        i32 size;
        BitmapInfoHeader header;
        std::vector<char> data;
    };

    export struct BinaryFiles : std::vector<std::wstring> {};

    export struct Control {
        enum class Type : i32 {
            mouse,
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
            virtual_key,
            no_invert,
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

    export struct Project {
        enum WindowFlag {
            maximized_on_boot_up,
            resize_display_to_fill_window_size,
            change_resolution_mode,
            allow_user_to_switch_to_from_fullscreen,
            heading,
            heading_when_maximized,
            has_menu_bar,
            menu_displayed_on_boot_up,
            no_minimize_box,
            no_maximize_box,
            no_thick_frame,
            do_not_center_frame_area_in_window,
            disable_close_button,
            hidden_at_start,
            multiple_document_interface,
            keep_screen_ratio,
            antialiasing_when_resizing,
            _w17,
            right_to_left_reading,
            right_to_left_layout,
            _w20,
            fit_inside
        };

        enum Flag {
            multisamples,                                    // Runtime
            machine_independent_speed,                       // Runtime
            play_sounds_over_frames,                         // Runtime
            do_not_mute_samples_when_application_lost_focus, // Runtime
            do_not_stop_screen_saver_when_input_event,       // Runtime
            _5,
            _6,
            enable_visual_themes,                       // Windows
            vsync,                                      // Runtime
            run_when_minimized,                         // Runtime
            run_while_resizing,                         // Runtime
            enable_debugger_keyboard_shortcuts,         // Settings
            do_not_show_debugger,                       // Settings
            do_not_share_data_if_run_as_subapplication, // Runtime
            display_mode_direct3d9,                     // Runtime
            display_mode_direct3d8,                     // Runtime
            _16,
            _17,
            _18,
            _19,
            _20,
            do_not_ignore_destroy_if_too_far_option_if_inactive_if_too_far_is_set_to_no, // Runtime
            disable_ime,                                                                 // Windows
            reduce_cpu_usage,                                                            // Windows
            _24,
            use_high_performance_gpu,                 // Windows
            enable_profiling,                         // Settings
            do_not_start_profiling_at_start_of_frame, // Settings
            display_mode_direct3d11,                  // Runtime
            premultiplied_alpha,                      // Runtime
            do_not_optimize_events,                   // Settings
            record_slowest_app_loops                  // Settings
        };

        enum class BuildType : i32 {
            windows_exe_application,
            windows_screen_saver,
            subapplication,
            java_subapplication,
            java_application,
            java_internet_applet,
            java_web_start,
            java_for_mobile_devices,
            java_for_bluray,
            java_mac_application,
            adobe_flash,
            java_for_blackberry,
            android_application,
            ios_application,
            ios_xcode_project,
            final_ios_xcode_project,
            xna_windows_application,
            mac_application,
            xna_windows_project,
            xna_xbox_project,
            xna_phone_project,
            xna_xbox_application,
            xna_phone_application,
            html5,
            playstation_vita,
            playstation_vita_development,
            playstation_vita_final_project,
            html5_development,
            html5_final_project,
            ouya,
            mac_application_file,
            mac_xcode_project,
            _32,
            uwp_project,
            android_app_bundle,
            android_app_bundle_with_expansion
        };

        enum class GraphicMode : i32 {
            _0,
            _1,
            _2,
            color256,
            color16m,
            _5,
            color32768,
            color65536
        };

        i16 format_version;
        i16 format_subversion;
        i32 editor_version;
        i32 editor_build;
        LangCodeId language;      // About
        std::wstring name;        // About
        std::wstring description; // About
        std::wstring path;        // About
        PreviewImage preview_image;
        FontBank font_bank;   // Data Elements - Fonts
        SoundBank sound_bank; // Data Elements - Sounds
        MusicBank music_bank; // Data Elements - Music
        ImageBank icon_bank;
        ImageBank image_bank;
        std::wstring author;     // About
        std::wstring copyright;  // About
        std::wstring company;    // About
        std::wstring version;    // About
        i32 window_width;        // Window
        i32 window_height;       // Window
        Color border_color;      // Window
        Flags<u32> window_flags; // Window
        Flags<u32> flags;
        std::wstring help_file; // About
        std::wstring vitalize_preview_file;
        i32 init_score;              // Runtime
        i32 init_lives;              // Runtime
        i32 frame_rate;              // Runtime
        BuildType build_type;        // Settings
        std::wstring build_filename; // Settings
        std::wstring effects_folder;
        std::wstring command_line; // Settings
        std::wstring about;        // About
        std::vector<char> installer;
        BinaryFiles binary_files;        // Data Elements - Binary Data
        Controls controls;               // Runtime
        MenuBar menu_bar;                // Window
        Values global_numbers;           // Values
        Values global_strings;           // Values
        GlobalEventBlocks global_events; // Events
        GraphicMode graphic_mode;        // Settings
        std::vector<u32> window_icons;   // About
        Qualifiers qualifiers;           // Events
        Extensions extensions;           // Data Elements - Extensions
        Frames frames;
        Chunks chunks;
    };

}
