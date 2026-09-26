add_rules("mode.debug", "mode.release")

set_languages("c++latest")
set_warnings("all", "error")

add_requires("vcpkg::spdlog", {debug = is_mode("debug"), configs = {shared = true}})

add_defines("NOMINMAX", "UNICODE", "_UNICODE", "_CONSOLE")

add_cxxflags("/JMC", "/sdl", "/permissive-", "/Zc:preprocessor", {tools = "cl", force = true})

if is_mode("release") then
    set_symbols("debug")
    set_policy("build.optimization.lto", true)
    add_cxxflags("/Oi", "/Gy", {tools = "cl"})
end

target("bamboo")
    set_kind("binary")
    add_files("src/**.cpp", "src/**.ixx")
    add_packages("vcpkg::spdlog")

    set_targetdir(is_mode("debug") and "build/debug" or "build/release")
    set_rundir("tests")
    set_runargs("game.mfa")
