export module bamboo.core;

import std;

namespace bamboo {

    static_assert(sizeof(float) == 4 && sizeof(double) == 8 && sizeof(wchar_t) == 2, "Unsupported data model.");

    export {

        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;

        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;

        using f32 = float;
        using f64 = double;

        using isize = std::ptrdiff_t;
        using usize = std::size_t;

    }

    export template <usize N>
    struct LiteralString {
        std::array<char, N> data;

        constexpr LiteralString(const char (&data)[N]) noexcept {
            std::ranges::copy(data, this->data.data());
        }

        constexpr operator std::string_view() const noexcept {
            return { data.data(), size() };
        }

        constexpr usize size() const noexcept {
            return N - 1;
        }
    };

}
