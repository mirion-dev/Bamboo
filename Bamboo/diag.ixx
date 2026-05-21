module;

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

export module bamboo.diag;

import std;
import bamboo.types;

using namespace std::literals;

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
        static std::string _format(auto& stream, std::string_view message, const std::source_location& loc) {
            return std::format(
                "[{}:{}] [{:#010x}] {}",
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
        ) :
            std::runtime_error{ this->_format(stream, message, loc) } {}
    };

    template <class S>
    class StreamPosFlagFormatter : public spdlog::custom_flag_formatter {
        std::weak_ptr<S> _ptr;

    public:
        StreamPosFlagFormatter(std::weak_ptr<S> ptr) noexcept :
            _ptr{ ptr } {}

        void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) {
            std::shared_ptr stream{ _ptr.lock() };
            if (stream == nullptr) {
                dest.append("invalid"sv);
                return;
            }

            dest.append(std::format("{:#010x}", static_cast<usize>(stream->tellg())));
        }

        std::unique_ptr<custom_flag_formatter> clone() const noexcept {
            return std::make_unique<StreamPosFlagFormatter>(_ptr);
        }
    };

    export template <class S>
    class Logger : public spdlog::logger {
        std::string _pattern;
        std::weak_ptr<S> _ptr;

        void _set_pattern() noexcept {
            auto formatter{ std::make_unique<spdlog::pattern_formatter>() };
            formatter->add_flag<StreamPosFlagFormatter<S>>('&', _ptr);
            formatter->set_pattern(_pattern);
            set_formatter(std::move(formatter));
        }

    public:
        Logger(std::string_view name, std::string_view path) noexcept :
            logger{
                std::string{ name },
                spdlog::sinks_init_list{
                    std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
                    std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::string{ path }, true)
                }
            } {}

        auto console_sink() const noexcept {
            return std::static_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(sinks()[0]);
        }

        auto file_sink() const noexcept {
            return std::static_pointer_cast<spdlog::sinks::basic_file_sink_mt>(sinks()[1]);
        }

        void set_pattern(std::string_view pattern) noexcept {
            _pattern = pattern;
            _set_pattern();
        }

        void set_stream(std::weak_ptr<S> ptr) noexcept {
            _ptr = ptr;
            _set_pattern();
        }
    };

}
