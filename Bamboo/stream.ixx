export module bamboo.stream;

import std;
import bamboo.types;

namespace bamboo {

    export template <class T>
    constexpr bool is_dense_layout_v{
        std::is_arithmetic_v<T> || std::is_enum_v<T>
        || std::is_trivially_copyable_v<T> && requires { typename T::is_dense_layout; }
        || std::is_array_v<T> && is_dense_layout_v<std::remove_all_extents_t<T>>
    };

    template <class T>
    concept binary_readable = std::is_lvalue_reference_v<T>
        && !std::is_const_v<std::remove_reference_t<T>> && is_dense_layout_v<std::remove_reference_t<T>>;

    template <class S, class T, class... Args>
    concept has_member_load = requires(S& stream, T&& value, Args&&... args) {
        std::forward<T>(value).load(stream, std::forward<Args>(args)...);
    };

    template <class S, class T, class... Args>
    concept has_non_member_load = requires(S& stream, T&& value, Args&&... args) {
        load(stream, std::forward<T>(value), std::forward<Args>(args)...);
    };

    template <class S, class T, class... Args>
    struct Loadable;

    template <class S, class T, class... Args>
    static constexpr bool indirectly_loadable_v{};

    template <class S, class T, class Size, class... Args>
    static constexpr bool indirectly_loadable_v<S, T, Size, Args...>{
        std::is_pointer_v<std::decay_t<T>> && std::convertible_to<Size, usize>
        && Loadable<S, std::remove_pointer_t<std::decay_t<T>>&, Args...>::value
    };

    template <class S, class T, class... Args>
    struct Loadable<S, T, Args...> : std::bool_constant<
            has_member_load<S, T, Args...> || has_non_member_load<S, T, Args...>
            || binary_readable<T> && sizeof...(Args) == 0 || indirectly_loadable_v<S, T, Args...>
        > {};

    export template <class S, class T, class... Args>
    concept loadable = Loadable<S, T, Args...>::value;

    struct Load {
    private:
        template <class S, class T, class Size, class... Args>
        static void _indirectly(S& stream, T&& value, Size&& size, Args&&... args) {
            using value_type = std::remove_pointer_t<std::decay_t<T>>;

            usize cast_size{ static_cast<usize>(size) };
            if constexpr (is_dense_layout_v<value_type>) {
                stream.read(reinterpret_cast<char*>(value), sizeof(value_type) * cast_size);
            }
            else {
                for (usize i{}; i < cast_size; ++i) {
                    Load::operator()(stream, value[i], std::forward<Args>(args)...);
                }
            }
        }

    public:
        template <class S, class T, class... Args>
            requires loadable<S, T, Args...>
        static void operator()(S& stream, T&& value, Args&&... args) {
            if constexpr (has_member_load<S, T, Args...>) {
                std::forward<T>(value).load(stream, std::forward<Args>(args)...);
            }
            else if constexpr (has_non_member_load<S, T, Args...>) {
                load(stream, std::forward<T>(value), std::forward<Args>(args)...);
            }
            else if constexpr (binary_readable<T> && sizeof...(Args) == 0) {
                stream.read(reinterpret_cast<char*>(&value), sizeof(T));
            }
            else {
                Load::_indirectly(stream, std::forward<T>(value), std::forward<Args>(args)...);
            }
        }
    };

    export constexpr Load load;

    template <class T>
    static constexpr bool is_tuple_v{};

    template <class... Args>
    static constexpr bool is_tuple_v<std::tuple<Args...>>{ true };

    template <class T>
    concept is_tuple = is_tuple_v<std::remove_cvref_t<T>>;

    template <class S, class T>
    static constexpr bool tuple_loadable_v{};

    template <class S, class... Args>
    static constexpr bool tuple_loadable_v<S, std::tuple<Args...>>{ loadable<S, Args...> };

    template <class S, class T>
    concept tuple_loadable = tuple_loadable_v<S, std::remove_cvref_t<T>>;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            exceptions(failbit | badbit);
        }

        Stream(std::string_view path) :
            Stream{} {

            open(std::u8string{ path.begin(), path.end() }, binary | in);
        }

        template <class S, class T, class... Args>
            requires loadable<S, T, Args...>
        S& load(this S& self, T&& value, Args&&... args) {
            bamboo::load(self, std::forward<T>(value), std::forward<Args>(args)...);
            return self;
        }

        template <class S, class T>
            requires (!is_tuple<T> ? loadable<S, T> : tuple_loadable<S, T>)
        S& operator>>(this S& self, T&& args) {
            if constexpr (!is_tuple<T>) {
                bamboo::load(self, std::forward<T>(args));
            }
            else {
                std::apply(
                    [&]<class... Args>(Args&&... args) { bamboo::load(self, std::forward<Args>(args)...); },
                    std::forward<T>(args)
                );
            }
            return self;
        }
    };

    export template <class... Args>
    auto args(Args&&... args) {
        return std::forward_as_tuple(std::forward<Args>(args)...);
    }

}
