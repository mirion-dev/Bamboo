export module bamboo.model.resource;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct Font {
        u32 handle;
        u32 checksum;
        u32 references;
        LogicalFont data;
    };

    export struct FontBank : std::vector<Font> {};

    export struct Sound {
        enum Flag {
            wave,
            midi,
            _2,
            _3,
            load_on_call,   // Options
            play_from_disk, // Options
            file,
            unicode_file,
            _8,
            _9,
            _10,
            _11,
            loaded
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
            _5,
            ace,
            rgba
        };

        enum class Format : i8 {
            rgba8888,
            rgba4444,
            rgba5551,
            palette,
            rgb888_masked,
            jpeg,
            rgb555_masked,
            rgb565_masked,
            rgba8888_masked,
            argb8888
        };

        u32 handle;
        u32 checksum;
        u32 references;
        i32 size;
        i16 width;
        i16 height;
        Format format;
        Flags<u8> flags;
        i16 origin_x;
        i16 origin_y;
        i16 action_x;
        i16 action_y;
        Color transparent_color;
        std::vector<char> data;
    };

    export struct ImageBank : std::vector<Image> {
        LogicalPalette palette;
    };

}
