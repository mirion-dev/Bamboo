export module bamboo.stream;

import std;

namespace bamboo {

    namespace load_impl {

        enum class strategy {
            none,
            custom_load,
            binary_read,
            binary_read_array
        };

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

        template <class S, class T, class... Args>
        static constexpr strategy classifier{
            has_custom_load<S, T, Args...>
            ? strategy::custom_load

            : has_binary_read<S>
            && std::is_lvalue_reference_v<T> && binary_readable<std::remove_reference_t<T>>
            && sizeof...(Args) == 0
            ? strategy::binary_read

            : has_binary_read<S>
            && std::is_pointer_v<std::decay_t<T>> && binary_readable<std::remove_pointer_t<std::decay_t<T>>>
            && sizeof...(Args) == 1 && (std::convertible_to<Args, std::streamsize> && ...)
            ? strategy::binary_read_array

            : strategy::none
        };

        struct fn {
            template <class S, class T, class... Args>
                requires (classifier<S, T, Args...> != strategy::none)
            static void operator()(S& stream, T&& value, Args&&... args) {
                static constexpr strategy STRATEGY{ classifier<S, T, Args...> };
                if constexpr (STRATEGY == strategy::custom_load) {
                    load(stream, std::forward<T>(value), std::forward<Args>(args)...);
                }
                else if constexpr (STRATEGY == strategy::binary_read) {
                    stream.read(reinterpret_cast<char*>(&value), sizeof(value));
                }
                else if constexpr (STRATEGY == strategy::binary_read_array) {
                    stream.read(reinterpret_cast<char*>(value), sizeof(*value) * args...);
                }
                else {
                    static_assert(false);
                    std::unreachable();
                }
            }
        };

    }

    static constexpr load_impl::fn load_fn;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            this->exceptions(std::ios::failbit | std::ios::badbit);
        }

        Stream(const std::string& path) :
            Stream{} {

            this->open(std::u8string{ path.begin(), path.end() }, std::ios::binary | std::ios::in);
        }

        template <class T, class... Args>
        Stream& load(this auto& self, T&& value, Args&&... args) {
            load_fn(self, std::forward<T>(value), std::forward<Args>(args)...);
            return self;
        }

        template <class T>
        Stream& operator>>(this auto& self, T&& value) {
            return self.load(std::forward<T>(value));
        }
    };

}
