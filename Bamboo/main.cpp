#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

import std;
import bamboo.general;
import bamboo.mfa;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::println("Usage: bamboo [PATH]");
        return 1;
    }

    if (argc > 2) {
        std::println("Too many arguments. Did you forget to add quotes around the path?");
        return 1;
    }

    spdlog::init_thread_pool(8192, 1);

    auto console_sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
    auto file_sink{ std::make_shared<spdlog::sinks::basic_file_sink_mt>("bamboo.log", true) };
    console_sink->set_level(spdlog::level::info);

    spdlog::sinks_init_list sinks{ console_sink, file_sink };
    auto logger{ std::make_shared<spdlog::async_logger>("main", sinks, spdlog::thread_pool()) };
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%^%l%$] %v");

    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    try {
        bamboo::mfa::Stream mfa_stream{ argv[1] };
        bamboo::File file;
        mfa_stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
        spdlog::error("See bamboo.log for more details.");
    }

    spdlog::shutdown();
}
