#include <spdlog/spdlog.h>

import std;
import bamboo.stream;
import bamboo.mfa;

int main() {
    try {
        bamboo::Stream stream{ "" };
        bamboo::mfa::File file;
        stream >> file;
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
