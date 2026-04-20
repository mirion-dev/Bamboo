export module bamboo.general;

import std;
import bamboo.types;
import bamboo.utils;

namespace bamboo {

    export struct Color {
        using is_dense_layout = void;

        u8 r, g, b, a;
    };

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;
        std::wstring app_name;
        std::wstring editor_filename;
    };

    export struct Font {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        i32 height;
        i32 width;
        i32 escapement;
        i32 orientation;
        i32 weight;
        u8 italic;
        u8 underline;
        u8 strike_out;
        i8 charset;
        i8 out_precision;
        i8 clip_precision;
        i8 quality;
        i8 pitch_family;
        std::wstring name;
    };

    export struct FontBank : std::vector<Font> {};

    export struct Sound {
        enum {
            check,
            load_on_call   = 4,
            play_from_disk = 5,
            has_name       = 8,
            name_crop      = 14
        };

        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        Flags<u32> flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct SoundBank : std::vector<Sound> {};

    export struct Music {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        Flags<u32> flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct MusicBank : std::vector<Music> {};

    export struct Image {
        enum {
            rle,
            rlew,
            rlet,
            lzx,
            alpha,
            ace,
            mac,
            rgba
        };

        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        i16 width;
        i16 height;
        i8 graphic_mode;
        Flags<u8> flags;
        i16 hotspot_x;
        i16 hotspot_y;
        i16 action_x;
        i16 action_y;
        Color transparent_color;
        std::vector<char> data;
    };

    export struct ImageBank : std::vector<Image> {
        i32 graphic_mode;
        i16 palette_version;
        std::vector<u32> palette;
    };

    export struct Control {
        i32 type;
        std::vector<i32> data;
    };

    export struct Setting {
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
        std::vector<std::wstring> binary_files;
        std::vector<Control> controls;
        //Menu menu;
        i32 window_menu;
        std::vector<i64> menu_images;
        //std::vector<Value> global_numbers;
        //std::vector<Value> global_strings;
        //Events<true> global_events;
        i32 graphic_mode;
        std::vector<u32> icons;
        //std::vector<Qualifier> qualifiers;
        //std::vector<Extension> extensions;
    };

    export struct File {
        Header header;
        FontBank font_bank;
        SoundBank sound_bank;
        MusicBank music_bank;
        ImageBank icon_bank;
        ImageBank image_bank;
        Setting setting;
    };

}
