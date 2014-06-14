solution "Shapes"
  location ("build/" .. os.get() .. (_ACTION and ("/" .. _ACTION) or ""))
  targetdir ("bin/" .. os.get())
  kind "ConsoleApp"
  language "C"
  configurations { "Debug", "Release" }
  configuration "Debug"
    flags { "Symbols", "ExtraWarnings", "FatalWarnings" }
  configuration "Release"
    flags { "OptimizeSpeed" }

  project "shapes"
    links { "m", "GL", "GLU", "glfw" }
    files { "src/*.h", "src/*.c" }


