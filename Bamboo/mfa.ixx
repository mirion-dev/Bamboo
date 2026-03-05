export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

    static void verify_size(std::integral auto size) {
        static constexpr usize MAX_SIZE{ 1000000 };

        if (size < 0) {
            throw std::runtime_error{ std::format("Array size cannot be negative. Found {}.", size) };
        }

        if (size > MAX_SIZE) {
            throw std::runtime_error{
                std::format("Array size is too large. Found {} but max allowed {}.", size, MAX_SIZE)
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

    template <class T, usize N>
    struct Array : std::array<T, N> {
        void load(Stream& stream) {
            stream.load(this->data(), N);
        }
    };

    template <class T, std::integral S = i32>
    struct Vector : std::vector<T> {
        void load(Stream& stream, S size) {
            mfa::verify_size(size);
            this->resize(size);
            stream.load(this->data(), size);
        }

        void load(Stream& stream) {
            S size;
            stream >> size;
            stream.load(*this, size);
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

    export struct Header {
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
        }
    };

}
