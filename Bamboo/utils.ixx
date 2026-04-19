module;

#include <spdlog/spdlog.h>

export module bamboo.utils;

import std;
import bamboo.types;
import bamboo.stream;

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

    export class Timer {
        std::string _name;
        std::chrono::steady_clock::time_point _start{ std::chrono::steady_clock::now() };

    public:
        Timer(std::string_view name) noexcept :
            _name{ name } {

            spdlog::info("Started {}.", _name);
        }

        ~Timer() noexcept {
            using namespace std::chrono;
            double duration{ duration_cast<milliseconds>(steady_clock::now() - _start).count() / 1e3 };
            spdlog::info("Finished {} ({}s).", _name, duration);
        }

        Timer(Timer&& other) noexcept = default;
        Timer& operator=(Timer&& other) noexcept = default;
    };

    export std::string to_string(std::wstring_view str) noexcept {
        std::u8string u8{ std::filesystem::path{ str.begin(), str.end() }.u8string() };
        return { reinterpret_cast<char*>(u8.data()), u8.size() };
    }

    export template <class S, class T, std::integral Size>
    void resize_load(S& stream, T& container, const Size& size) {
        using value_type = std::remove_pointer_t<decltype(container.data())>;

        static constexpr usize MAX_SIZE{ is_dense_layout_v<value_type> ? 100'000'000 / sizeof(value_type) : 100'000 };

        if (size < 0) {
            throw std::runtime_error{ std::format("Container size cannot be negative. Found {}.", size) };
        }

        if (size > MAX_SIZE) {
            throw std::runtime_error{
                std::format(
                    "Container size is too large. Found {} but max allowed {} for this type.",
                    size,
                    MAX_SIZE
                )
            };
        }

        container.resize(size);
        stream >> bamboo::args(container.data(), size);
    }

}
