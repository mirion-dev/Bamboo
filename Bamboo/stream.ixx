export module bamboo.stream;

import std;
import bamboo.core;

namespace bamboo {

    template <class S, class T, class... Args>
    concept has_custom_load = requires(S& s, T& v, Args&&... a) { load(s, v, std::forward<Args>(a)...); };

    template <class S>
    concept has_binary_read = requires(S& s, char* c, usize n) { s.read(c, n); };

    struct load_impl_t {
        template <class S, class T, class... Args>
        static void operator()(S& stream, T& value, Args&&... args) {
            if constexpr (has_custom_load<S, T, Args...>) {
                load(stream, value, std::forward<Args>(args)...);
            }
            else if constexpr (has_binary_read<S> && std::is_arithmetic_v<T> && sizeof...(args) == 0) {
                stream.read(reinterpret_cast<char*>(&value), sizeof(T));
            }
            else {
                static_assert(false, "load() required for non-arithmetic types.");
                std::unreachable();
            }
        }
    };

    static constexpr load_impl_t load_impl;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            this->exceptions(std::ios::failbit | std::ios::badbit);
        }

        Stream(std::string path) :
            Stream{} {

            this->open(std::u8string{ path.begin(), path.end() }, std::ios::binary | std::ios::in);
        }

        template <class T, class... Args>
        Stream& load(this auto&& self, T& value, Args&&... args) {
            load_impl(self, value, std::forward<Args>(args)...);
            return self;
        }
    };

}
