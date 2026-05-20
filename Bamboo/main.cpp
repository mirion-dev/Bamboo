#include <spdlog/spdlog.h>

import std;
import bamboo.general;
import bamboo.mfa;

int main(int argc, char** argv) {
    spdlog::set_pattern("[%^%l%$] %v");
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    if (argc < 2) {
        std::println("Usage: bamboo [PATH]");
        return 1;
    }

    if (argc > 2) {
        std::println("Too many arguments. Did you forget to add quotes around the path?");
        return 1;
    }

    try {
        bamboo::mfa::Stream mfa_stream{ argv[1] };
        bamboo::File file;
        mfa_stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
