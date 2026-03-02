export module bamboo.mfa;

import std;
import bamboo.core;
import bamboo.stream;

namespace bamboo::mfa {

    template <usize Size, class T = char>
        requires std::is_default_constructible_v<T>
    struct IgnoreN {
        template <class... Args>
        friend void load(Stream& stream, IgnoreN, Args&&... args) {
            if constexpr (std::is_scalar_v<T>) {
                stream.ignore(sizeof(T) * Size);
            }
            else {
                T dummy;
                for (usize i{}; i < Size; ++i) {
                    stream.load(dummy, std::forward<Args>(args)...);
                }
            }
        }
    };

    template <usize Size, class T = char>
    static constexpr IgnoreN<Size, T> ignore_n;

    template <class T>
    using Ignore = IgnoreN<1, T>;

    template <class T>
    static constexpr Ignore<T> ignore;

    template <LiteralString Expected>
    struct Signature {
        friend void load(Stream& stream, Signature) {
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
        requires std::is_arithmetic_v<T>
    struct Array : std::array<T, N> {
        friend void load(Stream& stream, Array& self) {
            stream.load(self.data(), N);
        }
    };

    template <class T, class S = i32>
        requires std::is_arithmetic_v<T>
    struct Vector : std::vector<T> {
        static constexpr usize MAX_SIZE{ 1000000 };

        friend void load(Stream& stream, Vector& self, std::streamsize size) {
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

            self.resize(size);
            stream.load(self.data(), size);
        }

        friend void load(Stream& stream, Vector& self) {
            S size;
            stream >> size;
            stream.load(self, size);
        }
    };

    struct String : std::wstring {
        friend void load(Stream& stream, String& self) {
            i16 size;
            stream >> size >> ignore_n<2>;
            self.resize(size);
            stream.load(self.data(), size);
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

        friend void load(Stream& stream, Header& self) {
            stream >> signature<"MFU2">
                >> self.runtime_version
                >> self.runtime_subversion
                >> self.product_version
                >> self.product_build
                >> self.language
                >> self.app_name
                >> ignore<String>
                >> self.editor_filename
                >> ignore<Vector<char>>;
        }
    };

}
