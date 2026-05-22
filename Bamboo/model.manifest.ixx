export module bamboo.model.manifest;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct BinaryFiles : std::vector<std::wstring> {};

    export struct Control {
        i32 type;
        std::vector<i32> keys;
    };

    export struct Controls : std::vector<Control> {};

    export struct MenuItem;

    export struct MenuItems : std::vector<MenuItem> {};

    struct MenuItem {
        enum Flag {
            disabled,
            checked = 3,
            parent,
            footer = 7
        };

        Flags<u16> flags;
        i16 id;
        std::wstring name;
        MenuItems items;
    };

    export struct MenuAccel {
        i8 shift;
        i16 key;
        i16 id;
    };

    export struct MenuAccels : std::vector<MenuAccel> {};

    export struct Menu {
        i32 size;
        i32 header_size;
        i32 item_offset;
        i32 item_size;
        i32 accel_offset;
        i32 accel_size;
        MenuItems items;
        MenuAccels accels;
        i32 window_menu;
        std::vector<i64> images;
    };

    export struct GlobalEvents {
        i32 size;
    };

    export struct Qualifier {
        std::wstring name;
        u32 handle;
    };

    export struct Qualifiers : std::vector<Qualifier> {};

    export struct Extension {
        u32 handle;
        std::wstring filename;
        std::wstring name;
        i32 magic_num;
        std::wstring subtype;
        i32 is_unicode;
    };

    export struct Extensions : std::vector<Extension> {};

    export struct Manifest {
        enum DisplayFlag {
            maximized_on_boot,
            resize_display,
            change_resolution_mode,
            allow_fullscreen_switch,
            heading,
            heading_when_maximized,
            menu_bar,
            menu_displayed,
            no_minimize_box,
            no_maximize_box,
            no_thick_frame,
            dont_center_frame,
            disable_close_button,
            hidden_at_start,
            keep_screen_ratio = 15,
            anti_aliasing,
            right_to_left_reading = 18,
            right_to_left_layout,
            fit_inside = 21
        };

        enum GraphicFlag {
            multi_samples,
            machine_independent_speed,
            play_sounds_over_frames,
            dont_mute_on_lost_focus,
            dont_stop_screen_saver,
            enable_visual_themes = 7,
            v_sync,
            run_when_minimized,
            run_while_resizing,
            debugger_shortcuts,
            dont_show_debugger,
            dont_share_sub_app_data,
            direct3d9,
            direct3d8,
            dont_ignore_destroy_far = 21,
            disable_ime,
            reduce_cpu_usage,
            enable_profiling = 26,
            dont_start_profiling,
            direct3d11,
            premultiplied_alpha,
            dont_optimize_events,
            record_slowest_loops
        };

        std::wstring app_name;
        std::wstring author;
        std::wstring description;
        std::wstring copyright;
        std::wstring company;
        std::wstring version;
        i32 app_width;
        i32 app_height;
        Color border_color;
        Flags<u32> display_flags;
        Flags<u32> graphic_flags;
        std::wstring help_file;
        i32 score;
        i32 lives;
        i32 frame_rate;
        i32 build_type;
        std::wstring build_filename;
        std::wstring about;
        BinaryFiles binary_files;
        Controls controls;
        Menu menu;
        Values global_numbers;
        Values global_strings;
        GlobalEvents global_events;
        i32 graphic_mode;
        std::vector<u32> icons;
        Qualifiers qualifiers;
        Extensions extensions;
    };

}
