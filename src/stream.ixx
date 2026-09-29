export module bamboo.stream;

import std;
import bamboo.types;
import bamboo.meta;

namespace bamboo {

    struct Load {
    private:
        template <class S, class TPtr, class Size, class... Args>
        static void _indirectly(S& stream, TPtr&& ptr, Size&& raw_size, Args&&... args) {
            using T = std::remove_pointer_t<std::decay_t<TPtr>>;

            auto size{ static_cast<usize>(raw_size) };
            if constexpr (binary_copyable<T>) {
                stream.read(reinterpret_cast<char*>(ptr), sizeof(T) * size);
            } else {
                for (usize i{}; i < size; ++i) {
                    Load::operator()(stream, ptr[i], std::forward<Args>(args)...);
                }
            }
        }

    public:
        template <class S, class T, class... Args>
            requires loadable<S, T, Args...>
        static void operator()(S& stream, T&& value, Args&&... args) {
            if constexpr (loadable_type<S, T, Args...> == LoadableType::member_load) {
                std::forward<T>(value).load(stream, std::forward<Args>(args)...);
            } else if constexpr (loadable_type<S, T, Args...> == LoadableType::non_member_load) {
                load(stream, std::forward<T>(value), std::forward<Args>(args)...);
            } else if constexpr (loadable_type<S, T, Args...> == LoadableType::directly) {
                stream.read(reinterpret_cast<char*>(&value), sizeof(T));
            } else if constexpr (loadable_type<S, T, Args...> == LoadableType::indirectly) {
                Load::_indirectly(stream, std::forward<T>(value), std::forward<Args>(args)...);
            } else {
                std::unreachable();
            }
        }
    };

    export constexpr Load load;

    export class Stream : public std::fstream {
    public:
        Stream() noexcept {
            exceptions(failbit | badbit);
        }

        Stream(std::string_view path) {
            exceptions(failbit | badbit); // WORKAROUND: Delegation leads to crash when open() fails.
            open(std::string{ path }, binary | in);
        }

        template <class S, class T, class... Args>
            requires loadable<S, T, Args...>
        S& load(this S& self, T&& value, Args&&... args) {
            bamboo::load(self, std::forward<T>(value), std::forward<Args>(args)...);
            return self;
        }

        template <class S, class T>
            requires(!is_tuple<T> ? loadable<S, T> : tuple_loadable<S, T>)
        S& operator>>(this S& self, T&& args) {
            if constexpr (!is_tuple<T>) {
                self.load(std::forward<T>(args));
            } else {
                std::apply(
                    [&]<class... Args>(Args&&... args) { self.load(std::forward<Args>(args)...); },
                    std::forward<T>(args)
                );
            }
            return self;
        }
    };

}
