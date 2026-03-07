export module bamboo.stream;

import std;
import bamboo.core;

namespace bamboo {

    export template <class T>
    constexpr bool dense_layout_v{
        std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_trivially_copyable_v<T> && requires { T::dense_layout; }
    };

    template <class T>
    concept binary_readable = !std::is_const_v<T> && dense_layout_v<T>;

    template <class S>
    concept has_binary_read = requires(S& stream, char* ptr, usize size) {
        stream.read(ptr, size);
    };

    template <class S, class T, class... Args>
    concept has_member_load = requires(S& stream, T&& value, Args&&... args) {
        std::forward<T>(value).load(stream, std::forward<Args>(args)...);
    };

    template <class S, class T, class... Args>
    concept has_non_member_load = requires(S& stream, T&& value, Args&&... args) {
        load(stream, std::forward<T>(value), std::forward<Args>(args)...);
    };

    enum class load_mode {
        read,
        read_array,
        member_custom,
        non_member_custom,
        none
    };

    template <class S, class T, class... Args>
    static constexpr load_mode select_load_mode{
        has_binary_read<S>
        && std::is_lvalue_reference_v<T> && binary_readable<std::remove_reference_t<T>>
        && sizeof...(Args) == 0
        ? load_mode::read

        : has_binary_read<S>
        && std::is_pointer_v<std::decay_t<T>> && binary_readable<std::remove_pointer_t<std::decay_t<T>>>
        && sizeof...(Args) == 1 && (std::convertible_to<Args, usize> && ...)
        ? load_mode::read_array

        : has_member_load<S, T, Args...>
        ? load_mode::member_custom

        : has_non_member_load<S, T, Args...>
        ? load_mode::non_member_custom

        : load_mode::none
    };

    template <class S, class T, class... Args>
    concept loadable = select_load_mode<S, T, Args...> != load_mode::none;

    struct load_fn {
        template <class S, class T, class... Args>
            requires loadable<S, T, Args...>
        static void operator()(S& stream, T&& value, Args&&... args) {
            static constexpr load_mode MODE{ select_load_mode<S, T, Args...> };
            if constexpr (MODE == load_mode::read) {
                stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            }
            else if constexpr (MODE == load_mode::read_array) {
                stream.read(reinterpret_cast<char*>(value), sizeof(*value) * args...);
            }
            else if constexpr (MODE == load_mode::member_custom) {
                std::forward<T>(value).load(stream, std::forward<Args>(args)...);
            }
            else if constexpr (MODE == load_mode::non_member_custom) {
                load(stream, std::forward<T>(value), std::forward<Args>(args)...);
            }
            else {
                static_assert(false);
                std::unreachable();
            }
        }
    };

    export constexpr load_fn load;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            exceptions(failbit | badbit);
        }

        Stream(std::string_view path) :
            Stream{} {

            open(std::u8string{ path.begin(), path.end() }, binary | in);
        }

        template <class T, class... Args>
        Stream& load(this auto& self, T&& value, Args&&... args) {
            bamboo::load(self, std::forward<T>(value), std::forward<Args>(args)...);
            return self;
        }

        template <class T>
        Stream& operator>>(this auto& self, T&& value) {
            return self.load(std::forward<T>(value));
        }
    };

    template <usize N>
    struct IgnoreBytes {
        void load(Stream& stream) const {
            stream.ignore(N);
        }
    };

    export template <usize N>
    constexpr IgnoreBytes<N> ignore_bytes;

    template <class T>
    struct Ignore {
        template <class... Args>
            requires (dense_layout_v<T> && sizeof...(Args) == 0
                || std::is_default_constructible_v<T> && loadable<Stream&, T, Args...>)
        void load(Stream& stream, Args&&... args) const {
            if constexpr (dense_layout_v<T> && sizeof...(Args) == 0) {
                stream >> ignore_bytes<sizeof(T)>;
            }
            else {
                T dummy;
                stream.load(dummy, std::forward<Args>(args)...);
            }
        }
    };

    export template <class T>
    constexpr Ignore<T> ignore;

}
