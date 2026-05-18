module;

#include <spdlog/spdlog.h>

export module bamboo.utils;

import std;

namespace bamboo {

    export std::string to_string(std::wstring_view str) noexcept {
        std::u8string u8{ std::filesystem::path{ str.begin(), str.end() }.u8string() };
        return { reinterpret_cast<char*>(u8.data()), u8.size() };
    }

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

}
