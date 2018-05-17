cmake_minimum_required(VERSION 3.7)
project(GameBoyEmu VERSION 0.1.0)

include_directories(include)
file(GLOB SOURCES "src/*.c")

add_executable(emu ${SOURCES})
