file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/DEMO)
add_executable(demo)

target_sources(demo PRIVATE ${SOURCES})

target_link_libraries(demo runeforge)

target_include_directories(demo PRIVATE include)