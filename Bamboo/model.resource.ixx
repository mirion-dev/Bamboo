export module bamboo.model.resource;

import std;
import bamboo.types;
import bamboo.model.base;

namespace bamboo {

    export struct PreviewImage {
        i32 size;
        BitmapInfoHeader header;
        std::vector<char> data;
    };

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

        enum class Format: i8 {
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

}
