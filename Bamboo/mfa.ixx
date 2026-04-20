module;

#include <spdlog/spdlog.h>
#undef pascal

export module bamboo.mfa;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.stream;
import bamboo.general;

namespace bamboo::mfa {

    struct Context {
        i32 build{};
    };

    export using Stream = Stream<Context>;

    template <class T, usize N>
    static void load(Stream& stream, std::array<T, N>& value) {
        stream >> bamboo::args(value.data(), N);
    }

    template <std::integral T>
    struct SizeType {};

    template <std::integral T>
    static constexpr SizeType<T> size_type;

    template <class T>
    static void load(Stream& stream, std::vector<T>& value, std::integral auto size) {
        bamboo::resize_load(stream, value, size);
    }

    template <class T, std::integral S = i32>
    static void load(Stream& stream, std::vector<T>& value, SizeType<S> = {}) {
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

    static constexpr StringType<StringTypeEnum::pascal> string_type_pascal;
    static constexpr StringType<StringTypeEnum::c> string_type_c;
    static constexpr StringType<StringTypeEnum::pascal_c> string_type_pascal_c;
    template <usize N>
    static constexpr StringType<StringTypeEnum::fixed_c, N> string_type_fixed_c;

    template <StringTypeEnum Type = {}, usize N = {}>
    static void load(Stream& stream, std::wstring& value, StringType<Type, N> = {}) {
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

    static void load(Stream& stream, Header& value) {
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

        spdlog::info("Application \"{}\" (Build {}).", to_string(value.app_name), value.product_build);
    }

    static void load(Stream& stream, Font& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
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
            >> value.pitch_family
            >> args(value.name, string_type_fixed_c<32>);

        spdlog::debug("Read font \"{}\".", to_string(value.name));
    }

    static void load(Stream& stream, FontBank& value) {
        stream >> signature<"ATNF"> >> static_cast<std::vector<Font>&>(value);
        spdlog::info("Read {} fonts.", value.size());
    }

    static void load(Stream& stream, Sound& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.flags[Sound::play_from_disk] ? 0 : (value.name.size() + 1) * 2));

        --value.handle;

        spdlog::debug("Read sound \"{}\".", to_string(value.name));
    }

    static void load(Stream& stream, SoundBank& value) {
        stream >> signature<"APMS"> >> static_cast<std::vector<Sound>&>(value);
        spdlog::info("Read {} sounds.", value.size());
    }

    static void load(Stream& stream, Music& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.flags
            >> value.frequency
            >> args(value.name, string_type_pascal_c);
        stream >> args(value.data, value.size - (value.name.size() + 1) * 2);

        spdlog::debug("Read music \"{}\".", to_string(value.name));
    }

    static void load(Stream& stream, MusicBank& value) {
        stream >> signature<"ASUM"> >> static_cast<std::vector<Music>&>(value);
        spdlog::info("Read {} music.", value.size());
    }

    static void load(Stream& stream, Image& value) {
        stream >> value.handle
            >> value.checksum
            >> value.references
            >> value.size
            >> value.width
            >> value.height
            >> value.graphic_mode
            >> value.flags
            >> skip<i16>
            >> value.hotspot_x
            >> value.hotspot_y
            >> value.action_x
            >> value.action_y
            >> value.transparent_color
            >> args(value.data, value.size);

        if (stream.context.build < 284) {
            ++value.handle;
        }
    }

    static void load(Stream& stream, ImageBank& value) {
        stream >> signature<"AGMI">
            >> value.graphic_mode
            >> value.palette_version
            >> args(value.palette, size_type<i16>)
            >> static_cast<std::vector<Image>&>(value);

        spdlog::info("Read {} images.", value.size());
    }

    static void load(Stream& stream, Control& value) {
        stream >> value.type >> value.data;
    }

    static void load(Stream& stream, Setting& value) {
        stream >> value.app_name
            >> value.author
            >> value.description
            >> value.copyright
            >> value.company
            >> value.version
            >> value.app_width
            >> value.app_height
            >> value.border_color
            >> value.display_flags
            >> value.graphic_flags
            >> value.help_file
            >> skip<std::wstring>
            >> value.score
            >> value.lives
            >> value.frame_rate
            >> value.build_type
            >> value.build_filename
            >> skip<std::wstring>
            >> skip<std::wstring>
            >> value.about
            >> skip<i32>
            >> value.binary_files
            >> value.controls;
    }

    export void load(Stream& stream, File& value) {
        Timer _{ "parsing MFA file" };
        {
            Timer _{ "parsing header" };
            stream >> value.header;
            stream.context.build = value.header.product_build;
        }
        {
            Timer _{ "parsing font bank" };
            stream >> value.font_bank;
        }
        {
            Timer _{ "parsing sound bank" };
            stream >> value.sound_bank;
        }
        {
            Timer _{ "parsing music bank" };
            stream >> value.music_bank;
        }
        {
            Timer _{ "parsing icon bank" };
            stream >> value.icon_bank;
        }
        {
            Timer _{ "parsing image bank" };
            stream >> value.image_bank;
        }
        {
            Timer _{ "parsing setting" };
            stream >> value.setting;
        }
    }

}
