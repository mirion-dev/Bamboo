export module bamboo.model.resource;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    // cFontElement
    export struct Font {
        u32 handle;
        u32 checksum;
        u32 ref_count;
        FontDesc data;
    };

    // cFontBank
    export struct FontBank : std::vector<Font> {};

    // cSoundElement
    export struct Sound {
        enum Flag {
            wave, // SP_* (SDK)
            midi, // SP_* (SDK)
            _2,
            _3,
            load_on_call,   // Options
            play_from_disk, // Options
            file,           // SNDF_* (SDK)
            unicode_file,   // SNDF_* (SDK)
            has_name,
            _9,
            _10,
            _11,
            loaded, // SNDF_* (SDK)
            _13,
            name_crop
        };

        u32 handle;
        u32 checksum;
        u32 ref_count;
        i32 size;
        Flags<u32> flags;
        std::wstring name;
        std::vector<unsigned char> data;
    };

    export struct Sample : Sound {};

    export struct Music : Sound {};

    // cSampleBank
    export struct SampleBank : std::vector<Sample> {};

    // cMusicBank
    export struct MusicBank : std::vector<Music> {};

    // cImageElement
    export struct Image {
        enum Flag {
            rle,
            rlew,
            rlet,
            lzx,
            alpha,
            _5,
            ace,
            mac
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
        u32 ref_count;
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
        std::vector<unsigned char> data;
    };

    // cImageBank
    export struct ImageBank : std::vector<Image> {
        Palette palette;
    };

}
