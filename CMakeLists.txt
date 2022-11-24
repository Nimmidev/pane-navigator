cmake_minimum_required(VERSION 3.13)
project(i3-tmux-vim-nav C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(i3-tmux-vim-nav src/main.c src/common.c src/i3.c)
include_directories(src/include)

target_link_libraries(i3-tmux-vim-nav libxdo.so libprocps.so)
