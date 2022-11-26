cmake_minimum_required(VERSION 3.13)
project(pane-navigator C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(pane-navigator src/main.c src/common.c src/i3.c src/tmux.c src/x11.c src/nvim.c)
include_directories(src/include)
target_link_libraries(pane-navigator libxdo.so libprocps.so libmsgpackc.so)

install(TARGETS pane-navigator DESTINATION bin)
