#include <cxxopts.hpp>

import std;
import bamboo.log;
import bamboo.model;
import bamboo.mfa;

int main(int argc, char** argv) {
    cxxopts::Options options{ "bamboo", "A converter between MFA and a VCS-friendly format." };
    options.add_options(
        "",
        { { "path", "File path", cxxopts::value<std::string>() },
          { "f,format", "File format", cxxopts::value<std::string>()->default_value("auto") },
          { "l,level", "Log level", cxxopts::value<std::string>()->default_value("info") } }
    );
    options.parse_positional("path");

    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch (const std::exception& error) {
        std::println(std::cerr, "{}", error.what());
        return EXIT_FAILURE;
    }
    if (!result.contains("path")) {
        std::println(std::cerr, "{}", options.help());
        return EXIT_FAILURE;
    }

    try {
        auto mfa_stream{ std::make_shared<bamboo::mfa::Stream>(result["path"].as<std::string>()) };
        bamboo::logger()->set_stream(std::weak_ptr{ mfa_stream });

        bamboo::Project project;
        *mfa_stream >> project;
    } catch (const std::exception& error) {
        bamboo::logger()->error(error.what());
        std::println(std::cerr, "See log for more details.");
        return EXIT_FAILURE;
    }
}
