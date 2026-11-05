# Student Manager — Bài tập lớn Kỹ thuật lập trình (C)

> **Đề tài 1 — Quản lý sinh viên**
> Học phần: Kỹ thuật lập trình (Ngôn ngữ C) — Khoa CNTT, Trường ĐH Giao thông Vận tải
>
> Chương trình console quản lý sinh viên: **mã SV, họ tên, lớp, điểm các môn**; hỗ trợ
> thêm / xóa / sửa / tìm kiếm / sắp xếp / thống kê, tính điểm trung bình, xếp loại học lực
> và **lưu – đọc dữ liệu bằng file nhị phân** giữa các lần chạy.

Tài liệu này hướng dẫn nhóm xây dựng dự án từ đầu đến khi nộp bài: thiết kế dữ liệu, chia module,
cách cài đặt từng chức năng, cách đáp ứng **từng yêu cầu bắt buộc** trong đề và cách kiểm thử.

---

## Mục lục

1. [Đối chiếu yêu cầu đề bài](#1-đối-chiếu-yêu-cầu-đề-bài)
2. [Danh sách chức năng](#2-danh-sách-chức-năng)
3. [Cấu trúc thư mục dự án](#3-cấu-trúc-thư-mục-dự-án)
4. [Thiết kế dữ liệu](#4-thiết-kế-dữ-liệu)
5. [Thiết kế các module và hàm](#5-thiết-kế-các-module-và-hàm)
6. [Hướng dẫn cài đặt chi tiết từng module](#6-hướng-dẫn-cài-đặt-chi-tiết-từng-module)
7. [Định dạng file lưu trữ](#7-định-dạng-file-lưu-trữ)
8. [Biên dịch và chạy chương trình](#8-biên-dịch-và-chạy-chương-trình)
9. [Phân công nhiệm vụ](#9-phân-công-nhiệm-vụ)
10. [Lộ trình thực hiện theo buổi](#10-lộ-trình-thực-hiện-theo-buổi)
11. [Kịch bản kiểm thử](#11-kịch-bản-kiểm-thử)
12. [Hướng dẫn viết báo cáo và chuẩn bị vấn đáp](#12-hướng-dẫn-viết-báo-cáo-và-chuẩn-bị-vấn-đáp)
13. [Checklist trước khi nộp](#13-checklist-trước-khi-nộp)

---

## 1. Đối chiếu yêu cầu đề bài

| Chương | Yêu cầu trong đề | Cách đáp ứng trong dự án | Vị trí |
|---|---|---|---|
| **Ch.1** – Kiểu dữ liệu, mảng, cấu trúc điều khiển | Dùng mảng / cấu trúc điều khiển trong ít nhất một chức năng xử lý danh sách | Mảng `scores[MAX_SUBJECTS]` lưu điểm các môn; mảng đếm `rankCount[RANK_COUNT]` khi thống kê xếp loại; `switch-case` cho menu; `for/while` duyệt danh sách | `student.h`, `stats.c`, `menu.c` |
| **Ch.2** – Hàm, con trỏ, con trỏ hàm, đệ quy, đối dòng lệnh | Nhiều hàm rõ chức năng; có tham số con trỏ; ≥ 1 hàm đệ quy | ~35 hàm trong 7 module; tham số `Node **head`, `Student *s`; **4 hàm đệ quy có ý nghĩa**; **con trỏ hàm** `CompareFn` để sắp xếp nhiều tiêu chí; `argv` nhận tên file dữ liệu | toàn bộ `src/` |
| **Ch.3** – Cấp phát động, struct, danh sách liên kết | struct + DSLK làm cấu trúc lưu trữ chính; đủ thêm/xóa/sửa/tìm/sắp xếp | `struct Student` + **danh sách liên kết đơn** `Node`; `malloc`/`free`; không dùng mảng tĩnh chứa danh sách sinh viên | `list.c` |
| **Ch.4** – Tệp nhị phân, văn bản, truy cập ngẫu nhiên | Lưu toàn bộ dữ liệu khi thoát, đọc lại khi khởi động | File nhị phân `fwrite/fread` với struct, có header kiểm tra; `fseek` đọc bản ghi thứ *i* (truy cập ngẫu nhiên); xuất báo cáo ra file văn bản `.csv` | `file_io.c` |
| **Ch.5** *(khuyến khích)* – Tiền xử lý, đối dòng lệnh | Cộng điểm nếu vận dụng | `#define` hằng số, include guard `#ifndef`, `#ifdef DEBUG` bật log, `#ifdef _WIN32` xử lý console tiếng Việt; tham số dòng lệnh `-f <file>` | `config.h`, `main.c` |
| Mục 3.4 – Giao diện | Menu lặp tới khi thoát; kiểm tra dữ liệu nhập | Menu `do-while`; module `input.c` kiểm tra rỗng, số nguyên, số thực trong khoảng, trùng mã | `menu.c`, `input.c` |

---

## 2. Danh sách chức năng

### 2.1. Chức năng bắt buộc (theo đề tài 1)

| # | Chức năng | Mô tả |
|---|---|---|
| F1 | Thêm sinh viên | Nhập mã, họ tên, lớp, điểm các môn; kiểm tra mã không trùng, điểm trong [0, 10] |
| F2 | Hiển thị danh sách | In bảng: STT, mã, họ tên, lớp, điểm từng môn, ĐTB, xếp loại |
| F3 | Tìm theo mã SV | Tìm **đệ quy** trên DSLK, in thông tin chi tiết |
| F4 | Sửa thông tin | Chọn trường cần sửa (họ tên / lớp / điểm từng môn); tự tính lại ĐTB |
| F5 | Xóa sinh viên | Xóa theo mã, có xác nhận (y/n), giải phóng bộ nhớ |
| F6 | Tính điểm trung bình | Tự động khi thêm/sửa; trung bình của các môn (có thể có hệ số tín chỉ) |
| F7 | Xếp loại học lực | Xuất sắc / Giỏi / Khá / Trung bình / Yếu / Kém |
| F8 | Lưu / đọc file | Tự đọc khi khởi động, tự lưu khi thoát, có thể lưu thủ công |

### 2.2. Chức năng mở rộng (được cộng điểm)

| # | Chức năng | Mô tả |
|---|---|---|
| E1 | Tìm theo tên / theo lớp | Tìm gần đúng (chứa chuỗi con, không phân biệt hoa thường) |
| E2 | Lọc theo khoảng điểm | Liệt kê SV có ĐTB trong `[a, b]` |
| E3 | Sắp xếp nhiều tiêu chí | Theo mã, theo tên, theo ĐTB giảm dần, theo **lớp rồi ĐTB** — dùng Merge Sort **đệ quy** + **con trỏ hàm** |
| E4 | Thống kê | Số SV theo xếp loại; ĐTB toàn khóa; SV điểm cao nhất/thấp nhất; **thống kê theo lớp** |
| E5 | In danh sách theo thứ tự ngược | Hàm **đệ quy** in từ cuối danh sách lên |
| E6 | Xuất báo cáo CSV | Ghi file văn bản mở được bằng Excel |
| E7 | Xem bản ghi thứ *i* trong file | Đọc trực tiếp bằng `fseek` (truy cập ngẫu nhiên) |

---

## 3. Cấu trúc thư mục dự án

```
Student_Manager/
├── CMakeLists.txt
├── README.md
├── include/               # Khai báo (header)
│   ├── config.h           # Hằng số, macro, cấu hình tiền xử lý
│   ├── student.h          # struct Student + hàm xử lý 1 sinh viên
│   ├── list.h             # struct Node + thao tác danh sách liên kết
│   ├── search_sort.h      # Tìm kiếm, sắp xếp, con trỏ hàm so sánh
│   ├── stats.h            # Thống kê
│   ├── file_io.h          # Đọc/ghi file
│   ├── input.h            # Nhập liệu an toàn + kiểm tra hợp lệ
│   └── menu.h             # Giao diện menu
├── src/                   # Cài đặt
│   ├── main.c
│   ├── student.c
│   ├── list.c
│   ├── search_sort.c
│   ├── stats.c
│   ├── file_io.c
│   ├── input.c
│   └── menu.c
└── data/
    └── students.dat       # File dữ liệu mẫu (nhị phân) — nộp kèm
```

> Hiện tại dự án mới có `main.c` ở thư mục gốc. Chuyển nó vào `src/` và cập nhật
> `CMakeLists.txt` như ở [mục 8](#8-biên-dịch-và-chạy-chương-trình).

**Quy tắc chung cho cả nhóm:**

- Mỗi file `.h` có include guard (`#ifndef ... #define ... #endif`).
- Tên hàm dạng `camelCase`, tên kiểu dạng `PascalCase`, hằng số dạng `UPPER_CASE`.
- Mỗi hàm có chú thích ngắn: mục đích, tham số, giá trị trả về.
- `main()` chỉ khởi tạo – gọi menu – dọn dẹp, **không chứa logic xử lý**.
- Chỉ dùng `fgets` để nhập (không trộn `scanf` và `fgets` → tránh lỗi trôi lệnh).

---

## 4. Thiết kế dữ liệu

### 4.1. `config.h` — hằng số và cấu hình

```c
#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ID        12   /* Mã SV tối đa 11 ký tự + '\0'          */
#define MAX_NAME      51
#define MAX_CLASS     21
#define MAX_SUBJECTS  5    /* Số môn học cố định                     */
#define MAX_LINE      256  /* Bộ đệm nhập một dòng                   */

#define MIN_SCORE     0.0f
#define MAX_SCORE     10.0f

#define DEFAULT_DATA_FILE "data/students.dat"
#define FILE_MAGIC        0x53544D47u  /* "STMG" — nhận dạng file   */
#define FILE_VERSION      1

/* Bật bằng: cmake -DCMAKE_C_FLAGS="-DDEBUG" hoặc gcc -DDEBUG */
#ifdef DEBUG
  #define LOG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
  #define LOG(fmt, ...) ((void)0)
#endif

#endif /* CONFIG_H */
```

### 4.2. `struct Student` — dữ liệu một sinh viên

```c
typedef struct {
    char  id[MAX_ID];               /* Mã SV — khóa, duy nhất       */
    char  name[MAX_NAME];           /* Họ tên                       */
    char  className[MAX_CLASS];     /* Lớp                          */
    float scores[MAX_SUBJECTS];     /* Điểm các môn (mảng — Ch.1)   */
    float avg;                      /* Điểm trung bình (tự tính)    */
} Student;

/* Tên môn học — dùng chung toàn chương trình (khai báo extern trong .h) */
extern const char *SUBJECT_NAMES[MAX_SUBJECTS];
/* student.c:
const char *SUBJECT_NAMES[MAX_SUBJECTS] = {
    "Toan cao cap", "Vat ly", "Lap trinh C", "Tieng Anh", "Triet hoc"
}; */
```

> **Quan trọng:** `Student` **không chứa con trỏ** → có thể `fwrite`/`fread` nguyên struct
> vào file nhị phân một cách an toàn. Con trỏ `next` được tách sang `Node`.

### 4.3. `struct Node` — nút danh sách liên kết đơn

```c
typedef struct Node {
    Student      data;
    struct Node *next;
} Node;
```

```
head
 │
 ▼
┌─────────────┬──────┐   ┌─────────────┬──────┐   ┌─────────────┬──────┐
│ SV001 ...   │ next ├──►│ SV002 ...   │ next ├──►│ SV003 ...   │ NULL │
└─────────────┴──────┘   └─────────────┴──────┘   └─────────────┴──────┘
```

### 4.4. Quy tắc xếp loại học lực (thang 10)

| ĐTB | Xếp loại |
|---|---|
| 9.0 – 10 | Xuất sắc |
| 8.0 – < 9.0 | Giỏi |
| 6.5 – < 8.0 | Khá |
| 5.0 – < 6.5 | Trung bình |
| 3.5 – < 5.0 | Yếu |
| < 3.5 | Kém |

Dùng kiểu liệt kê để vừa in tên, vừa làm chỉ số mảng đếm khi thống kê:

```c
typedef enum { RANK_EXCELLENT, RANK_GOOD, RANK_FAIR,
               RANK_AVERAGE, RANK_WEAK, RANK_POOR, RANK_COUNT } Rank;
```

---

## 5. Thiết kế các module và hàm

### 5.1. Sơ đồ chức năng

```
                         ┌──────────────┐
                         │    main()    │  đọc argv → loadFromFile → runMenu → saveToFile → freeList
                         └──────┬───────┘
                                │
                         ┌──────▼───────┐
                         │   menu.c     │  vòng lặp menu, gọi các module
                         └──────┬───────┘
     ┌─────────────┬────────────┼──────────────┬──────────────┬─────────────┐
┌────▼────┐  ┌─────▼─────┐ ┌────▼─────┐  ┌─────▼──────┐ ┌─────▼─────┐ ┌─────▼─────┐
│ input.c │  │ student.c │ │ list.c   │  │search_sort │ │ stats.c   │ │ file_io.c │
│ nhập &  │  │ ĐTB, xếp  │ │ thêm/xóa │  │ tìm, sắp   │ │ thống kê  │ │ đọc/ghi   │
│ kiểm tra│  │ loại, in  │ │ /sửa/giải│  │ xếp (đệ    │ │ (đệ quy)  │ │ nhị phân, │
│         │  │           │ │ phóng    │  │ quy, con   │ │           │ │ CSV,fseek │
│         │  │           │ │          │  │ trỏ hàm)   │ │           │ │           │
└─────────┘  └───────────┘ └──────────┘  └────────────┘ └───────────┘ └───────────┘
```

### 5.2. Danh sách hàm

| Module | Hàm | Ghi chú |
|---|---|---|
| **student** | `void computeAverage(Student *s)` | tham số con trỏ |
| | `Rank classify(float avg)` | |
| | `const char *rankName(Rank r)` | |
| | `void printTableHeader(void)` / `void printStudentRow(int stt, const Student *s)` | |
| | `void printStudentDetail(const Student *s)` | |
| **list** | `Node *createNode(const Student *s)` | `malloc` |
| | `int addStudent(Node **head, const Student *s)` | thêm cuối, chặn trùng mã |
| | `int deleteStudent(Node **head, const char *id)` | `free` |
| | `int countNodes(const Node *head)` | **đệ quy** |
| | `void printList(const Node *head)` | |
| | `void printListReverse(const Node *head)` | **đệ quy** |
| | `void freeList(Node **head)` | |
| **search_sort** | `Node *findById(Node *head, const char *id)` | **đệ quy** |
| | `int searchByName(const Node *head, const char *keyword)` | không phân biệt hoa/thường |
| | `int searchByClass(...)`, `int filterByAvgRange(...)` | |
| | `int cmpById / cmpByName / cmpByAvgDesc / cmpByClassThenAvg(const Student*, const Student*)` | dùng làm **con trỏ hàm** |
| | `void mergeSort(Node **head, CompareFn cmp)` | **đệ quy** |
| **stats** | `float sumAvg(const Node *head)` | **đệ quy** |
| | `void countByRank(const Node *head, int rankCount[])` | mảng đếm |
| | `void statsByClass(const Node *head)` | thống kê theo nhóm |
| | `void showTopAndBottom(const Node *head)` | |
| **file_io** | `int saveToFile(const char *path, const Node *head)` | `fwrite` |
| | `int loadFromFile(const char *path, Node **head)` | `fread` |
| | `int readRecordAt(const char *path, int index, Student *out)` | `fseek` |
| | `int exportCsv(const char *path, const Node *head)` | file văn bản |
| **input** | `void readLine(const char *prompt, char *buf, size_t size)` | |
| | `void readNonEmpty(const char *prompt, char *buf, size_t size)` | |
| | `int readInt(const char *prompt, int min, int max)` | |
| | `float readFloat(const char *prompt, float min, float max)` | |
| | `int confirm(const char *prompt)` | y/n |
| **menu** | `void runMenu(Node **head, const char *dataFile)` + mỗi mục menu một hàm `handleAdd`, `handleDelete`, ... | |

---

## 6. Hướng dẫn cài đặt chi tiết từng module

> Các đoạn mã dưới đây là **khung tham khảo** cho phần cốt lõi. Nhóm cần tự hoàn thiện,
> đặt tên, chú thích theo cách hiểu của mình và **giải thích được từng dòng** khi vấn đáp.

### 6.1. `input.c` — nhập liệu an toàn

Nguyên tắc: luôn đọc **cả dòng** bằng `fgets`, rồi mới chuyển đổi bằng `strtol` / `strtof`,
kiểm tra con trỏ `end` để phát hiện ký tự thừa (ví dụ nhập `7.5abc`).

```c
/* Đọc một dòng, bỏ '\n'; nếu người dùng nhập quá dài thì xả phần thừa. */
void readLine(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (fgets(buf, (int)size, stdin) == NULL) { buf[0] = '\0'; return; }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {                       /* dòng dài hơn bộ đệm → xả stdin */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    trim(buf);                     /* tự viết: xóa khoảng trắng đầu/cuối */
}

void readNonEmpty(const char *prompt, char *buf, size_t size) {
    do {
        readLine(prompt, buf, size);
        if (buf[0] == '\0') printf("  ! Khong duoc de trong.\n");
    } while (buf[0] == '\0');
}

float readFloat(const char *prompt, float min, float max) {
    char line[MAX_LINE];
    char *end;
    for (;;) {
        readLine(prompt, line, sizeof line);
        float v = strtof(line, &end);
        if (end != line && *end == '\0' && v >= min && v <= max) return v;
        printf("  ! Vui long nhap so trong khoang [%.1f, %.1f].\n", min, max);
    }
}
```

`readInt` cài đặt tương tự với `strtol`. Ngoài ra nên kiểm tra:

- **Mã SV**: không rỗng, không chứa dấu cách, chuyển về chữ in hoa (`toupper`) để `sv001` và `SV001` là một; **không trùng** mã đã có (gọi `findById`).
- **Họ tên**: không rỗng, không chứa chữ số.
- **Điểm**: số thực trong `[0, 10]`.

### 6.2. `student.c` — tính ĐTB, xếp loại

```c
/* Truyền con trỏ để hàm cập nhật trực tiếp trường avg của sinh viên. */
void computeAverage(Student *s) {
    float sum = 0.0f;
    for (int i = 0; i < MAX_SUBJECTS; i++)     /* duyệt mảng điểm — Ch.1 */
        sum += s->scores[i];
    s->avg = sum / MAX_SUBJECTS;
}

Rank classify(float avg) {
    if (avg >= 9.0f) return RANK_EXCELLENT;
    if (avg >= 8.0f) return RANK_GOOD;
    if (avg >= 6.5f) return RANK_FAIR;
    if (avg >= 5.0f) return RANK_AVERAGE;
    if (avg >= 3.5f) return RANK_WEAK;
    return RANK_POOR;
}
```

> Mở rộng: thêm mảng `CREDITS[MAX_SUBJECTS]` (số tín chỉ) và tính **ĐTB có trọng số**
> `Σ(điểm × tín chỉ) / Σ tín chỉ`.

### 6.3. `list.c` — danh sách liên kết đơn

**Thêm vào cuối** (truyền `Node **head` vì có thể phải thay đổi chính con trỏ `head`):

```c
int addStudent(Node **head, const Student *s) {
    if (findById(*head, s->id) != NULL) return 0;      /* trùng mã */

    Node *node = createNode(s);                        /* malloc + copy data */
    if (node == NULL) return -1;                       /* hết bộ nhớ */

    if (*head == NULL) { *head = node; return 1; }
    Node *p = *head;
    while (p->next != NULL) p = p->next;
    p->next = node;
    return 1;
}
```

**Xóa theo mã:**

```c
int deleteStudent(Node **head, const char *id) {
    Node *cur = *head, *prev = NULL;
    while (cur != NULL && strcmp(cur->data.id, id) != 0) {
        prev = cur;
        cur  = cur->next;
    }
    if (cur == NULL) return 0;              /* không tìm thấy */
    if (prev == NULL) *head = cur->next;    /* xóa nút đầu   */
    else              prev->next = cur->next;
    free(cur);
    return 1;
}
```

**Sửa:** dùng `findById` lấy con trỏ tới nút, rồi sửa trực tiếp `node->data` qua con trỏ
(menu con: 1-Họ tên, 2-Lớp, 3-Điểm một môn, 4-Toàn bộ điểm, 0-Xong). Sau khi sửa điểm
**phải gọi lại** `computeAverage(&node->data)`. Không cho sửa mã SV (là khóa).

**Giải phóng toàn bộ** khi thoát chương trình:

```c
void freeList(Node **head) {
    while (*head != NULL) {
        Node *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
}
```

### 6.4. Các hàm đệ quy (có ý nghĩa thực tế)

Đề yêu cầu tối thiểu 1 hàm đệ quy *có mục đích*. Dự án dùng 4 hàm; mỗi hàm cần nêu được
**điều kiện dừng** và **bước đệ quy** khi vấn đáp.

**① Tìm sinh viên theo mã** — dừng khi hết danh sách hoặc tìm thấy:

```c
Node *findById(Node *head, const char *id) {
    if (head == NULL) return NULL;                       /* dừng: hết DS  */
    if (strcmp(head->data.id, id) == 0) return head;     /* dừng: thấy    */
    return findById(head->next, id);                     /* đệ quy        */
}
```

**② In danh sách theo thứ tự ngược** (DSLK đơn không đi lùi được — đệ quy giải quyết điều này):

```c
void printListReverse(const Node *head) {
    if (head == NULL) return;
    printListReverse(head->next);      /* in phần sau trước */
    printStudentRow(0, &head->data);   /* rồi mới in nút hiện tại */
}
```

**③ Tổng ĐTB và đếm số SV** → tính ĐTB của cả khóa/lớp:

```c
float sumAvg(const Node *head) {
    if (head == NULL) return 0.0f;
    return head->data.avg + sumAvg(head->next);
}

int countNodes(const Node *head) {
    return head == NULL ? 0 : 1 + countNodes(head->next);
}
/* ĐTB toàn khóa = sumAvg(head) / countNodes(head)  (kiểm tra count > 0) */
```

**④ Merge Sort trên DSLK** — xem mục 6.5.

> Lưu ý: đệ quy trên DSLK có độ sâu = số phần tử. Với vài nghìn sinh viên vẫn an toàn;
> có thể nêu điểm này ở phần "Hạn chế" trong báo cáo.

### 6.5. `search_sort.c` — sắp xếp đa tiêu chí bằng con trỏ hàm

Khai báo kiểu con trỏ hàm so sánh:

```c
typedef int (*CompareFn)(const Student *a, const Student *b);

int cmpById(const Student *a, const Student *b)      { return strcmp(a->id, b->id); }
int cmpByAvgDesc(const Student *a, const Student *b) {
    if (a->avg < b->avg) return 1;
    if (a->avg > b->avg) return -1;
    return 0;
}
/* Nhiều tiêu chí: theo lớp, cùng lớp thì ĐTB giảm dần */
int cmpByClassThenAvg(const Student *a, const Student *b) {
    int c = strcmp(a->className, b->className);
    return c != 0 ? c : cmpByAvgDesc(a, b);
}
```

> `cmpByName`: nên so sánh theo **tên** (từ cuối của họ tên) trước, rồi mới tới họ đệm —
> đúng thói quen sắp xếp tên tiếng Việt. Dùng `strrchr(name, ' ')` để lấy tên.

Merge Sort đệ quy trên DSLK (O(n log n), không cần mảng phụ):

```c
static void splitList(Node *src, Node **front, Node **back) {
    Node *slow = src, *fast = src->next;
    while (fast != NULL && fast->next != NULL) {   /* rùa và thỏ */
        slow = slow->next;
        fast = fast->next->next;
    }
    *front = src;
    *back  = slow->next;
    slow->next = NULL;
}

static Node *mergeLists(Node *a, Node *b, CompareFn cmp) {
    if (a == NULL) return b;
    if (b == NULL) return a;
    if (cmp(&a->data, &b->data) <= 0) {
        a->next = mergeLists(a->next, b, cmp);
        return a;
    }
    b->next = mergeLists(a, b->next, cmp);
    return b;
}

void mergeSort(Node **head, CompareFn cmp) {
    if (*head == NULL || (*head)->next == NULL) return;   /* dừng */
    Node *a, *b;
    splitList(*head, &a, &b);
    mergeSort(&a, cmp);
    mergeSort(&b, cmp);
    *head = mergeLists(a, b, cmp);
}
```

Trong menu, chọn tiêu chí bằng **mảng con trỏ hàm**:

```c
CompareFn criteria[] = { cmpById, cmpByName, cmpByAvgDesc, cmpByClassThenAvg };
int choice = readInt("Chon tieu chi (1-4): ", 1, 4);
mergeSort(head, criteria[choice - 1]);
```

### 6.6. `stats.c` — thống kê

- **Theo xếp loại**: duyệt danh sách, `rankCount[classify(p->data.avg)]++`; in số lượng và tỉ lệ %.
- **ĐTB toàn khóa**: `sumAvg / countNodes` (đệ quy).
- **Cao nhất / thấp nhất**: một lần duyệt, giữ hai con trỏ `best`, `worst`.
- **Theo lớp (thống kê theo nhóm)**: sắp xếp bản sao theo lớp, hoặc duyệt và với mỗi lớp
  chưa xử lý thì đếm số SV + ĐTB của lớp đó; in bảng `Lớp | Sĩ số | ĐTB lớp | Số SV Giỏi trở lên`.
- **Điểm TB từng môn**: mảng `float subjectSum[MAX_SUBJECTS]` cộng dồn qua danh sách.

### 6.7. `menu.c` — giao diện

```
============================================
        CHUONG TRINH QUAN LY SINH VIEN
============================================
  1. Them sinh vien
  2. Hien thi danh sach sinh vien
  3. Tim kiem sinh vien (theo ma / ten / lop)
  4. Sua thong tin sinh vien
  5. Xoa sinh vien
  6. Sap xep danh sach
  7. Loc sinh vien theo khoang diem TB
  8. Thong ke & xep loai hoc luc
  9. In danh sach theo thu tu nguoc
 10. Luu du lieu / Xuat file CSV
 11. Xem ban ghi thu i trong file (truy cap ngau nhien)
  0. Thoat (tu dong luu)
--------------------------------------------
Lua chon cua ban:
```

```c
void runMenu(Node **head, const char *dataFile) {
    int choice;
    do {
        printMainMenu();
        choice = readInt("Lua chon cua ban: ", 0, 11);
        switch (choice) {
            case 1: handleAdd(head);              break;
            case 2: printList(*head);             break;
            case 3: handleSearch(*head);          break;
            case 4: handleEdit(*head);            break;
            case 5: handleDelete(head);           break;
            case 6: handleSort(head);             break;
            /* ... */
            case 0: printf("Dang luu du lieu...\n"); break;
        }
    } while (choice != 0);
}
```

Gợi ý trải nghiệm: hiển thị "Danh sach rong" khi chưa có dữ liệu; sau mỗi thao tác
`Nhan Enter de tiep tuc...`; xác nhận trước khi xóa; báo số kết quả tìm được.

### 6.8. `main.c` — điểm vào chương trình

```c
int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);          /* hiển thị UTF-8 trên console Windows */
#endif
    const char *dataFile = DEFAULT_DATA_FILE;
    if (argc == 3 && strcmp(argv[1], "-f") == 0)   /* đối dòng lệnh — Ch.2/Ch.5 */
        dataFile = argv[2];

    Node *head = NULL;
    int n = loadFromFile(dataFile, &head);
    printf("Da doc %d sinh vien tu '%s'.\n", n < 0 ? 0 : n, dataFile);

    runMenu(&head, dataFile);

    if (!saveToFile(dataFile, head))
        fprintf(stderr, "Loi: khong the luu file '%s'!\n", dataFile);
    freeList(&head);
    return 0;
}
```

> Console Windows nhập tiếng Việt có dấu qua `fgets` thường bị lỗi mã hóa. Để đơn giản và
> ổn định, nhóm nên quy ước **nhập không dấu** (ví dụ `Nguyen Van An`) và ghi rõ trong báo cáo.

---

## 7. Định dạng file lưu trữ

### 7.1. File nhị phân `students.dat` (file chính)

```
┌──────────────────────────────┬───────────────┬───────────────┬─────┬───────────────┐
│ FileHeader                   │ Student #0    │ Student #1    │ ... │ Student #n-1  │
│ magic | version | count      │ sizeof(Student) bytes mỗi bản ghi                    │
└──────────────────────────────┴───────────────┴───────────────┴─────┴───────────────┘
```

```c
typedef struct {
    unsigned int magic;     /* FILE_MAGIC — nhận đúng file của chương trình */
    int          version;   /* FILE_VERSION — đổi khi thay đổi struct       */
    int          count;     /* số bản ghi theo sau                          */
} FileHeader;
```

**Ghi file** (gọi khi thoát và khi người dùng chọn "Lưu"):

```c
int saveToFile(const char *path, const Node *head) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) return 0;

    FileHeader h = { FILE_MAGIC, FILE_VERSION, countNodes(head) };
    fwrite(&h, sizeof h, 1, f);

    for (const Node *p = head; p != NULL; p = p->next)
        if (fwrite(&p->data, sizeof(Student), 1, f) != 1) { fclose(f); return 0; }

    fclose(f);
    return 1;
}
```

**Đọc file** (gọi khi khởi động):

```c
/* Trả về số bản ghi đã đọc, 0 nếu file chưa tồn tại (lần chạy đầu), -1 nếu file lỗi. */
int loadFromFile(const char *path, Node **head) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) return 0;

    FileHeader h;
    if (fread(&h, sizeof h, 1, f) != 1 || h.magic != FILE_MAGIC
        || h.version != FILE_VERSION || h.count < 0) {
        fclose(f);
        return -1;
    }
    Student s;
    int loaded = 0;
    for (int i = 0; i < h.count && fread(&s, sizeof s, 1, f) == 1; i++)
        if (addStudent(head, &s) == 1) loaded++;
    fclose(f);
    return loaded;
}
```

**Truy cập ngẫu nhiên** — đọc thẳng bản ghi thứ `index` mà không đọc cả file:

```c
int readRecordAt(const char *path, int index, Student *out) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) return 0;
    long offset = (long)sizeof(FileHeader) + (long)index * (long)sizeof(Student);
    int ok = fseek(f, offset, SEEK_SET) == 0 && fread(out, sizeof *out, 1, f) == 1;
    fclose(f);
    return ok;
}
```

> **Mẹo an toàn dữ liệu:** ghi ra `students.tmp` trước, ghi thành công mới `remove()` file cũ
> và `rename()` file tạm → nếu chương trình lỗi giữa chừng, file cũ không bị hỏng.
>
> **Lưu ý:** file nhị phân phụ thuộc trình biên dịch (padding, kích thước `int`/`float`).
> Dùng cùng một bộ biên dịch khi tạo và đọc file mẫu.

### 7.2. File văn bản `report.csv` (xuất báo cáo)

```
MaSV,HoTen,Lop,Toan cao cap,Vat ly,Lap trinh C,Tieng Anh,Triet hoc,DTB,XepLoai
SV001,Nguyen Van An,CNTT1,8.5,7.0,9.0,8.0,7.5,8.00,Gioi
```

Dùng `fprintf` với định dạng cố định; nếu muốn đọc lại từ CSV thì dùng `fgets` + `strtok`
với cùng định dạng (đề yêu cầu "xử lý định dạng đọc/ghi nhất quán").

---

## 8. Biên dịch và chạy chương trình

### 8.1. Bằng CLion / CMake

Cập nhật `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(Student_Manager C)

set(CMAKE_C_STANDARD 11)

add_executable(Student_Manager
        src/main.c
        src/student.c
        src/list.c
        src/search_sort.c
        src/stats.c
        src/file_io.c
        src/input.c
        src/menu.c)

target_include_directories(Student_Manager PRIVATE include)

if (NOT MSVC)
    target_compile_options(Student_Manager PRIVATE -Wall -Wextra)
endif ()

# Bật log debug:  cmake -DENABLE_DEBUG=ON ..
option(ENABLE_DEBUG "Bat macro DEBUG" OFF)
if (ENABLE_DEBUG)
    target_compile_definitions(Student_Manager PRIVATE DEBUG)
endif ()
```

Trong CLion: **Build → Build Project**, rồi **Run**. Lưu ý thư mục làm việc khi chạy trong
CLion mặc định là `cmake-build-debug/` → đặt *Working directory* trong
*Run → Edit Configurations* về thư mục gốc dự án để đường dẫn `data/students.dat` đúng.

### 8.2. Bằng dòng lệnh (GCC / MinGW)

```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/*.c -o student_manager
./student_manager                      # dùng file mặc định data/students.dat
./student_manager -f data/k64.dat      # chỉ định file dữ liệu khác
gcc -std=c11 -DDEBUG -Iinclude src/*.c -o student_manager_debug   # bản có log
```

---

## 9. Phân công nhiệm vụ

Theo gợi ý Mục 5 của đề (điều chỉnh theo số thành viên thực tế; nhóm 3 người thì gộp TV4, TV5
vào TV1–TV3). Điền tên/MSSV và nộp kèm báo cáo.

| Thành viên | MSSV | Module phụ trách | File | Mức hoàn thành |
|---|---|---|---|---|
| TV1 (Nhóm trưởng) | | Thiết kế `struct`, DSLK; thêm / xóa / sửa / giải phóng | `student.*`, `list.*` | % |
| TV2 | | Tìm kiếm, sắp xếp (Merge Sort, con trỏ hàm), thống kê, **các hàm đệ quy** | `search_sort.*`, `stats.*` | % |
| TV3 | | Đọc/ghi file nhị phân, CSV, truy cập ngẫu nhiên, file dữ liệu mẫu | `file_io.*`, `data/` | % |
| TV4 | | Menu, kiểm tra hợp lệ dữ liệu nhập, tích hợp module, `main.c`, CMake | `input.*`, `menu.*`, `main.c` | % |
| TV5 | | Kiểm thử tổng thể, viết báo cáo, chuẩn bị slide/demo bảo vệ | `docs/`, báo cáo | % |

**Hợp đồng giao diện:** các file `.h` (đặc biệt `config.h`, `student.h`, `list.h`) phải được
thống nhất **ngay buổi 2** — sau đó mỗi người cài đặt file `.c` của mình độc lập. Muốn đổi
`struct Student` phải báo cả nhóm (vì ảnh hưởng tới định dạng file nhị phân → tăng `FILE_VERSION`).

**Làm việc với Git:** mỗi người làm trên nhánh riêng (`feature/list`, `feature/file-io`, ...),
commit nhỏ, thường xuyên, thông điệp rõ ràng; nhóm trưởng merge vào `main` sau khi build được.
Lịch sử commit cũng là minh chứng đóng góp khi đánh giá chéo.

---

## 10. Lộ trình thực hiện theo buổi

| Buổi | Mục tiêu | Việc cụ thể | Sản phẩm |
|---|---|---|---|
| **1** | Chọn đề tài, lập nhóm | Đăng ký đề tài "Quản lý sinh viên"; tạo repo Git; đọc README này | Danh sách nhóm + đề tài |
| **2** | Thiết kế | Chốt `struct Student`, `Node`, danh sách môn, quy tắc xếp loại; viết đủ các file `.h`; lập bảng phân công | Bản thiết kế dữ liệu + bảng phân công |
| *Tự học* | | TV4 viết `input.c` + khung menu; TV1 viết `createNode`, `addStudent`, `printList` | |
| **3** | Module DSLK | Thêm / xóa / sửa / tìm theo mã chạy được trên dữ liệu nhập tay | Module cấu trúc dữ liệu chạy được |
| *Tự học* | | TV2 viết sắp xếp + thống kê; TV3 viết `saveToFile` / `loadFromFile` | |
| **4** | File + đệ quy + tích hợp | Hoàn thiện đọc/ghi file, 4 hàm đệ quy; ghép toàn bộ module; tạo `data/students.dat` mẫu (~15–20 SV, đủ các mức xếp loại, ≥ 3 lớp) | Chương trình tích hợp chạy được, có lưu/đọc file |
| *Tự học* | | Chạy kịch bản kiểm thử (mục 11), sửa lỗi; chụp màn hình; viết báo cáo | |
| **5** | Bảo vệ | Demo, trả lời vấn đáp từng thành viên | Sản phẩm hoàn chỉnh + báo cáo |

---

## 11. Kịch bản kiểm thử

| # | Tình huống | Thao tác | Kết quả mong đợi |
|---|---|---|---|
| T1 | Lần chạy đầu tiên | Xóa `students.dat`, chạy chương trình | Không lỗi, báo "Da doc 0 sinh vien" |
| T2 | Thêm hợp lệ | Thêm SV001 đủ thông tin | Hiển thị trong danh sách, ĐTB & xếp loại đúng |
| T3 | Trùng mã | Thêm lại SV001 (hoặc `sv001`) | Báo trùng mã, yêu cầu nhập lại |
| T4 | Trường rỗng | Nhấn Enter khi nhập họ tên | Báo "Khong duoc de trong" |
| T5 | Điểm sai kiểu | Nhập `abc`, `7.5x`, `-1`, `11` | Từ chối, yêu cầu nhập lại |
| T6 | Menu sai | Nhập `99`, `a`, chuỗi rất dài | Không treo, không trôi lệnh |
| T7 | Tìm kiếm | Tìm mã có / không tồn tại; tìm tên `an` | Kết quả đúng; báo "Khong tim thay" |
| T8 | Sửa điểm | Sửa điểm môn 1 của SV001 | ĐTB và xếp loại cập nhật lại |
| T9 | Xóa đầu / giữa / cuối / duy nhất | Xóa từng vị trí | Danh sách còn lại đúng, không crash |
| T10 | Danh sách rỗng | Hiển thị, sắp xếp, thống kê, in ngược khi chưa có SV | Báo danh sách rỗng, không chia cho 0 |
| T11 | Sắp xếp | Thử cả 4 tiêu chí; danh sách 1 phần tử | Thứ tự đúng, không mất phần tử |
| T12 | Biên xếp loại | ĐTB = 9.0, 8.0, 6.5, 5.0, 3.5 | Rơi đúng vào mức tương ứng |
| T13 | **Bền vững dữ liệu** | Thêm / sửa / xóa → Thoát → Chạy lại | Dữ liệu y nguyên như trước khi thoát |
| T14 | File hỏng | Ghi đè `students.dat` bằng nội dung bất kỳ | Báo file không hợp lệ, chương trình vẫn chạy |
| T15 | Đối dòng lệnh | `student_manager -f test.dat` | Đọc/ghi đúng file chỉ định |
| T16 | Rò rỉ bộ nhớ *(tùy chọn)* | Chạy với Valgrind / Dr. Memory | Không còn vùng nhớ chưa giải phóng |

---

## 12. Hướng dẫn viết báo cáo và chuẩn bị vấn đáp

### 12.1. Cấu trúc báo cáo (theo Mục 7 của đề)

1. **Trang bìa** — Trường/Khoa/Bộ môn, học phần, tên đề tài, thành viên + MSSV, lớp, GVHD.
2. **Bảng phân công nhiệm vụ** và mức độ hoàn thành (bảng ở mục 9).
3. **Mục lục.**
4. **Chương 1 – Mô tả bài toán:** phát biểu bài toán, phạm vi, danh sách chức năng (mục 2).
5. **Chương 2 – Thiết kế:** `struct Student`, `Node`, hình vẽ DSLK; sơ đồ chức năng (mục 5.1);
   bảng hàm (mục 5.2); định dạng file (mục 7).
6. **Chương 3 – Cài đặt:** trích và giải thích mã tiêu biểu — thêm/xóa trên DSLK, 4 hàm đệ quy
   (điều kiện dừng, bước đệ quy, vẽ cây gọi cho `printListReverse` với 3 nút), Merge Sort + con trỏ
   hàm, `saveToFile`/`loadFromFile`/`readRecordAt`, kiểm tra dữ liệu nhập.
7. **Chương 4 – Kết quả:** ảnh chụp màn hình từng chức năng, bao gồm ảnh minh chứng T13
   (dữ liệu còn nguyên sau khi chạy lại) và các trường hợp nhập sai.
8. **Kết luận:** đạt được gì; hạn chế (nhập không dấu, đệ quy sâu với dữ liệu rất lớn, file nhị phân
   phụ thuộc trình biên dịch, số môn cố định); hướng phát triển (DSLK môn học động, đăng nhập,
   giao diện đồ họa, hoàn tác thao tác...).
9. **Tài liệu tham khảo.**

### 12.2. Câu hỏi vấn đáp thường gặp — mọi thành viên đều phải trả lời được

- Vì sao hàm `addStudent` / `deleteStudent` nhận `Node **head` mà không phải `Node *head`?
- Nếu quên `free` trong `deleteStudent` thì điều gì xảy ra?
- Vì sao `Student` không chứa con trỏ `next`? Nếu `fwrite` một struct có con trỏ thì sao?
- Chỉ ra điều kiện dừng của từng hàm đệ quy. Nếu bỏ điều kiện dừng thì sao?
- Merge Sort chia đôi DSLK như thế nào (kỹ thuật con trỏ nhanh/chậm)? Độ phức tạp?
- Con trỏ hàm `CompareFn` giúp gì so với viết 4 hàm sắp xếp riêng?
- `fseek` tính vị trí bản ghi thứ *i* như thế nào?
- Vì sao dùng `fgets` + `strtof` thay vì `scanf("%f")`?
- `#ifdef DEBUG`, include guard có tác dụng gì?
- Chương trình đảm bảo dữ liệu không mất giữa các lần chạy bằng cách nào?

---

## 13. Checklist trước khi nộp

**Yêu cầu kỹ thuật bắt buộc**

- [ ] Dữ liệu chính là `struct`; lưu trữ bằng **danh sách liên kết**, không dùng mảng tĩnh cho danh sách SV
- [ ] Đủ thêm / xóa / sửa / tìm kiếm (+ sắp xếp, thống kê)
- [ ] ≥ 7–10 hàm, `main()` gọn
- [ ] Có tham số con trỏ (`Node **`, `Student *`)
- [ ] Có ≥ 1 hàm đệ quy có ý nghĩa thực tế
- [ ] Lưu khi thoát, đọc lại khi khởi động — đã chạy test T13
- [ ] Menu lặp đến khi chọn thoát; kiểm tra dữ liệu nhập (rỗng, kiểu số, khoảng giá trị)

**Chất lượng**

- [ ] Biên dịch không lỗi, không cảnh báo với `-Wall -Wextra`
- [ ] Mọi `malloc` đều kiểm tra `NULL`; mọi nút đều được `free` khi thoát
- [ ] Mọi `fopen` đều kiểm tra `NULL` và có `fclose`
- [ ] Có chú thích cho từng hàm; tên biến/hàm rõ nghĩa
- [ ] Đã chạy hết bảng kiểm thử ở mục 11

**Hồ sơ nộp** (đặt tên theo quy định, ví dụ `Nhom03_LTNC_K64_BTL`)

- [ ] Mã nguồn `.c/.h` + `CMakeLists.txt` (không nộp thư mục `cmake-build-*`, `.idea/`)
- [ ] File dữ liệu mẫu `data/students.dat`
- [ ] Bảng phân công nhiệm vụ
- [ ] Báo cáo `.docx` hoặc `.pdf` đúng cấu trúc mục 12.1

---

> **Lưu ý học thuật:** Mã nguồn trong README chỉ là khung gợi ý. Theo Mục 10 của đề, sản phẩm
> phải do chính nhóm xây dựng và **mọi thành viên phải giải thích được toàn bộ hệ thống** —
> hãy tự viết lại, chạy thử và hiểu từng hàm trước buổi bảo vệ.
