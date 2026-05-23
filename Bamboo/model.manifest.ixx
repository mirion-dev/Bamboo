export module bamboo.model.manifest;

import std;
import bamboo.types;
import bamboo.model.base;
import bamboo.model.event;

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
            _1,
            _2,
            checked,
            parent,
            _5,
            _6,
            last
        };

        Flags<u16> flags;
        i16 id;
        std::wstring name;
        MenuItems children;
    };

    export struct MenuAccel {
        i8 modifier;
        i16 key;
        i16 id;
    };

    export struct MenuAccels : std::vector<MenuAccel> {};

    export struct MenuBar {
        i32 size;
        i32 header_size;
        i32 item_offset;
        i32 item_size;
        i32 accel_offset;
        i32 accel_size;
        MenuItems items;
        MenuAccels accels;
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
        enum Flag {
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
            _14,
            keep_screen_ratio,
            antialiasing_when_resizing,
            _17,
            right_to_left_reading,
            right_to_left_layout,
            _20,
            fit_inside,
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
            multisamples,
            machine_independent_speed,
            play_sounds_over_frames,
            do_not_mute_samples_when_application_lost_focus,
            do_not_stop_screen_saver_when_input_event,
            _37,
            _38,
            enable_visual_themes,
            vsync,
            run_when_minimized,
            run_while_resizing,
            enable_debugger_keyboard_shortcuts,
            show_debugger,
            do_not_share_data_if_run_as_subapplication,
            direct3d9,
            direct3d8,
            _48,
            _49,
            _50,
            _51,
            _52,
            ignore_destroy_if_too_far_option_if_inactive_if_too_far_is_set_to_no,
            disable_ime,
            reduce_cpu_usage,
            _56,
            _57,
            enable_profiling,
            start_profiling_at_start_of_frame,
            direct3d11,
            premultiplied_alpha,
            optimize_events,
            record_slowest_app_loops
        };

        std::wstring app_name;
        std::wstring author;
        std::wstring description;
        std::wstring copyright;
        std::wstring company;
        std::wstring version;
        i32 window_width;
        i32 window_height;
        Color border_color;
        Flags<u64> options;
        std::wstring help_file;
        i32 init_score;
        i32 init_lives;
        i32 frame_rate;
        i32 build_type;
        std::wstring build_filename;
        std::wstring about;
        BinaryFiles binary_files;
        Controls controls;
        MenuBar menu_bar;
        i32 window_menu;
        std::vector<i64> menu_images;
        Values global_numbers;
        Values global_strings;
        GlobalEventBlocks global_events;
        i32 graphic_mode;
        std::vector<u32> window_icons;
        Qualifiers qualifiers;
        Extensions extensions;
    };

}
