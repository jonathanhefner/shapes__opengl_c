solution "Shapes"
  targetdir "bin"
  objdir "bin/obj"
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


