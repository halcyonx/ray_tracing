workspace "RayTracing"
    architecture "x64"
    startproject "RayTracing"
    location "proj"

    configurations {
        "Debug",
        "Release"
    }

project "RayTracing"
    kind "ConsoleApp"
    language "C++"
    targetdir   "build/%{cfg.architecture}/bin/%{cfg.buildcfg}"
    objdir      "build/%{cfg.architecture}/obj/%{cfg.buildcfg}"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    filter "configurations:Release" 
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Debug"
        defines {"NDEBUG"}
        optimize "On"
