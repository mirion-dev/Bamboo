module;

#include <cassert>

export module bamboo.model.base;

import std;
import bamboo.types;

namespace bamboo {

    export struct Color {
        using is_dense_layout = void;

        u8 r, g, b, a;
    };

    export template <std::unsigned_integral T>
    struct Flags {
        using is_dense_layout = void;

        T value;

        class Ref {
            friend Flags;

            T* _ptr;
            T _mask;

            Ref(T* ptr, T mask) noexcept :
                _ptr{ ptr },
                _mask{ mask } {}

        public:
            operator bool() const noexcept {
                return *_ptr & _mask;
            }

            const Ref& operator=(bool value) const noexcept {
                if (value) {
                    *_ptr |= _mask;
                }
                else {
                    *_ptr &= ~_mask;
                }
                return *this;
            }
        };

        Ref operator[](usize index) noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return { &value, static_cast<T>(T{ 1 } << index) };
        }

        bool operator[](usize index) const noexcept {
            assert(index < std::numeric_limits<T>::digits);
            return value & static_cast<T>(T{ 1 } << index);
        }
    };

    export struct Value {
        enum Type {
            integer,
            decimal,
            string
        };

        std::wstring name;
        std::variant<i32, f64, std::wstring> value;
    };

    export struct Values : std::vector<Value> {};

    export struct Header {
        i16 runtime_version;
        i16 runtime_subversion;
        i32 product_version;
        i32 product_build;
        i32 language;
        std::wstring app_name;
        std::wstring editor_filename;
    };

}
