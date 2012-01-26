solution "BasicDemo"

language "C++"
configurations {"Release", "Debug"}
configuration "Release"
do
    flags {
        "Optimize",
        "StaticRuntime",
        "NoMinimalRebuild",
        "FloatFast",
    }
end

configuration "Debug"
do
    flags {
        "Symbols",
        "StaticRuntime" ,
        "NoMinimalRebuild",
        "NoEditAndContinue" ,
        "FloatFast",
    }
end

configuration {"Windows"}
do
    defines { 
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_SECURE_NO_DEPRECATE",
        "_WIN32",
        "_WINDOWS",
    }
end

------------------------------------------------------------------------------
project "BasicDemo"

language "C++"
kind "ConsoleApp"
defines {
    'FREEGLUT_STATIC',
}
includedirs {
    "../bullet-2.79",
    "../freeglut/include",
    "../gldrawer",
}
files {
    'bulletworld.cpp',
    'bulletworld.h',
    'camera.cpp',
    'camera.h',
    'DemoApplication.cpp',
    'DemoApplication.h',
    'GlutStuff.cpp',
    'GlutStuff.h',
    'main.cpp',
    'texture.cpp',
    'texture.h',
}
links {
    'gldrawer',
    "BulletDynamics", "BulletCollision", "LinearMath",
    "freeglut", "glu32", "opengl32",
    "user32", "gdi32", "winmm",
}

configuration "Debug"
do
  defines { "DEBUG" }
  flags { "Symbols" }
  libdirs { "../debug" }
  targetdir "../debug"
end

configuration "Release"
do
  defines { "NDEBUG" }
  flags { "Optimize" }
  libdirs { "../release" }
  targetdir "../release"
end

