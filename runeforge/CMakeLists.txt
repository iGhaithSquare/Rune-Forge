file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/../bin" CACHE PATH "" FORCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/RUNEFORGE)

add_library(runeforge STATIC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

target_sources(runeforge PRIVATE ${SOURCES})
target_include_directories(runeforge
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(runeforge PUBLIC runewall)



install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION $<CONFIG>/EDITOR/include/)
install(DIRECTORY ${CMAKE_BINARY_DIR}/_deps/runewall-src/runewall/include/ DESTINATION $<CONFIG>/EDITOR/include/)
install(DIRECTORY ${CMAKE_BINARY_DIR}/_deps/gaven-src/gaven/include/ DESTINATION $<CONFIG>/EDITOR/include/)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION $<CONFIG>/EDITOR/src/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/_deps/runewall-src/runewall/src/ DESTINATION $<CONFIG>/EDITOR/src/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/_deps/gaven-src/gaven/src/ DESTINATION $<CONFIG>/EDITOR/src/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${CMAKE_BINARY_DIR}/_deps/cjson-src/ DESTINATION $<CONFIG>/EDITOR/include/ FILES_MATCHING PATTERN "*.h")

install(TARGETS runeforge cjson runewall gaven ARCHIVE DESTINATION $<CONFIG>/EDITOR)