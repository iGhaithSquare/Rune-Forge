file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR)
add_executable(editor)

target_sources(editor PRIVATE ${SOURCES})

target_link_libraries(editor runeforge)

target_include_directories(editor PRIVATE include)

add_custom_target(
    runeforge_sdk ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/include
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/runtime
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/licenses/CJSON
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/../runeforge/include/runeforge.h ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/include
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/_deps/cjson-src/LICENSE ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/licenses/CJSON
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/GAVEN/ ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/RUNEFORGE/ ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/RUNEWALL/ ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/RUNTIME/ ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/EDITOR/runtime/
)