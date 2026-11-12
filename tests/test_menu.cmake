# Run the actual console menu against a disposable copy, never the shipped data.
set(data "${CMAKE_CURRENT_BINARY_DIR}/menu_test_students.dat")
set(csv "${CMAKE_CURRENT_BINARY_DIR}/menu_test_report.csv")
set(input "${CMAKE_CURRENT_BINARY_DIR}/menu_test_input.txt")
set(bad "${CMAKE_CURRENT_BINARY_DIR}/menu_test_bad.dat")
configure_file("${SAMPLE}" "${data}" COPYONLY)

function(run_menu commands)
    file(WRITE "${input}" "${commands}")
    execute_process(COMMAND "${PROGRAM}" -f "${data}"
            INPUT_FILE "${input}" OUTPUT_VARIABLE output ERROR_VARIABLE error
            RESULT_VARIABLE result TIMEOUT 10)
    if (NOT "${result}" STREQUAL "0")
        message(FATAL_ERROR "Menu failed: ${result}\n${output}\n${error}")
    endif ()
    set(output "${output}" PARENT_SCOPE)
endfunction()

function(require_output text)
    string(FIND "${output}" "${text}" found)
    if (found LESS 0)
        message(FATAL_ERROR "Missing '${text}' in menu output:\n${output}")
    endif ()
endfunction()

# Read record 1 from disk immediately after deletion, before manual/exit save.
run_menu("10\n1\n\n5\nSV001\ny\n\n10\n1\n\n9\n${csv}\n\n0\n")
require_output("Da doc 18 sinh vien")
require_output("8. Import du lieu mau")
require_output("9. Xuat bao cao CSV")
require_output("10. Xem ban ghi trong file da luu")
require_output("Nguyen Van An")
require_output("Da xoa sinh vien SV001")
require_output("Tran Thi Binh")
require_output("Da luu 17 sinh vien")
require_output("Da xuat CSV")
file(STRINGS "${csv}" rows)
list(LENGTH rows count)
if (NOT count EQUAL 18)
    message(FATAL_ERROR "CSV should have 1 header and 17 students, got ${count}")
endif ()
file(READ "${csv}" report)
if (report MATCHES "SV001,")
    message(FATAL_ERROR "Deleted student is still in CSV")
endif ()

run_menu("3\nSV001\n\n10\n1\n\n0\n")
require_output("Da doc 17 sinh vien")
require_output("Khong tim thay sinh vien SV001")
require_output("Tran Thi Binh")

# Read the score FROM DISK before exit save, not just the in-memory edit display.
run_menu("4\nSV002\n3\n1\n6\n0\n\n10\n1\n\n0\n")
require_output("DTB moi = 8.08")
string(REGEX MATCH "XEM BAN GHI TRONG FILE DA LUU[^=]*8[.]08" persisted "${output}")
if (NOT persisted)
    message(FATAL_ERROR "Edited score was not autosaved before exit:\n${output}")
endif ()
run_menu("3\nSV002\n\n0\n")
require_output("8.08")

# Add then read the new disk record, without choosing manual save.
run_menu("1\nSV019\nTest Autosave\nTEST_CLASS\n7\n7\n7\n7\n7\n\n10\n18\n\n0\n")
string(REGEX MATCH "XEM BAN GHI TRONG FILE DA LUU[^=]*Test Autosave" persisted "${output}")
if (NOT persisted)
    message(FATAL_ERROR "Added student was not autosaved before exit:\n${output}")
endif ()

# Each edit branch must autosave, including text and all-subject scores.
run_menu("4\nSV019\n1\nNew Autosave Name\n2\nNEW_CLASS\n4\n9\n9\n9\n9\n9\n0\n\n10\n18\n\n0\n")
string(REGEX MATCH "XEM BAN GHI TRONG FILE DA LUU[^=]*New Autosave Name[^=]*NEW_CLASS[^=]*9[.]00" persisted "${output}")
if (NOT persisted)
    message(FATAL_ERROR "Text/all-score edits were not autosaved:\n${output}")
endif ()

# Broken startup data must not be replaced with an empty list.
file(WRITE "${bad}" "BROKEN DATA")
execute_process(COMMAND "${PROGRAM}" -f "${bad}"
        OUTPUT_VARIABLE output ERROR_VARIABLE error RESULT_VARIABLE result TIMEOUT 10)
file(READ "${bad}" preserved)
if (NOT "${result}" STREQUAL "1" OR NOT preserved STREQUAL "BROKEN DATA")
    message(FATAL_ERROR "Corrupt input file was not protected: ${result} ${error}")
endif ()

# A stale temporary file causes a visible save failure, with the old data intact.
file(READ "${data}" before HEX)
file(WRITE "${data}.tmp" "KEEP")
run_menu("5\nSV019\ny\n\n0\nn\n\n0\ny\n")
require_output("Khong luu duoc")
require_output("Thay doi moi CHUA DUOC LUU, chi con trong RAM")
require_output("Thoat ma khong luu")
file(READ "${data}" after HEX)
if (NOT before STREQUAL after)
    message(FATAL_ERROR "Failed save changed the original data")
endif ()

file(REMOVE "${data}.tmp")
file(REMOVE "${data}")
# EOF at the menu exits and saves an empty first-run database rather than looping.
run_menu("")
require_output("Da doc 0 sinh vien")
require_output("Da luu 0 sinh vien")

# Import from the dedicated source file, read from disk before exit, repeat safely.
file(READ "${SAMPLE_SOURCE}" sampleBefore HEX)
run_menu("8\n\n10\n18\n\n8\n\n0\n")
require_output("Da import 18 sinh vien mau, bo qua 0 ma da ton tai")
require_output("Ta Hai Yen")
require_output("Da import 0 sinh vien mau, bo qua 18 ma da ton tai")
run_menu("4\nSV001\n1\nKeep My Name\n0\n\n5\nSV002\ny\n\n8\n\n10\n1\n\n0\n")
require_output("Da import 1 sinh vien mau, bo qua 17 ma da ton tai")
string(REGEX MATCH "XEM BAN GHI TRONG FILE DA LUU[^=]*Keep My Name" preserved "${output}")
if (NOT preserved)
    message(FATAL_ERROR "Import overwrote an existing student's edits")
endif ()
file(READ "${SAMPLE_SOURCE}" sampleAfter HEX)
if (NOT sampleBefore STREQUAL sampleAfter)
    message(FATAL_ERROR "Import changed the source sample file")
endif ()

file(REMOVE "${data}" "${csv}" "${input}" "${bad}")
message(STATUS "Menu integration passed: load/delete/save/CSV/random access/restart/edit/errors")
