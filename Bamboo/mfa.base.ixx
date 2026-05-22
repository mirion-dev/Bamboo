module;

#include <spdlog/spdlog.h>
#undef pascal

export module bamboo.mfa:base;

import std;
import bamboo.types;
import bamboo.diag;
import bamboo.stream;
import bamboo.stream.utils;
import bamboo.model;

namespace bamboo::mfa {

    export class Stream : public bamboo::Stream {
    public:
        i32 build{};

        using bamboo::Stream::Stream;
    };

    template <class T, usize N>
    void load(Stream& stream, std::array<T, N>& value) {
        stream >> bamboo::args(value.data(), N);
    }

    template <std::integral T>
    struct SizeType {};

    template <std::integral T>
    constexpr SizeType<T> size_type;

    template <class T>
    void load(Stream& stream, std::vector<T>& value, std::integral auto size) {
        bamboo::resize_load(stream, value, size);
    }

    template <class T, std::integral S = i32>
    void load(Stream& stream, std::vector<T>& value, SizeType<S> = {}) {
        S size;
        stream >> size >> bamboo::args(value, size);
    }

    enum class StringTypeEnum {
        pascal,
        c,
        pascal_c,
        fixed_c
    };

    template <StringTypeEnum Type, usize N = {}>
    struct StringType {};

    constexpr StringType<StringTypeEnum::pascal> string_type_pascal;
    constexpr StringType<StringTypeEnum::c> string_type_c;
    constexpr StringType<StringTypeEnum::pascal_c> string_type_pascal_c;
    template <usize N>
    constexpr StringType<StringTypeEnum::fixed_c, N> string_type_fixed_c;

    template <StringTypeEnum Type = {}, usize N = {}>
    void load(Stream& stream, std::wstring& value, StringType<Type, N> = {}) {
        if constexpr (Type == StringTypeEnum::pascal) {
            static constexpr u32 MASK_UNICODE{ 1u << 31 };

            i32 size;
            stream >> size;
            if (!(size & MASK_UNICODE)) {
                throw std::runtime_error{ "ASCII strings are unsupported." };
            }
            resize_load(stream, value, size & ~MASK_UNICODE);
        }
        else if constexpr (Type == StringTypeEnum::c) {
            value.clear();
            wchar_t ch;
            while (stream >> ch, ch != '\0') {
                value.push_back(ch);
            }
        }
        else if constexpr (Type == StringTypeEnum::pascal_c) {
            i32 size;
            stream >> size;
            resize_load(stream, value, size);
            if (value.empty() || value.back() != '\0') {
                throw std::runtime_error{ "A Pascal-C string must be null-terminated." };
            }
            value.pop_back();
        }
        else if constexpr (Type == StringTypeEnum::fixed_c) {
            resize_load(stream, value, N);
            usize end{ value.find(L'\0') };
            if (end == -1) {
                throw std::runtime_error{ "A fixed C string must be null-terminated." };
            }
            value.resize(end);
        }
        else {
            static_assert(false, "Unknown StringTypeEnum.");
        }
    }

    template <class T>
    void load(Stream& stream, std::optional<T>& value) {
        u8 has_value;
        stream >> has_value;
        if (has_value) {
            stream >> value.emplace();
        }
    }

    void load(Stream& stream, Value& value) {
        i32 type;
        stream >> value.name >> type;

        switch (type) {
        case Value::integer:
            stream >> value.value.emplace<i32>();
            break;
        case Value::decimal:
            stream >> value.value.emplace<f64>();
            break;
        case Value::string:
            stream >> value.value.emplace<std::wstring>();
            break;
        default:
            throw std::runtime_error{ std::format("Unknown value type {}.", type) };
        }
    }

    void load(Stream& stream, Header& value) {
        stream >> signature<"MFU2">
            >> value.runtime_version
            >> value.runtime_subversion
            >> value.product_version
            >> value.product_build
            >> value.language
            >> value.app_name
            >> skip<std::wstring>
            >> value.editor_filename
            >> skip<std::vector<char>>;

        spdlog::info("App name: {:?}.", to_string(value.app_name));
        spdlog::info("Build: {}.", value.product_build);
    }

}
