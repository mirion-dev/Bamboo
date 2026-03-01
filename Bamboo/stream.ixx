export module bamboo.stream;

import std;
import bamboo.core;

namespace bamboo {

    template <class S, class T, class... Args>
    concept has_custom_load = requires(S& stream, T&& value, Args&&... args) {
        load(stream, std::forward<T>(value), std::forward<Args>(args)...);
    };

    template <class S>
    concept has_binary_read = requires(S& stream, char* ptr, usize size) {
        stream.read(ptr, size);
    };

    struct load_impl_t {
        template <class S, class T, class... Args>
        static void operator()(S& stream, T&& value, Args&&... args) {
            using RawT = std::remove_reference_t<T>;

            if constexpr (has_custom_load<S, T, Args...>) {
                load(stream, std::forward<T>(value), std::forward<Args>(args)...);
            }
            else if constexpr (has_binary_read<S> && std::is_arithmetic_v<RawT> && sizeof...(Args) == 0) {
                static_assert(
                    std::is_lvalue_reference_v<T> && !std::is_const_v<RawT>,
                    "Cannot load into a const or rvalue arithmetic type."
                );
                stream.read(reinterpret_cast<char*>(&value), sizeof(RawT));
            }
            else {
                static_assert(false, "No viable load() found.");
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
        Stream& load(this auto& self, T&& value, Args&&... args) {
            load_impl(self, std::forward<T>(value), std::forward<Args>(args)...);
            return self;
        }

        template <class T>
        Stream& operator>>(this auto& self, T&& value) {
            return self.load(std::forward<T>(value));
        }
    };

}
