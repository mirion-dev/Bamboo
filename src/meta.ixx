export module bamboo.meta;

import std;
import bamboo.types;

namespace bamboo {

    static constexpr usize MAX_AGGREGATE_FIELD_COUNT{ 64 };

    export template <usize N>
    struct StringLiteral : std::array<char, N - 1> {
        consteval StringLiteral(const char (&data)[N]) noexcept {
            std::ranges::copy_n(data, N - 1, this->data());
        }

        consteval operator std::string_view() const noexcept {
            return { this->data(), this->size() };
        }
    };

    struct Any {
        template <class T>
        consteval operator T() const noexcept {
            return {};
        }
    };

    template <class T, usize N = 0>
    consteval usize aggregate_field_count() noexcept {
        if constexpr ([]<usize... I>(std::index_sequence<I...>) noexcept {
                          return requires { T{ (I, Any{})... }; };
                      }(std::make_index_sequence<N + 1>{})) {
            return aggregate_field_count<T, N + 1>();
        }
        return N;
    }

    template <class... Args>
    consteval auto make_tuple(Args&&... args) noexcept {
        return std::tuple<std::remove_reference_t<Args>...>{ std::forward<Args>(args)... };
    }

    template <class T, usize N>
        requires(N <= MAX_AGGREGATE_FIELD_COUNT)
    consteval auto aggregate_field_tuple() noexcept {
#include "inc/aggregate_field_tuple.inc"
    }

    export template <class T>
        requires std::is_aggregate_v<T> && std::is_class_v<T>
    using aggregate_fields_t = decltype(aggregate_field_tuple<T, aggregate_field_count<T>()>());

}
