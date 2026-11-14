/*
 * search_sort.h — Tìm kiếm, lọc và sắp xếp danh sách sinh viên
 * (Merge Sort đệ quy + con trỏ hàm so sánh).
 * Phụ trách: TV2
 *
 * Tìm theo mã: dùng findById (ĐỆ QUY) đã khai báo trong list.h, không định nghĩa lại.
 */
#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "list.h"

/* Con trỏ hàm so sánh hai sinh viên — dùng để chọn tiêu chí sắp xếp.
 * Trả về < 0 nếu a đứng trước b, 0 nếu ngang nhau, > 0 nếu a đứng sau b. */
typedef int (*CompareFn)(const Student *a, const Student *b);

/* ---------- Tìm kiếm / lọc ---------- */

/* Kiểm tra text có chứa keyword (không phân biệt hoa thường).
 * Trả về 1 nếu có (keyword rỗng luôn khớp), 0 nếu không hoặc tham số NULL. */
int containsIgnoreCase(const char *text, const char *keyword);

/* In bảng các SV có họ tên chứa keyword (không phân biệt hoa thường).
 * Trả về số SV khớp; 0 thì in "Khong tim thay". */
int searchByName(const Node *head, const char *keyword);

/* In bảng các SV có tên lớp chứa keyword (không phân biệt hoa thường).
 * Trả về số SV khớp; 0 thì in "Khong tim thay". */
int searchByClass(const Node *head, const char *keyword);

/* In bảng các SV có ĐTB trong đoạn [a, b] (tự hoán đổi nếu a > b;
 * biên có sai số 1e-4 như classify). Trả về số SV khớp. */
int filterByAvgRange(const Node *head, float a, float b);

/* ---------- Hàm so sánh (dùng làm con trỏ hàm) ---------- */

/* Theo mã SV tăng dần. */
int cmpById(const Student *a, const Student *b);

/* Theo tên (từ cuối của họ tên) tăng dần, không phân biệt hoa thường;
 * trùng tên thì so cả họ tên (họ đệm), cuối cùng so mã. */
int cmpByName(const Student *a, const Student *b);

/* Theo ĐTB giảm dần. */
int cmpByAvgDesc(const Student *a, const Student *b);

/* Theo lớp tăng dần, cùng lớp thì ĐTB giảm dần. */
int cmpByClassThenAvg(const Student *a, const Student *b);

/* ---------- Sắp xếp ---------- */

/* Merge Sort (ĐỆ QUY) trên DSLK theo tiêu chí cmp; ổn định, O(n log n).
 * Chỉ nối lại con trỏ next, không cấp phát/giải phóng nút.
 * Nhận Node** vì nút đầu có thể thay đổi. */
void mergeSort(Node **head, CompareFn cmp);

#endif /* SEARCH_SORT_H */
