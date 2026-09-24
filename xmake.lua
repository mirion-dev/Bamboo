add_rules("mode.debug", "mode.release")

set_languages("c++latest")
set_warnings("all", "error")

add_requires("vcpkg::spdlog", {debug = is_mode("debug")})

target("Bamboo")
    set_kind("binary")
    add_files("Bamboo/*.cpp", "Bamboo/*.ixx")
    add_packages("vcpkg::spdlog")

    add_defines("NOMINMAX", "UNICODE", "_UNICODE", "_CONSOLE")

    add_cxxflags("/JMC", "/sdl", "/permissive-", "/Zc:preprocessor", "/external:W0", {tools = "cl", force = true})

    set_targetdir(is_mode("debug") and "build/debug" or "build/release")
    set_rundir("tests")
    set_runargs("game.mfa")

    if is_mode("release") then
        set_symbols("debug")
        add_cxxflags("/Oi", "/Gy", {tools = "cl"})
        set_policy("build.optimization.lto", true)
    end
