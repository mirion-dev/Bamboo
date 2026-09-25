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

}
