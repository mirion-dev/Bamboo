export module bamboo.stream;

import std;
import bamboo.types;
import bamboo.meta;

export import bamboo.stream.core;

namespace bamboo {

    export template <class... Args>
    auto args(Args&&... args) {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    }

    export template <class T, class... Args>
    struct Skip {
        template <class S>
            requires(
                loadable_type<S, T&> == LoadableType::directly
                || std::is_default_constructible_v<T> && loadable<S, T&, Args...>
            )
        void load(S& stream) const {
            if constexpr (loadable_type<S, T&> == LoadableType::directly) {
                stream.ignore(sizeof(T));
            } else {
                T dummy;
                stream >> bamboo::args(dummy, std::forward<Args>(args)...);
            }
        }
    };

    export template <class T, class... Args>
    constexpr Skip<T, Args...> skip;

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

    export template <class S, class C, std::integral Size>
    void resize_load(S& stream, C& container, Size size) {
        using value_type = std::remove_pointer_t<decltype(container.data())>;

        static constexpr usize MAX_SIZE{ binary_copyable<value_type> ? (1 << 26) / sizeof(value_type) : 1 << 16 };

        if (size < 0) {
            throw std::runtime_error{ std::format("Container size cannot be negative. Found {}.", size) };
        }

        if (size >= MAX_SIZE) {
            throw std::runtime_error{
                std::format("Container size is too large. Found {} but max allowed {} for this type.", size, MAX_SIZE)
            };
        }

        container.resize(size);
        stream >> bamboo::args(container.data(), size);
    }

}
