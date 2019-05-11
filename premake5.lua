-- premake5.lua

workspace "openGL"
    configurations { "Debug", "Release" }
    platforms { "x64", "x86" }
	system "windows"

    filter "platforms:x86"
        architecture "x86"

    filter "platforms:x64"
        architecture "x86_64"

    filter "configurations:Debug"
        optimize "Debug"
    filter "configurations:Release"
        optimize "On"

	outputdir = "%{cfg.platform}-%{cfg.buildcfg}"

project "ImGui"
    kind "StaticLib"
    language "C++"
    location "ImGui"
    files 
    {
         "ImGui/src/**.cpp",
		 "Imgui/src/**.c",
         "Imgui/src/**.h",
         "ImGui/src/**.hpp"
    }
	removefiles "Imgui/src/main.cpp"
    includedirs
    {
        "%{wks.location}/dependencies/Includes"
    }
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")

project "objExpander"
	kind "ConsoleApp"
	language "C++"
	location "objExpander"
	files
	{
		"objExpander/src/**.cpp",
		"objExpander/src/**.h"
	}
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")

project "openGL"
	kind "WindowedApp"
	language "C++"
	location "openGL"
	files
	{
		"openGL/src/**.cpp",
		"openGL/src/**.c",
		"openGL/src/**.h",
		"openGL/src/**.hpp"
	}
	includedirs
	{
		"%{wks.location}/dependencies/Includes",
		"%{prj.location}/src",
		"%{prj.location}/Vendor",
		"%{wks.location}/stb_image/src",
		"%{wks.location}/ImGui/src",
		"%{prj.location}/src/Utils"
	}
	libdirs
	{
		"%{wks.location}/dependencies/lib-vc2015",
		"%{wks.location}/dependencies"
	}
	links
	{
		"Imgui",
		"stb_image"
	}
	filter "platforms:x64"
		links
		{
			"glfw3_64",
			"openGL32",
			"glew64s"
		}
	filter "platforms:x86"
		links
		{
			"glfw3_32",
			"openGL32",
			"glew32s"
		}
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")

project "stb_image"
	kind "StaticLib"
	language "C++"
	location "stb_image"
	files
	{
		"stb_image/src/**.cpp",
		"stb_image/src/**.c",
		"stb_image/src/**.h",
		"stb_image/src/**.hpp"
	}
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/" .. outputdir .. "/%{prj.name}")