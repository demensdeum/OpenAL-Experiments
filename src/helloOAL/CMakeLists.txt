cmake_minimum_required(VERSION 3.29)

set(PROJECT_NAME HelloOAL)

project(${PROJECT_NAME})

find_package(OpenAL REQUIRED)

if(EMSCRIPTEN)
set(CMAKE_C_FLAGS " \
-s EXCEPTION_DEBUG \
-fexceptions \
--preload-file test.wav \
--bind \
--use-preload-plugins \
-Wall \
-Wextra \
-Werror=return-type \
-lopenal \
-s ASSERTIONS=1 \
-w \
-g4 \
-s DISABLE_EXCEPTION_CATCHING=0 \
-O3 \
--no-heap-copy \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXIT_RUNTIME=1")
endif()

add_executable(${PROJECT_NAME} "main.c")

target_link_libraries(${PROJECT_NAME} openal)