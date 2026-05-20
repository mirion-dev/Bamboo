export module bamboo.stream.utils;

import std;
import bamboo.types;
import bamboo.utils;
import bamboo.stream;

namespace bamboo {

    export template <class T, class... Args>
    struct Skip {
        template <class S>
            requires (is_dense_layout_v<T> && sizeof...(Args) == 0
                || std::is_default_constructible_v<T> && loadable<S, T&, Args...>)
        void load(S& stream) const {
            if constexpr (is_dense_layout_v<T> && sizeof...(Args) == 0) {
                stream.ignore(sizeof(T));
            }
            else {
                T dummy;
                stream >> bamboo::args(dummy, std::forward<Args>(args)...);
            }
        }
    };

    export template <class T, class... Args>
    constexpr Skip<T, Args...> skip;

    template <usize N>
    struct StringLiteral : std::array<char, N - 1> {
        consteval StringLiteral(const char (&data)[N]) noexcept {
            std::ranges::copy_n(data, N - 1, this->data());
        }

        consteval operator std::string_view() const noexcept {
            return { this->data(), this->size() };
        }
    };

    export template <StringLiteral Expected>
    struct Signature {
        void load(auto& stream) const {
            static constexpr usize N{ Expected.size() };
            std::array<char, N> buffer;
            stream >> bamboo::args(buffer.data(), N);

            std::string_view expected{ Expected }, actual{ buffer };
            if (expected != actual) {
                throw Exception{
                    std::format("Incorrect signature. Expected \"{}\" but found \"{}\".", expected, actual)
                };
            }
        }
    };

    export template <StringLiteral Expected>
    constexpr Signature<Expected> signature;

    export void resize_load(auto& stream, auto& container, std::integral auto size) {
        using value_type = std::remove_pointer_t<decltype(container.data())>;

        static constexpr usize MAX_SIZE{ is_dense_layout_v<value_type> ? 100'000'000 / sizeof(value_type) : 100'000 };

        if (size < 0) {
            throw Exception{ std::format("Container size cannot be negative. Found {}.", size) };
        }

        if (size > MAX_SIZE) {
            throw Exception{
                std::format(
                    "Container size is too large. Found {} but max allowed {} for this type.",
                    size,
                    MAX_SIZE
                )
            };
        }

        container.resize(size);
        stream >> bamboo::args(container.data(), size);
    }

}
