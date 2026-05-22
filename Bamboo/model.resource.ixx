export module bamboo.model.resource;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Font {
        u32 handle;
        u32 checksum;
        u32 references;
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
        enum Flag {
            check,
            load_on_call   = 4,
            play_from_disk = 5,
            has_name       = 8,
            name_crop      = 14
        };

        u32 handle;
        u32 checksum;
        u32 references;
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
        u32 references;
        i32 size;
        Flags<u32> flags;
        i32 frequency;
        std::wstring name;
        std::vector<char> data;
    };

    export struct MusicBank : std::vector<Music> {};

    export struct Image {
        enum Flag {
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
        u32 references;
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

    export struct Resources {
        FontBank font_bank;
        SoundBank sound_bank;
        MusicBank music_bank;
        ImageBank icon_bank;
        ImageBank image_bank;
    };

}
