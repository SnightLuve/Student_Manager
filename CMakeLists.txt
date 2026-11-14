cmake_minimum_required(VERSION 3.10)
project(Student_Manager C)

set(CMAKE_C_STANDARD 11)

# Các module input, menu sẽ được thêm vào đây khi TV4 hoàn thành.
add_executable(Student_Manager
        src/main.c
        src/student.c
        src/list.c
        src/file_io.c
        src/search_sort.c
        src/stats.c)

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
    # Test chi doc file mau (sample_students.dat). KHONG dung data/students.dat:
    # do la file du lieu that, chuong trinh autosave vao do nen so SV thay doi.
    add_test(NAME file_io_sample COMMAND file_io_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat")
    add_executable(search_sort_stats_tests
            tests/test_search_sort_stats.c src/student.c src/list.c src/file_io.c
            src/search_sort.c src/stats.c)
    target_include_directories(search_sort_stats_tests PRIVATE include)
    if (NOT MSVC)
        target_compile_options(search_sort_stats_tests PRIVATE -Wall -Wextra)
    endif ()
    add_test(NAME search_sort_stats COMMAND search_sort_stats_tests
            "${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat")
    add_executable(student_list_tests
            tests/test_student_list.c src/student.c src/list.c
            src/search_sort.c src/stats.c)
    target_include_directories(student_list_tests PRIVATE include)
    if (NOT MSVC)
        target_compile_options(student_list_tests PRIVATE -Wall -Wextra)
    endif ()
    add_test(NAME student_list COMMAND student_list_tests)
    add_test(NAME menu_input COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:Student_Manager>"
            "-DSAMPLE=${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_input.cmake")
    add_test(NAME menu_file_io COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:Student_Manager>"
            "-DSAMPLE=${CMAKE_CURRENT_SOURCE_DIR}/data/sample_students.dat"
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
