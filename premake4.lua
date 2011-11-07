solution "GLDemo_Shapes"
  location ("build/" .. os.get() .. (_ACTION and ("/" .. _ACTION) or ""))
  targetdir ("bin/" .. os.get())
  kind "ConsoleApp"
  language "C"
  configurations { "Debug", "Release" }
  configuration "Debug"
    flags { "Symbols", "ExtraWarnings", "FatalWarnings" }
  configuration "Release"
    flags { "OptimizeSpeed" }

  project "gldemo_shapes"
    links { "GL", "GLU", "glfw" }
    files { "src/*.h", "src/*.c" }


