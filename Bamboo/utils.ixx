export module bamboo.utils;

import std;

namespace bamboo {

    export std::string to_string(std::wstring_view str) noexcept {
        std::u8string u8{ std::filesystem::path{ str.begin(), str.end() }.u8string() };
        return { reinterpret_cast<char*>(u8.data()), u8.size() };
    }

    export class Timer {
        std::chrono::steady_clock::time_point _start{ std::chrono::steady_clock::now() };

    public:
        double duration() noexcept {
            auto now{ std::chrono::steady_clock::now() };
            double res{ std::chrono::duration<double>{ now - _start }.count() };
            _start = now;
            return res;
        }
    };

}
