cmake_minimum_required(VERSION 3.10)
project(Student_Manager C)

set(CMAKE_C_STANDARD 11)

# Các module khác (search_sort, stats, file_io, input, menu) sẽ được thêm
# vào đây khi TV2–TV4 hoàn thành.
add_executable(Student_Manager
        src/main.c
        src/student.c
        src/list.c)

target_include_directories(Student_Manager PRIVATE include)

if (NOT MSVC)
    target_compile_options(Student_Manager PRIVATE -Wall -Wextra)
endif ()

# Bật log debug:  cmake -DENABLE_DEBUG=ON ..
option(ENABLE_DEBUG "Bat macro DEBUG" OFF)
if (ENABLE_DEBUG)
    target_compile_definitions(Student_Manager PRIVATE DEBUG)
endif ()
