module;

#include <cassert>

export module bamboo.model.base;

import std;
import bamboo.types;

namespace bamboo {

    export template <std::unsigned_integral T>
    struct Flags {
        using unpadded = void;

        T value;

        class Ref {
            friend Flags;

            T* _ptr;
            T _mask;

            Ref(T* ptr, T mask) noexcept
                : _ptr{ ptr }, _mask{ mask } {}

        public:
            operator bool() const noexcept {
                return *_ptr & _mask;
            }

            Ref& operator=(bool value) const noexcept {
                if (value) {
                    *_ptr |= _mask;
                } else {
                    *_ptr &= ~_mask;
                }
                return *this;
            }
        };

        Ref operator[](usize index) noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return { &value, static_cast<T>(T{ 1 } << index) };
        }

        bool operator[](usize index) const noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return value & static_cast<T>(T{ 1 } << index);
        }
    };

    // COLORREF
    export using Color = u32;

    // LCID
    export using Language = u32;

    // BITMAPINFOHEADER
    export struct BitmapHeader {
        using unpadded = void;

        u32 size;
        i32 width;
        i32 height;
        u16 planes;
        u16 bit_count;
        u32 compression;
        u32 size_image;
        i32 x_pixels_per_meter;
        i32 y_pixels_per_meter;
        u32 color_used;
        u32 color_important;
    };

    // LOGFONTW
    export struct FontDesc {
        i32 height;
        i32 width;
        i32 escapement;
        i32 orientation;
        i32 weight;
        u8 italic;
        u8 underline;
        u8 strike_out;
        u8 charset;
        u8 out_precision;
        u8 clip_precision;
        u8 quality;
        u8 pitch_and_family;
        std::wstring face_name;
    };

    // PALETTEENTRY
    export struct PaletteEntry {
        using unpadded = void;

        u8 red;
        u8 green;
        u8 blue;
        u8 flags;
    };

    // LOGPALETTE
    export struct Palette {
        u16 version;
        std::vector<PaletteEntry> palette_entry;
    };

}
