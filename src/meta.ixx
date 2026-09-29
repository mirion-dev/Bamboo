export module bamboo.meta;

import std;
import bamboo.types;

namespace bamboo {

    export template <usize N>
    struct StringLiteral : std::array<char, N - 1> {
        consteval StringLiteral(const char (&data)[N]) noexcept {
            std::ranges::copy_n(data, N - 1, this->data());
        }

        consteval operator std::string_view() const noexcept {
            return { this->data(), this->size() };
        }
    };

    template <class T>
    struct Unpadded;

    template <class T>
    concept unpadded = Unpadded<T>::value;

    template <class T>
    concept has_unpadded_mark = requires { typename T::unpadded; };

    template <class T>
    struct Unpadded : std::bool_constant<
                          std::is_scalar_v<T>
                          || std::is_array_v<T> && unpadded<std::remove_all_extents_t<T>>
                          || std::is_trivially_copyable_v<T> && has_unpadded_mark<T>
                      > {};

    export template <class T>
    concept binary_copyable
        = unpadded<T> && !std::is_null_pointer_v<T> && !std::is_pointer_v<T> && !std::is_member_pointer_v<T>;

    template <class S, class T, class... Args>
    struct Loadable;

    export template <class S, class T, class... Args>
    concept loadable = Loadable<S, T, Args...>::value;

    template <class S, class T, class... Args>
    concept has_member_load = requires(S& stream, T&& value, Args&&... args) {
        std::forward<T>(value).load(stream, std::forward<Args>(args)...);
    };

    template <class S, class T, class... Args>
    concept has_non_member_load = requires(S& stream, T&& value, Args&&... args) {
        load(stream, std::forward<T>(value), std::forward<Args>(args)...);
    };

    template <class S, class T, class... Args>
    concept directly_loadable = std::is_lvalue_reference_v<T>
                                && !std::is_const_v<std::remove_reference_t<T>>
                                && binary_copyable<std::remove_reference_t<T>>
                                && sizeof...(Args) == 0;

    template <class S, class T, class... Args>
    struct IndirectlyLoadable : std::false_type {};

    template <class S, class T, class Size, class... Args>
    struct IndirectlyLoadable<S, T, Size, Args...>
        : std::bool_constant<
              std::is_pointer_v<std::decay_t<T>>
              && std::convertible_to<Size, usize>
              && loadable<S, std::remove_pointer_t<std::decay_t<T>>&, Args...>
          > {};

    template <class S, class T, class... Args>
    concept indirectly_loadable = IndirectlyLoadable<S, T, Args...>::value;

    export enum class LoadableType {
        none,
        member_load,
        non_member_load,
        directly,
        indirectly
    };

    export template <class S, class T, class... Args>
    constexpr LoadableType loadable_type{ [] {
        if constexpr (has_member_load<S, T, Args...>) {
            return LoadableType::member_load;
        } else if constexpr (has_non_member_load<S, T, Args...>) {
            return LoadableType::non_member_load;
        } else if constexpr (directly_loadable<S, T, Args...>) {
            return LoadableType::directly;
        } else if constexpr (indirectly_loadable<S, T, Args...>) {
            return LoadableType::indirectly;
        } else {
            return LoadableType::none;
        }
    }() };

    template <class S, class T, class... Args>
    struct Loadable : std::bool_constant<loadable_type<S, T, Args...> != LoadableType::none> {};

}
