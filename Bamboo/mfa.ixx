export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

    template <usize Size>
    struct IgnoreBytes {
        void load(Stream& stream) const {
            stream.ignore(Size);
        }
    };

    template <usize Size>
    static constexpr IgnoreBytes<Size> ignore_bytes;

    template <class T>
        requires std::is_default_constructible_v<T>
    struct Ignore {
        template <class... Args>
        void load(Stream& stream, Args&&... args) const {
            T dummy;
            stream.load(dummy, std::forward<Args>(args)...);
        }
    };

    template <class T>
    static constexpr Ignore<T> ignore;

    template <LiteralString Expected>
    struct Signature {
        void load(Stream& stream) const {
            std::array<char, Expected.size()> buffer;
            stream.load(buffer.data(), buffer.size());

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

    template <class T, usize N>
    struct Array : std::array<T, N> {
        void load(Stream& stream) {
            if constexpr (dense_layout_v<T>) {
                stream.load(this->data(), N);
            }
            else {
                for (auto& i : *this) {
                    stream >> i;
                }
            }
        }
    };

    template <class T, std::integral S = i32>
        requires dense_layout_v<T>
    struct Vector : std::vector<T> {
        static constexpr usize MAX_SIZE{ 1000000 };

        void load(Stream& stream, std::streamsize size) {
            if (size < 0) {
                throw std::runtime_error{ std::format("Vector size cannot be negative. Found {}.", size) };
            }

            if (size > MAX_SIZE) {
                throw std::runtime_error{
                    std::format(
                        "Vector size is too large. Found {} but max allowed {}. Try to increase Vector::MAX_SIZE.",
                        size,
                        MAX_SIZE
                    )
                };
            }

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
            resize(size);
            stream.load(data(), size);
        }
    };

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
