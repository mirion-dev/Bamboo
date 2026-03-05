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

    template <std::integral T>
    class Size {
        static constexpr usize MAX_SIZE{ 1000000 };

        T _value{};

    public:
        operator T() const noexcept {
            return _value;
        }

        void load(Stream& stream) {
            stream >> _value;

            if (_value < 0) {
                throw std::runtime_error{ std::format("Container size cannot be negative. Found {}.", _value) };
        }

            if (_value > MAX_SIZE) {
            throw std::runtime_error{
                    std::format("Container size is too large. Found {} but max allowed {}.", _value, MAX_SIZE)
            };
        }
    }
    };

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
        void load(Stream& stream, S size) {
            this->resize(size);
            stream.load(this->data(), size);
        }

        void load(Stream& stream) {
            Size<S> size;
            stream >> size;
            stream.load(*this, size);
        }
    };

    struct String : std::wstring {
        void load(Stream& stream) {
            Size<i16> size;
            stream >> size >> ignore_bytes<2>;
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

    export struct File {
        Header header;

        void load(Stream& stream) {
            {
                Timer _{ "parsing header" };
                stream >> header;
            }
        }
    };

}
