#include <cstdlib>

import std;
import bamboo.log;
import bamboo.model;
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

    try {
        auto mfa_stream{ std::make_shared<bamboo::mfa::Stream>(argv[1]) };
        bamboo::logger()->set_stream(std::weak_ptr{ mfa_stream });

        bamboo::Project project;
        *mfa_stream >> project;
    } catch (const std::exception& error) {
        bamboo::logger()->error(error.what());
        bamboo::logger()->error("See log for more details.");
        return EXIT_FAILURE;
    }
}
