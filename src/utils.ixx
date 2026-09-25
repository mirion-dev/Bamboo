export module bamboo.utils;

import std;

namespace bamboo {

    export std::string to_string(std::wstring_view str) {
        std::u8string str_u8{ std::filesystem::path{ str }.u8string() };
        return { reinterpret_cast<char*>(str_u8.data()), str_u8.size() };
    }

    export std::wstring to_wstring(std::string_view str) {
        std::u8string str_u8{ str.begin(), str.end() };
        return std::filesystem::path{ str }.wstring();
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
