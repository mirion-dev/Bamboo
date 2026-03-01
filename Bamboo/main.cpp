#include <spdlog/spdlog.h>

import std;
import bamboo.stream;
import bamboo.mfa;

int main() {
    try {
        bamboo::Stream stream{ "" };
        bamboo::mfa::Header header;

        spdlog::info("Start parsing.");
        stream.load(header);
    }
    catch (const std::exception& error) {
        spdlog::error(error.what());
    }
}
