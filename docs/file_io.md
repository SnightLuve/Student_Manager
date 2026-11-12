# Module TV3: lưu trữ dữ liệu

Module thực hiện thiết kế tại README mục 5.2 và 7. Không thay đổi `Student`,
`Node`, `FILE_MAGIC` hoặc `FILE_VERSION`. Các hàm được khai báo trong
`include/file_io.h`, cài đặt trong `src/file_io.c`.

## Giao diện bàn giao cho TV4

| Hàm | Kết quả |
|---|---|
| `saveToFile(path, head)` | 1: thành công; 0: thất bại |
| `loadFromFile(path, &head)` | Số SV đã thêm; 0: file chưa tồn tại/file có 0 bản ghi; -1: lỗi |
| `readRecordAt(path, index, &student)` | 1: thành công; 0: lỗi hoặc chỉ số ngoài phạm vi |
| `exportCsv(path, head)` | 1: thành công; 0: thất bại |

`loadFromFile` **thêm vào cuối** danh sách hiện có; thường gọi với `head = NULL`
khi khởi động. Nếu lỗi, toàn bộ danh sách cũ được giữ nguyên, không nạp một phần.
Mã trùng trong file hoặc trùng với danh sách hiện có là lỗi, không âm thầm bỏ qua.
Mã đọc vào được chuẩn hóa in hoa, ĐTB được tính lại từ điểm môn.

Các chuỗi phải có ký tự kết thúc trong giới hạn, không rỗng/toàn khoảng trắng;
mã không có khoảng trắng, điểm hữu hạn và nằm trong [0, 10]. File sai magic,
version, count, kích thước, dữ liệu hoặc thiếu bộ nhớ đều bị từ chối.
Không tồn tại file được xem là lần chạy đầu; lỗi mở file khác trả -1.

## Định dạng và an toàn ghi file

Nhị phân version 1: `FileHeader { unsigned int magic; int version; int count; }`,
sau đó `count` bản ghi `Student`. Không ghi `Node` hoặc con trỏ `next`.
Kích thước/padding/thứ tự byte phụ thuộc compiler và nền tảng; khi đổi cấu trúc
phải thống nhất với nhóm, tăng version và tạo lại file mẫu bằng compiler đích.

Chỉ số `readRecordAt` bắt đầu từ **0**. Vị trí bản ghi là
`sizeof(FileHeader) + index * sizeof(Student)`. Hàm kiểm tra header/kích thước,
rồi dùng `fseek` để đọc đúng một bản ghi; không dựng danh sách. Hàm chỉ kiểm tra
dữ liệu bản ghi được chọn, còn `loadFromFile` kiểm tra toàn bộ dữ liệu/mã trùng.

Lưu/xuất ghi vào `<path>.tmp`, kiểm tra ghi và đóng thành công rồi mới thay file
đích. Windows dùng `MoveFileExA`, nền tảng khác dùng `rename`; không xóa file cũ
trước khi thay. File tạm được mở độc quyền để không ghi đè file tạm đã tồn tại.
Nếu còn file tạm từ lần chạy bị ngắt, kiểm tra và xử lý nó trước khi lưu lại.
Các hàm không dùng đồng thời trên cùng một đường dẫn. Thư mục cha phải tồn tại.
Cơ chế này bảo vệ file cũ trước lỗi ghi thông thường, không cam kết phục hồi khi
mất điện. Đường dẫn Windows hiện dùng API ANSI; dùng đường dẫn tương thích code
page của máy.

CSV có header theo tên 5 môn hiện tại, dấu phẩy phân cột, dấu chấm thập phân,
2 chữ số lẻ và CRLF. Chuỗi có dấu phẩy/ngoặc kép/xuống dòng được escape.
Dữ liệu mẫu dùng tên không dấu như quy ước của dự án. Không có chức năng nhập CSV.

## Build, kiểm thử và tạo lại dữ liệu mẫu

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
cmake --build build --target create_sample_data
```

Chạy `build/create_sample_data` (Windows: `.exe`; cấu hình nhiều loại build có
thể nằm trong `build/Debug/`) từ thư mục gốc dự án để tạo lại `data/students.dat`
và `data/report.csv`. Công cụ **ghi đè file đích**, chỉ chạy khi muốn đặt lại dữ
liệu mẫu. Có thể truyền đường dẫn khác:

```sh
build/create_sample_data data/demo.dat data/demo.csv
```

File mẫu gồm 18 SV, 3 lớp, mỗi mức xếp loại có 3 SV. CTest kiểm tra module và
đọc trực tiếp file mẫu được nộp kèm. Nếu dùng compiler/nền tảng khác, tạo lại
file mẫu trước khi chạy kiểm thử mẫu.

Kiểm thử gồm: lưu–đọc khớp, sửa/xóa rồi lưu và đọc lại, đọc bản ghi đầu/cuối,
chỉ số sai, file rỗng hợp lệ, thiếu file, header sai, file bị cắt/byte dư,
chuỗi không kết thúc, điểm sai/NaN/vô cực, mã trùng, chuẩn hóa và tính lại ĐTB,
giữ nguyên danh sách khi đọc lỗi, CSV escape, lỗi ghi và file tạm tồn tại.

## Tích hợp menu hiện tại

`main.c` đã nối module file I/O vào menu hiện tại theo system design.
TV4 có thể tách nhập liệu/menu thành các module riêng sau. Luồng đang chạy:

1. Nhận `-f <path>` hoặc dùng `DEFAULT_DATA_FILE`. Đường dẫn tương đối tính từ
   working directory: khi chạy trong CLion, đặt Working directory về thư mục
   gốc dự án để dùng `data/students.dat` mẫu (hoặc truyền đường dẫn tuyệt đối).
2. Khởi tạo `head = NULL`, gọi `loadFromFile` và xử lý mã lỗi.
3. Mục **8** import từ `data/sample_students.dat` trong source, thêm các mã
   chưa có, giữ nguyên sinh viên đã tồn tại rồi tự lưu vào file dữ liệu đang dùng.
   File mẫu được đọc, không bị ghi đè bởi thao tác thêm/sửa/xóa. CMake cấu hình
   đường dẫn source tự động để chạy từ CLion/build vẫn tìm thấy file mẫu;
   khi chuyển source sang máy khác cần build lại. Biên dịch GCC trực tiếp
   dùng đường dẫn tương đối `data/sample_students.dat` từ thư mục đang chạy.
   **9** xuất CSV từ RAM, cho nhập đường dẫn (mặc định
   `report.csv`); **10** xem bản ghi. Nhập STT từ 1, chương trình truyền
   `STT - 1` cho `readRecordAt`. Bản ghi này thuộc **file đã lưu**, không phải
   danh sách đang sửa trong RAM; menu có thông báo điều này.
4. Mỗi lần thêm/xóa hoặc sửa họ tên/lớp/điểm thành công đều gọi lưu ngay,
   kể cả khi còn ở menu con sửa thông tin. Nếu ghi thất bại, thông báo rõ
   thay đổi mới chỉ còn trong RAM, cần khắc phục lỗi; chương trình thử lưu lại
   khi có thay đổi tiếp theo hoặc khi thoát. Không còn mục lưu thủ công.
5. Mục **0** (Thoát) vẫn lưu lần cuối rồi thoát. Nếu lưu thất bại, cho quay lại menu để thử lại
   hoặc xác nhận thoát không lưu. Nếu hết luồng nhập mà lưu thất bại,
   chương trình kết thúc với mã lỗi. Nếu file đầu vào hỏng, báo lỗi và kết
   thúc ngay, giữ nguyên file để người dùng kiểm tra hoặc dùng `-f` chọn file khác.

Thêm/sửa/xóa cập nhật DSLK trong RAM rồi tự gọi `saveToFile` ghi lại danh sách
và count ngay. Khi thấy thông báo lưu thành công, dữ liệu mới đã có trong file,
không cần đợi thoát. Nếu ghi thất bại, file cũ được giữ nguyên, danh sách RAM
vẫn giữ thay đổi để thử lưu lại. Tắt cưỡng bức trong lúc đang nhập hoặc đang ghi
không đảm bảo lưu được thao tác chưa hoàn tất.

CTest `menu_file_io` chạy menu thực với bản sao dữ liệu mẫu: xóa/lưu, xuất CSV,
xem bản ghi, sửa điểm, mở lại kiểm tra dữ liệu, file hỏng, lỗi lưu và hết luồng nhập.
Kiểm thử tự lưu đọc bản ghi trực tiếp từ file ngay sau thêm/sửa/xóa, trước khi
thoát; đồng thời kiểm tra cảnh báo khi tự lưu thất bại. Kiểm thử import vào danh
sách rỗng, import lặp, mã trùng giữ nguyên dữ liệu đã sửa và file mẫu không đổi.

Khi chạy với đường dẫn dữ liệu mặc định, chương trình tạo thư mục `data` nếu
chưa có. Với `-f` hoặc đường dẫn CSV tự nhập, thư mục cha vẫn cần tồn tại.
