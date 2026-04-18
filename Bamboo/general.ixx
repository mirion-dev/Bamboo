export module bamboo.general;

import std;
import bamboo.core;

namespace bamboo {

    export struct Color {
        using dense_layout = void;

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
        static constexpr u32 DECOMPRESSED{ 1 << 6 };

        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        u32 flags;
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
        u32 flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct MusicBank : std::vector<Music> {};

    export struct Image {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        i16 width;
        i16 height;
        i8 graphic_mode;
        u8 flags;
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
        std::wstring app_name;
        std::wstring author;
        std::wstring description;
        std::wstring copyright;
        std::wstring company;
        std::wstring version;
        i32 app_width;
        i32 app_height;
        Color border_color;
        u32 display_flags;
        u32 graphic_flags;
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
