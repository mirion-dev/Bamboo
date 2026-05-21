export module bamboo.diag;

import std;
import bamboo.types;

namespace bamboo {

    export std::string to_string(std::wstring_view str) noexcept {
        std::u8string u8{ std::filesystem::path{ str }.u8string() };
        return { reinterpret_cast<char*>(u8.data()), u8.size() };
    }

    export std::wstring to_wstring(std::string_view str) noexcept {
        std::u8string u8{ str.begin(), str.end() };
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

    export template <class S>
    class LoadError : public std::runtime_error {
        static std::string _format(auto& stream, std::string_view message, const std::source_location& loc) noexcept {
            return std::format(
                "[{}:{}] [{:#018x}] {}",
                to_string(std::filesystem::path{ loc.file_name() }.filename().wstring()),
                loc.line(),
                static_cast<usize>(stream.tellg()),
                message
            );
        }

    public:
        LoadError(
            S& stream,
            std::string_view message,
            const std::source_location& loc = std::source_location::current()
        ) noexcept :
            std::runtime_error{ this->_format(stream, message, loc) } {}
    };

}
