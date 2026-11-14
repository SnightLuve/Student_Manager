# Kiem tra nhap lieu cua menu that (readLine/readNonEmpty/readId/readInt/readFloat,
# chan CSV ghi de file du lieu, tham so dong lenh). Luon chay tren ban sao file mau.
set(dir "${CMAKE_CURRENT_BINARY_DIR}")
set(data "${dir}/input_test_students.dat")
set(other "${dir}/input_test_other.dat")
set(csv "${dir}/input_test_report.csv")
set(input "${dir}/input_test_stdin.txt")

function(fresh_data)
    file(REMOVE "${data}" "${data}.tmp")
    configure_file("${SAMPLE}" "${data}" COPYONLY)
endfunction()

function(run_menu commands)
    file(WRITE "${input}" "${commands}")
    execute_process(COMMAND "${PROGRAM}" -f "${data}"
            INPUT_FILE "${input}" OUTPUT_VARIABLE output ERROR_VARIABLE error
            RESULT_VARIABLE result TIMEOUT 10
            ENCODING UTF8)   # Windows mac dinh giai ma theo code page ANSI → hong UTF-8
    if (NOT "${result}" STREQUAL "0")
        message(FATAL_ERROR "Menu failed: ${result}\n${output}\n${error}")
    endif ()
    set(output "${output}" PARENT_SCOPE)
endfunction()

function(require_output text)
    string(FIND "${output}" "${text}" found)
    if (found LESS 0)
        message(FATAL_ERROR "Missing '${text}' in output:\n${output}")
    endif ()
endfunction()

function(forbid_output text)
    string(FIND "${output}" "${text}" found)
    if (NOT found LESS 0)
        message(FATAL_ERROR "Unexpected '${text}' in output:\n${output}")
    endif ()
endfunction()

# So lan xuat hien chinh xac cua text (khong phai regex).
function(require_count text expected)
    set(rest "${output}")
    set(n 0)
    string(LENGTH "${text}" len)
    while (TRUE)
        string(FIND "${rest}" "${text}" pos)
        if (pos LESS 0)
            break()
        endif ()
        math(EXPR n "${n} + 1")
        math(EXPR pos "${pos} + ${len}")
        string(SUBSTRING "${rest}" ${pos} -1 rest)
    endwhile ()
    if (NOT n EQUAL expected)
        message(FATAL_ERROR "'${text}' xuat hien ${n} lan, mong doi ${expected}:\n${output}")
    endif ()
endfunction()

set(scores5 "5\n5\n5\n5\n5\n")

# 1) Ma SV: TAB/khoang trang bi tu choi; 12 ky tu bi tu choi (khong cat ngam);
#    dung 11 ky tu duoc nhan; chu thuong duoc chuan hoa; trung ma bi chan.
fresh_data()
run_menu("1\nSV\t99\nSV 99\nSV0000000001\nsv001\nsv000000001\nTen Hop Le\nCNTT1\n${scores5}\n0\n")
require_count("Ma SV khong duoc chua khoang trang" 2)
require_count("Qua dai: toi da 11 byte" 1)
require_output("Ma SV001 da ton tai")
require_output("Da them")
require_output("Da luu 19 sinh vien")
forbid_output("Khong luu duoc")
run_menu("3\nSV000000001\n\n0\n")
require_output("Da doc 19 sinh vien")
require_output("Ten Hop Le")

# 2) Ho ten / lop: dung gioi han byte thi nhan, vuot 1 byte hoac ten co dau qua dai
#    thi bao loi; ten co chu so bi tu choi; chi co khoang trang = de trong.
string(REPEAT "A" 50 name50)
string(REPEAT "A" 51 name51)
string(REPEAT "C" 20 class20)
string(REPEAT "C" 21 class21)
set(utf8Name "Nguyễn Thị Phương Thảo Hoàng Ngọc Bích Diệp Anh Thư")
fresh_data()
run_menu("1\nSV100\n${name51}\n${utf8Name}\nTen 2\n   \n${name50}\n${class21}\n${class20}\n${scores5}\n3\nSV100\n\n0\n")
require_count("Qua dai: toi da 50 byte" 2)
require_count("Ho ten khong duoc chua chu so" 1)
require_count("Khong duoc de trong" 1)
require_count("Qua dai: toi da 20 byte" 1)
require_output("Ho ten       : ${name50}")
require_output("Lop          : ${class20}")
require_output("Da luu 19 sinh vien")

# Ten tieng Viet co dau vua gioi han van luu/doc lai dung.
set(utf8Short "Nguyễn Thị Thảo")
run_menu("1\nSV101\n${utf8Short}\nCNTT1\n${scores5}\n0\n")
run_menu("3\nSV101\n\n0\n")
require_output("Ho ten       : ${utf8Short}")

# 3) So nguyen o menu: chu, so kem chu, tran long, ngoai khoang, so am deu bi tu choi.
fresh_data()
run_menu("abc\n5abc\n99999999999999999999\n11\n-1\n 7 \n\n0\n")
require_count("Vui long nhap so nguyen trong khoang [0, 10]" 5)
require_output("So sinh vien: 18")

# 4) Diem: ngoai khoang, nan, inf, dau phay thap phan bi tu choi; 1e1 = 10 hop le.
fresh_data()
run_menu("1\nSV200\nTen Diem\nL1\n10.01\n-0.5\nnan\ninf\n7,5\n1e1\n0\n0\n0\n0\n\n3\nSV200\n\n0\n")
require_count("Vui long nhap so trong khoang [0.0, 10.0]" 5)
require_output("Diem TB      : 2.00")

# 5) Dong rac 300 ky tu: bao loi DUNG 1 lan, phan thua khong bi doc thanh lenh tiep.
string(REPEAT "x" 300 junk)
string(REPEAT "7" 300 junkDigits)
fresh_data()
run_menu("${junk}\n${junkDigits}\n7\n\n0\n")
require_count("Vui long nhap so nguyen" 2)
require_count("So sinh vien: 18" 1)

# 6) Dong cuoi khong co '\n' van duoc nhan (khong bi coi la qua dai).
fresh_data()
run_menu("7\n\n0")
require_output("Tam biet")

# 7) Het input giua luc them: khong them ban ghi do dang, van luu va thoat binh thuong.
fresh_data()
run_menu("1\nSV300\nTen Do Dang\n")
require_output("Da luu 18 sinh vien")
forbid_output("Da them")

# 8) Cac nhanh khac: tim/sua/xoa ma khong ton tai, xac nhan sai, huy xoa.
fresh_data()
run_menu("3\nSV999\n\n4\nSV999\n\n5\nSV002\nmaybe\nn\n\n0\n")
require_count("Khong tim thay sinh vien SV999" 2)
require_output("Nhap y hoac n")
require_output("Da huy")
require_output("Da luu 18 sinh vien")

# 9) CSV khong duoc ghi de file du lieu, ke ca viet duong dan khac ("/./")
#    hoac la file du lieu .dat khac; file nhi phan van nguyen ven.
fresh_data()
configure_file("${SAMPLE}" "${other}" COPYONLY)
file(READ "${other}" otherBefore HEX)
run_menu("9\n${dir}/./input_test_students.dat\n\n9\n${other}\n\n9\n${csv}\n\n0\n")
require_count("Hay chon file CSV khac file nhi phan" 2)
require_output("Da xuat CSV: ${csv}")
file(READ "${other}" otherAfter HEX)
if (NOT otherBefore STREQUAL otherAfter)
    message(FATAL_ERROR "CSV da ghi de file du lieu khac")
endif ()
run_menu("7\n\n0\n")
require_output("Da doc 18 sinh vien")
file(STRINGS "${csv}" rows)
list(LENGTH rows count)
if (NOT count EQUAL 19)
    message(FATAL_ERROR "CSV phai co 1 tieu de + 18 SV, co ${count}")
endif ()

# 10) Tham so dong lenh sai → ma thoat 1, in cach dung, khong tao/doi file.
foreach (args "-f" "-x;a" "-f;a;b")
    execute_process(COMMAND "${PROGRAM}" ${args}
            OUTPUT_VARIABLE output ERROR_VARIABLE error RESULT_VARIABLE result TIMEOUT 10)
    if (NOT "${result}" STREQUAL "1" OR NOT error MATCHES "Cach dung")
        message(FATAL_ERROR "Tham so '${args}' phai bi tu choi: ${result} ${error}")
    endif ()
endforeach ()

file(REMOVE "${data}" "${other}" "${csv}" "${input}")
message(STATUS "Input validation passed")
