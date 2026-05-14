cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/)
add_library(Sandbox SHARED ${SOURCES})
target_link_libraries(Sandbox "C:/dev/Visual Studio Code/Rune-Forge/bin/Debug/EDITOR/libruneforge.a")
target_include_directories(Sandbox PRIVATE "C:/dev/Visual Studio Code/Rune-Forge/bin/Debug/EDITOR/include")
