file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR)
add_executable(editor)

target_sources(editor PRIVATE ${SOURCES})

target_link_libraries(editor runeforge)

target_include_directories(editor PRIVATE include)