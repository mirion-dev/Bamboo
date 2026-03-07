module;

#include <spdlog/spdlog.h>

export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

#pragma region utils

    class Timer {
        std::string _message;
        std::chrono::steady_clock::time_point _start{ std::chrono::steady_clock::now() };

    public:
        Timer(std::string_view message) noexcept :
            _message{ message } {

            spdlog::info("Started {}.", _message);
        }

        ~Timer() noexcept {
            using namespace std::chrono;
            double duration{ duration_cast<milliseconds>(steady_clock::now() - _start).count() / 1e3 };
            spdlog::info("Finished {} ({}s).", _message, duration);
        }

        Timer(Timer&& other) noexcept = default;
        Timer& operator=(Timer&& other) noexcept = default;
    };

    static void verify_size(std::integral auto size) {
        static constexpr usize MAX_SIZE{ 100'000'000 };

        if (size < 0) {
            throw std::runtime_error{ std::format("Container size cannot be negative. Found {}.", size) };
        }

        if (size > MAX_SIZE) {
            throw std::runtime_error{
                std::format("Container size is too large. Found {} but max allowed {}.", size, MAX_SIZE)
            };
        }
    }

    template <class T>
        requires !dense_layout_v<T>
    static void load(Stream& stream, T* ptr, usize size) {
        for (usize i{}; i < size; ++i) {
            stream >> ptr[i];
        }
    }

#pragma endregion

#pragma region basic containers

    template <class T, usize N>
    struct Array : std::array<T, N> {
        void load(Stream& stream) {
            stream.load(this->data(), N);
        }
    };

    template <class T, std::integral S = i32>
    struct Vector : std::vector<T> {
        void load(Stream& stream) {
            S size;
            stream >> size;
            stream.load(*this, size);
        }

        void load(Stream& stream, S size) {
            mfa::verify_size(size);
            this->resize(size);
            stream.load(this->data(), size);
        }
    };

    struct String : std::wstring {
        void load(Stream& stream) {
            i16 size;
            stream >> size >> ignore_bytes<2>;

            mfa::verify_size(size);
            resize(size);
            stream.load(data(), size);
        }
    };

#pragma endregion

    template <LiteralString Expected>
    struct Signature {
        void load(Stream& stream) const {
            Array<char, Expected.size()> buffer;
            stream >> buffer;

            std::string_view expected{ Expected }, actual{ buffer };
            if (expected != actual) {
                throw std::runtime_error{
                    std::format("Incorrect signature. Expected \"{}\" but found \"{}\".", expected, actual)
                };
            }
        }
    };

    template <LiteralString Expected>
    static constexpr Signature<Expected> signature;

    struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;
        String app_name;
        String editor_filename;

        void load(Stream& stream) {
            stream >> signature<"MFU2">
                >> runtime_version
                >> runtime_subversion
                >> product_version
                >> product_build
                >> language
                >> app_name
                >> ignore<String>
                >> editor_filename
                >> ignore<Vector<char>>;

            spdlog::info("Application \"{}\" (Build {}).", bamboo::to_string(app_name), product_build);
        }
    };

    struct Font {
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

        void load(Stream& stream) {
            Array<wchar_t, 32> buffer;
            stream >> handle
                >> checksum
                >> references
                >> size
                >> height
                >> width
                >> escapement
                >> orientation
                >> weight
                >> italic
                >> underline
                >> strike_out
                >> charset
                >> out_precision
                >> clip_precision
                >> quality
                >> pitch_family
                >> buffer;

            name = buffer.data();
            spdlog::debug("Read font \"{}\".", bamboo::to_string(name));
        }
    };

    struct FontBank : Vector<Font> {
        void load(Stream& stream) {
            stream >> signature<"ATNF"> >> static_cast<Vector&>(*this);
            spdlog::info("Read {} font(s).", size());
        }
    };

    struct Sound {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        u32 flags;
        i32 frequency;
        std::wstring name;
        Vector<char> data;

        void load(Stream& stream) {
            Vector<wchar_t> buffer;
            stream >> handle
                >> checksum
                >> references
                >> size
                >> flags
                >> frequency
                >> buffer;
            stream.load(data, flags & 0x0040 ? size : size - static_cast<i32>(buffer.size() * sizeof(wchar_t)));

            name = { std::from_range, buffer };
            spdlog::debug("Read sound \"{}\".", bamboo::to_string(name));
        }
    };

    struct SoundBank : Vector<Sound> {
        void load(Stream& stream) {
            stream >> signature<"APMS"> >> static_cast<Vector&>(*this);
            spdlog::info("Read {} sound(s).", size());
        }
    };

    struct Music {
        u32 handle;
        u32 checksum;
        i32 references;
        i32 size;
        u32 flags;
        i32 frequency;
        std::wstring name;
        Vector<char> data;

        void load(Stream& stream) {
            Vector<wchar_t> buffer;
            stream >> handle
                >> checksum
                >> references
                >> size
                >> flags
                >> frequency
                >> buffer;
            stream.load(data, size);

            name = { std::from_range, buffer };
            spdlog::debug("Read music \"{}\".", bamboo::to_string(name));
        }
    };

    struct MusicBank : Vector<Music> {
        void load(Stream& stream) {
            stream >> signature<"ASUM"> >> static_cast<Vector&>(*this);
            spdlog::info("Read {} music(s).", size());
        }
    };

    export struct File {
        Header header;
        FontBank font_bank;
        SoundBank sound_bank;
        MusicBank music_bank;

        void load(Stream& stream) {
            {
                Timer _{ "parsing header" };
                stream >> header;
            }
            {
                Timer _{ "parsing font bank" };
                stream >> font_bank;
            }
            {
                Timer _{ "parsing sound bank" };
                stream >> sound_bank;
            }
            {
                Timer _{ "parsing music bank" };
                stream >> music_bank;
            }
        }
    };

}
