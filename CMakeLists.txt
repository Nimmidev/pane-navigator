cmake_minimum_required(VERSION 3.13)
project(pane-navigator C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(pane-navigator src/main.c src/common.c src/i3.c src/tmux.c src/x11.c src/nvim.c)
include_directories(src/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(PROCPS REQUIRED libprocps)
pkg_check_modules(MSGPACK REQUIRED msgpack)

target_link_libraries(pane-navigator ${X11_LIBRARIES} ${PROCPS_LIBRARIES} ${MSGPACK_LIBRARIES})
target_include_directories(pane-navigator PUBLIC ${X11_INCLUDE_DIRS} ${PROCPS_INCLUDE_DIRS} ${MSGPACK_INCLUDE_DIRS})
target_compile_options(pane-navigator PUBLIC ${X11_CFLAGS_OTHER} ${PROCPS_CFLAGS_OTHER} ${MSGPACK_CFLAGS_OTHER})

install(TARGETS pane-navigator DESTINATION bin)
