#include <spdlog/spdlog.h>

import std;
import bamboo.mfa;

int main() {
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    try {
        bamboo::mfa::Stream stream{ "" };
        bamboo::mfa::File file;
        stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
