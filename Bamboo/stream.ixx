export module bamboo.stream;

import std;

namespace bamboo {

    template <class S, class T, class... Args>
    concept has_custom_load = requires(S& stream, T&& value, Args&&... args) {
        load(stream, std::forward<T>(value), std::forward<Args>(args)...);
    };

    template <class S>
    concept has_binary_read = requires(S& stream, char* ptr, std::streamsize size) {
        stream.read(ptr, size);
    };

    template <class T>
    concept binary_readable = !std::is_const_v<T> && (std::is_arithmetic_v<T> || std::is_enum_v<T>);

    enum class load_mode {
        unsupported,
        custom,
        scalar,
        scalar_array
    };

    template <class S, class T, class... Args>
    static constexpr load_mode select_load_mode{
        has_custom_load<S, T, Args...>
        ? load_mode::custom

        : has_binary_read<S>
        && std::is_lvalue_reference_v<T> && binary_readable<std::remove_reference_t<T>>
        && sizeof...(Args) == 0
        ? load_mode::scalar

        : has_binary_read<S>
        && std::is_pointer_v<std::decay_t<T>> && binary_readable<std::remove_pointer_t<std::decay_t<T>>>
        && sizeof...(Args) == 1 && (std::convertible_to<Args, std::streamsize> && ...)
        ? load_mode::scalar_array

        : load_mode::unsupported
    };

    struct load_fn {
        template <class S, class T, class... Args>
            requires (select_load_mode<S, T, Args...> != load_mode::unsupported)
        static void operator()(S& stream, T&& value, Args&&... args) {
            static constexpr load_mode MODE{ select_load_mode<S, T, Args...> };
            if constexpr (MODE == load_mode::custom) {
                load(stream, std::forward<T>(value), std::forward<Args>(args)...);
            }
            else if constexpr (MODE == load_mode::scalar) {
                stream.read(reinterpret_cast<char*>(&value), sizeof(value));
            }
            else if constexpr (MODE == load_mode::scalar_array) {
                stream.read(reinterpret_cast<char*>(value), sizeof(*value) * args...);
            }
            else {
                static_assert(false);
                std::unreachable();
            }
        }
    };

    static constexpr load_fn load;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            exceptions(failbit | badbit);
        }

        Stream(const std::string& path) :
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

}
