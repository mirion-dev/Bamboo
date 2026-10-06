export module bamboo.stream_utils;

import std;
import bamboo.types;
import bamboo.meta;
import bamboo.stream;

namespace bamboo {

    export template <class... Args>
    auto args(Args&&... args) {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    }

    export struct Move {
        usize pos;

        template <class S>
        void load(S& stream) const {
            stream.seekg(pos);
        }
    };

    export auto move{ [](usize pos) { return Move{ pos }; } };

    export template <class T>
    struct Skip {
        template <class S>
            requires(binary_copyable<T> || std::is_default_constructible_v<T> && loadable<S, T&>)
        void load(S& stream) const {
            if constexpr (binary_copyable<T>) {
                stream.ignore(sizeof(T));
            } else {
                T dummy;
                stream >> dummy;
            }
        }
    };

    export template <class T>
    constexpr Skip<T> skip;

    export template <StringLiteral Expected>
    struct Signature {
        template <class S>
        void load(S& stream) const {
            static constexpr usize N{ Expected.size() };
            std::array<char, N> buffer;
            stream >> bamboo::args(buffer.data(), N);

            std::string_view expected{ Expected }, actual{ buffer };
            if (expected != actual) {
                throw std::runtime_error{
                    std::format("Incorrect signature. Expected {:?} but found {:?}.", expected, actual)
                };
            }
        }
    };

    export template <StringLiteral Expected>
    constexpr Signature<Expected> signature;

    export template <class C, std::integral Size>
        requires requires(C& container, Size size) {
            container.resize(size);
            container.data();
        }
    struct Resize {
    private:
        using T = std::remove_pointer_t<decltype(std::declval<C>().data())>;

        static constexpr usize MAX_SIZE{ binary_copyable<T> ? (1 << 26) / sizeof(T) : 1 << 16 };

    public:
        C& container;
        Size size;

        template <class S>
            requires loadable<S, T*, Size>
        void load(S& stream) const {
            if (size < 0) {
                throw std::runtime_error{ std::format("Container size cannot be negative. Found {}.", size) };
            }

            if (size >= MAX_SIZE) {
                throw std::runtime_error{ std::format(
                    "Container size is too large. Found {} but max allowed {} for this type.", size, MAX_SIZE
                ) };
            }

            container.resize(size);
            stream >> bamboo::args(container.data(), size);
        }
    };

    export auto resize{ []<class C, class Size>(C& container, Size size) { return Resize{ container, size }; } };

}
