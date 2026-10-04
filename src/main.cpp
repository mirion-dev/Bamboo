#include <cxxopts.hpp>
#include <spdlog/spdlog.h>

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
    options.show_positional_help();

    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch (const std::exception& error) {
        std::println(std::cerr, "{}.", error.what());
        return EXIT_FAILURE;
    }

    if (!result.unmatched().empty() || result.count("path") != 1) {
        std::println(std::cerr, "{}", options.help());
        return EXIT_FAILURE;
    }
    std::filesystem::path path{ result["path"].as<std::string>() };

    enum class Format {
        mfa,
        vcs_friendly
    } format;
    {
        static constexpr std::array FORMAT{ "mfa", "vcsf" };

        auto raw_format{ result["format"].as<std::string>() };
        if (raw_format == "auto") {
            format = std::filesystem::is_directory(path) ? Format::vcs_friendly : Format::mfa;
        } else {
            auto iter{ std::ranges::find(FORMAT, raw_format) };
            if (iter == FORMAT.end()) {
                std::println(std::cerr, "Unknown format (support: {:n:}).", FORMAT);
                return EXIT_FAILURE;
            }

            format = static_cast<Format>(iter - FORMAT.begin());
        }
    }
    if (format == Format::vcs_friendly) {
        std::println(std::cerr, "To be implemented.");
        return EXIT_FAILURE;
    }

    {
        static constexpr std::array LEVEL{ "trace", "debug", "info", "warn", "error", "critical", "off" };

        auto raw_level{ result["level"].as<std::string>() };
        auto iter{ std::ranges::find(LEVEL, raw_level) };
        if (iter == LEVEL.end()) {
            std::println(std::cerr, "Unknown level (support: {:n:}).", LEVEL);
            return EXIT_FAILURE;
        }

        bamboo::logger()->set_level(static_cast<spdlog::level::level_enum>(iter - LEVEL.begin()));
    }

    try {
        auto mfa_stream{ std::make_shared<bamboo::mfa::Stream>(result["path"].as<std::string>()) };
        bamboo::logger()->set_stream(std::weak_ptr{ mfa_stream });

        bamboo::Project project;
        *mfa_stream >> project;
    } catch (const std::exception& error) {
        bamboo::logger()->error(std::string{ error.what() } + '.');
        std::println(std::cerr, "See log for more details.");
        return EXIT_FAILURE;
    }
}
