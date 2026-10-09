module;

#include <spdlog/spdlog.h>
#undef pascal

export module bamboo.mfa.base;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.log;
import bamboo.stream;
import bamboo.stream_utils;
import bamboo.model;

namespace bamboo::mfa {

    export class Stream : public bamboo::Stream {
    public:
        Application* app;

        using bamboo::Stream::Stream;
    };

    export template <class T, usize N>
    void load(Stream& stream, std::array<T, N>& value) {
        stream >> bamboo::args(value.data(), N);
    }

    export template <std::integral T>
    struct SizeType {};

    export template <std::integral T>
    constexpr SizeType<T> size_type;

    export template <class T, std::integral Size>
    void load(Stream& stream, std::vector<T>& value, Size size) {
        stream >> resize(value, size);
    }

    export template <class T, std::integral Size = i32>
    void load(Stream& stream, std::vector<T>& value, SizeType<Size> = {}) {
        Size size;
        stream >> size >> bamboo::args(value, size);
    }

    export enum class StringTypeEnum {
        pascal,
        c,
        pascal_c,
        fixed_c
    };

    export template <StringTypeEnum Type, usize N = {}>
    struct StringType {};

    export constexpr StringType<StringTypeEnum::pascal> string_type_pascal;
    export constexpr StringType<StringTypeEnum::c> string_type_c;
    export constexpr StringType<StringTypeEnum::pascal_c> string_type_pascal_c;
    export template <usize N>
    constexpr StringType<StringTypeEnum::fixed_c, N> string_type_fixed_c;

    export template <StringTypeEnum Type = {}, usize N = {}>
    void load(Stream& stream, std::wstring& value, StringType<Type, N> = {}) {
        if constexpr (Type == StringTypeEnum::pascal) {
            static constexpr u32 WIDE{ 1u << 31 };

            i32 size;
            stream >> size;
            if (size & WIDE) {
                stream >> resize(value, size & ~WIDE);
            } else {
                std::string str;
                stream >> resize(str, size);
                value = to_wstring(str);
            }
        } else if constexpr (Type == StringTypeEnum::c) {
            value.clear();

            wchar_t ch;
            while (stream >> ch, ch != '\0') {
                value.push_back(ch);
            }
        } else if constexpr (Type == StringTypeEnum::pascal_c) {
            i32 size;
            stream >> size >> resize(value, size);
            if (value.empty() || value.back() != '\0') {
                throw std::runtime_error{ "A Pascal-C string must be null-terminated" };
            }

            value.pop_back();
        } else if constexpr (Type == StringTypeEnum::fixed_c) {
            stream >> resize(value, N);
            usize end{ value.find(L'\0') };
            if (end == -1) {
                throw std::runtime_error{ "A fixed C string must be null-terminated" };
            }

            value.resize(end);
        } else {
            static_assert(false, "Unknown string type.");
        }
    }

    export template <class T>
    void load(Stream& stream, std::optional<T>& value, bool has_value) {
        if (has_value) {
            stream >> value.emplace();
        }
    }

    export template <class T, std::integral Size = u8>
    void load(Stream& stream, std::optional<T>& value, SizeType<Size> = {}) {
        Size has_value;
        stream >> has_value >> bamboo::args(value, has_value);
    }

    export void load(Stream& stream, FontDesc& value) {
        stream
            >> value.height
            >> value.width
            >> value.escapement
            >> value.orientation
            >> value.weight
            >> value.italic
            >> value.underline
            >> value.strike_out
            >> value.charset
            >> value.out_precision
            >> value.clip_precision
            >> value.quality
            >> value.pitch_and_family
            >> args(value.face_name, string_type_fixed_c<32>);
    }

    export void load(Stream& stream, Palette& value) {
        stream >> value.version >> args(value.palette_entry, size_type<u16>);
    }

    export void load(Stream& stream, MenuEntry& value) {
        stream >> value.flags;
        if (!value.flags[MenuEntry::popup]) {
            stream >> value.id;
        }
        stream >> args(value.string, string_type_c);
    }

    export void load(Stream& stream, MenuAccel& value) {
        stream >> value.flags >> value.ansi >> value.id >> skip<i16>; // Padding
    }

}
