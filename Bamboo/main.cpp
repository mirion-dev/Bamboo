#include <spdlog/spdlog.h>

import std;
import bamboo.diag;
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

    auto logger{ std::make_shared<bamboo::Logger<std::istream>>("default", "bamboo.log") };
    logger->console_sink()->set_level(spdlog::level::info);
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%^%l%$] [%&] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    try {
        auto mfa_stream{ std::make_shared<bamboo::mfa::Stream>(argv[1]) };
        logger->set_stream(mfa_stream);

        bamboo::File file;
        *mfa_stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
        spdlog::error("See bamboo.log for more details.");
    }
}
