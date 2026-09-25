module;

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

export module bamboo.log;

import std;
import bamboo.types;
import bamboo.utils;

namespace bamboo {

    static constexpr auto LOG_PATH{ "bamboo.log" };
    static constexpr auto LOGGER_NAME{ "bamboo" };

    template <class S>
    class StreamPosFlagFormatter : public spdlog::custom_flag_formatter {
        std::weak_ptr<S> _ptr;

    public:
        StreamPosFlagFormatter(const std::weak_ptr<S>& ptr) noexcept
            : _ptr{ ptr } {}

        void format(const spdlog::details::log_msg&, const std::tm&, spdlog::memory_buf_t& dest) override {
            std::shared_ptr stream{ _ptr.lock() };
            if (stream == nullptr) {
                dest.append(std::string_view{ "invalid" });
                return;
            }

            dest.append(std::format("{:#010x}", static_cast<usize>(stream->tellg())));
        }

        std::unique_ptr<custom_flag_formatter> clone() const noexcept override {
            return std::make_unique<StreamPosFlagFormatter>(_ptr);
        }
    };

    class Logger : public spdlog::logger {
        using logger::set_formatter;
        using logger::set_pattern;

    public:
        using logger::logger;

        // WORKAROUND: ICE.
        using S = std::istream;
        void set_stream(const std::weak_ptr<S>& ptr) noexcept {
            auto formatter{ std::make_unique<spdlog::pattern_formatter>() };
            formatter->add_flag<StreamPosFlagFormatter<S>>('&', ptr);
            formatter->set_pattern("[%^%l%$] [%&] %v");
            set_formatter(std::move(formatter));
        }
    };

    export Logger* logger() noexcept {
        static auto value{ [] noexcept -> Logger {
            try {
                auto console_sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
                auto file_sink{ std::make_shared<spdlog::sinks::basic_file_sink_mt>(LOG_PATH, true) };
                // file_sink->set_level(spdlog::level::trace);
                return { LOGGER_NAME, { console_sink, file_sink } };
            } catch (const std::exception&) {
                return { LOGGER_NAME, std::make_shared<spdlog::sinks::null_sink_st>() };
            }
        }() };
        return &value;
    }

}
