#include <spdlog/spdlog.h>

import std;
import bamboo.stream;
import bamboo.mfa;

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        bamboo::Stream stream{ "" };
        bamboo::mfa::File file;
        stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
