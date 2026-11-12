/*
 * list.h — Danh sách liên kết đơn chứa sinh viên:
 * tạo nút, thêm, xóa, sửa, đếm, in, giải phóng.
 * Phụ trách: TV1
 */
#ifndef LIST_H
#define LIST_H

#include "student.h"

/* Nút danh sách liên kết đơn — con trỏ next tách khỏi Student. */
typedef struct Node {
    Student      data;
    struct Node *next;
} Node;

/* Cấp phát (malloc) một nút mới chứa bản sao của *s. Trả về NULL nếu hết bộ nhớ. */
Node *createNode(const Student *s);

/* Tìm nút theo mã SV (ĐỆ QUY). Trả về con trỏ tới nút hoặc NULL.
 * Lưu ý: hàm đặt ở list.c vì addStudent/updateStudent cần dùng;
 * module search_sort dùng lại hàm này, không định nghĩa lại. */
Node *findById(Node *head, const char *id);

/* Thêm sinh viên vào CUỐI danh sách (chặn trùng mã).
 * Trả về: 1 thành công, 0 trùng mã, -1 hết bộ nhớ. */
int addStudent(Node **head, const Student *s);

/* Xóa sinh viên theo mã và giải phóng nút.
 * Trả về: 1 đã xóa, 0 không tìm thấy. */
int deleteStudent(Node **head, const char *id);

/* Sửa thông tin sinh viên có mã newData->id: thay họ tên, lớp, điểm
 * (giữ nguyên mã) và tính lại ĐTB. Trả về: 1 thành công, 0 không tìm thấy. */
int updateStudent(Node *head, const Student *newData);

/* Đếm số nút (ĐỆ QUY). */
int countNodes(const Node *head);

/* In toàn bộ danh sách dạng bảng; báo "Danh sach rong" nếu trống. */
void printList(const Node *head);

/* In danh sách theo thứ tự ngược (ĐỆ QUY). */
void printListReverse(const Node *head);

/* Giải phóng toàn bộ danh sách và đặt *head = NULL. */
void freeList(Node **head);

#endif /* LIST_H */
