file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/main.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/RUNTIME)
add_executable(runtime)

target_sources(runtime PRIVATE ${SOURCES})

target_link_libraries(runtime runeforge)

target_include_directories(runtime PRIVATE include)