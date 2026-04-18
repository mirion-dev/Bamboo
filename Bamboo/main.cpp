#include <spdlog/spdlog.h>

import std;
import bamboo.general;
import bamboo.mfa;

int main() {
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    try {
        bamboo::mfa::Stream mfa_stream{ "" };
        bamboo::File file;
        mfa_stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
