cmake_minimum_required(VERSION 3.7)
project(GameBoyEmu VERSION 0.1.0)

#Include SDL
#include("/usr/lib/cmake/SDL2/SDL2Targets.cmake")
#find_package(SDL2 REQUIRED)
#message(“SDL2 libraries: ${SDL2_LIBRARIES}”)
#message(“SDL2 includes: ${SDL2_INCLUDE_DIRS}”)
set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
set(SDL2_LIBRARIES "/usr/lib/libSDL2.so")
include_directories(${SDL2_INCLUDE_DIRS})

#Include OpenGL2
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


#include project files
include_directories(include)
file(GLOB SOURCES "src/*.c")

#Generate executable from sources
add_executable(emu ${SOURCES})

#Link SDL libraries to the executable
target_link_libraries(emu ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
