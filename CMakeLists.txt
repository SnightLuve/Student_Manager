cmake_minimum_required(VERSION 3.10)
project(Student_Manager C)

set(CMAKE_C_STANDARD 11)

# Các module khác (search_sort, stats, input, menu) sẽ được thêm
# vào đây khi TV2–TV4 hoàn thành.
add_executable(Student_Manager
        src/main.c
        src/student.c
        src/list.c
        src/file_io.c)

target_include_directories(Student_Manager PRIVATE include)
target_compile_definitions(Student_Manager PRIVATE
        DEFAULT_SAMPLE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat")

if (NOT MSVC)
    target_compile_options(Student_Manager PRIVATE -Wall -Wextra)
endif ()

# Cong cu TV3: tao lai file mau bang dung compiler/struct cua du an.
add_executable(create_sample_data EXCLUDE_FROM_ALL
        tools/create_sample_data.c src/student.c src/list.c src/file_io.c)
target_include_directories(create_sample_data PRIVATE include)

include(CTest)
if (BUILD_TESTING)
    add_executable(file_io_tests
            tests/test_file_io.c src/student.c src/list.c src/file_io.c)
    target_include_directories(file_io_tests PRIVATE include)
    if (NOT MSVC)
        target_compile_options(file_io_tests PRIVATE -Wall -Wextra)
    endif ()
    add_test(NAME file_io COMMAND file_io_tests)
    add_test(NAME file_io_sample COMMAND file_io_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/data/students.dat")
    add_test(NAME menu_file_io COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:Student_Manager>"
            "-DSAMPLE=${CMAKE_CURRENT_SOURCE_DIR}/data/students.dat"
            "-DSAMPLE_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_menu.cmake")
endif ()

if (NOT MSVC)
    target_compile_options(create_sample_data PRIVATE -Wall -Wextra)
endif ()

# Bật log debug:  cmake -DENABLE_DEBUG=ON ..
option(ENABLE_DEBUG "Bat macro DEBUG" OFF)
if (ENABLE_DEBUG)
    target_compile_definitions(Student_Manager PRIVATE DEBUG)
endif ()
