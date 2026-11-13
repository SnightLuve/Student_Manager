/*
 * search_sort.c — Cài đặt tìm kiếm, lọc và Merge Sort với con trỏ hàm.
 * Phụ trách: TV2
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "search_sort.h"

/* Sai số khi so ĐTB với biên — giống classify trong student.c. */
#define AVG_EPS 1e-4f

/* ===================== Hàm phụ ===================== */

/* So sánh hai chuỗi không phân biệt hoa thường (strcasecmp không thuộc C11).
 * Trả về < 0, 0, > 0 như strcmp. */
static int compareIgnoreCase(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Trả về con trỏ tới tên (từ cuối) trong họ tên; không có dấu cách thì cả chuỗi. */
static const char *givenName(const char *fullName) {
    const char *space = strrchr(fullName, ' ');
    return space != NULL ? space + 1 : fullName;
}

/* In một dòng kết quả; in tiêu đề bảng trước dòng đầu tiên. */
static void printMatch(int *found, const Student *s) {
    if (*found == 0) printTableHeader();
    (*found)++;
    printStudentRow(*found, s);
}

/* In thông báo sau khi tìm: không thấy hoặc tổng số kết quả. */
static void printSearchSummary(int found) {
    if (found == 0) printf("  Khong tim thay sinh vien phu hop.\n");
    else            printf("  Tim thay %d sinh vien.\n", found);
}

/* ===================== Tìm kiếm / lọc ===================== */

int containsIgnoreCase(const char *text, const char *keyword) {
    if (text == NULL || keyword == NULL) return 0;
    size_t keyLen = strlen(keyword);
    if (keyLen == 0) return 1;
    for (; *text != '\0'; text++) {             /* thử từng vị trí bắt đầu */
        size_t i = 0;
        while (i < keyLen && text[i] != '\0'
               && tolower((unsigned char)text[i]) == tolower((unsigned char)keyword[i]))
            i++;
        if (i == keyLen) return 1;
    }
    return 0;
}

int searchByName(const Node *head, const char *keyword) {
    int found = 0;
    for (const Node *p = head; p != NULL; p = p->next)
        if (containsIgnoreCase(p->data.name, keyword)) printMatch(&found, &p->data);
    printSearchSummary(found);
    return found;
}

int searchByClass(const Node *head, const char *keyword) {
    int found = 0;
    for (const Node *p = head; p != NULL; p = p->next)
        if (containsIgnoreCase(p->data.className, keyword)) printMatch(&found, &p->data);
    printSearchSummary(found);
    return found;
}

int filterByAvgRange(const Node *head, float a, float b) {
    if (a > b) {                                /* người dùng nhập ngược biên */
        float tmp = a;
        a = b;
        b = tmp;
    }
    int found = 0;
    for (const Node *p = head; p != NULL; p = p->next)
        if (p->data.avg + AVG_EPS >= a && p->data.avg - AVG_EPS <= b)
            printMatch(&found, &p->data);
    printSearchSummary(found);
    return found;
}

/* ===================== Hàm so sánh ===================== */

int cmpById(const Student *a, const Student *b) {
    return strcmp(a->id, b->id);
}

int cmpByName(const Student *a, const Student *b) {
    int c = compareIgnoreCase(givenName(a->name), givenName(b->name)); /* tên trước */
    if (c != 0) return c;
    c = compareIgnoreCase(a->name, b->name);                           /* rồi họ đệm */
    if (c != 0) return c;
    return strcmp(a->id, b->id);                /* thứ tự xác định khi trùng hẳn */
}

int cmpByAvgDesc(const Student *a, const Student *b) {
    if (a->avg < b->avg) return 1;
    if (a->avg > b->avg) return -1;
    return 0;
}

int cmpByClassThenAvg(const Student *a, const Student *b) {
    int c = strcmp(a->className, b->className);
    return c != 0 ? c : cmpByAvgDesc(a, b);
}

/* ===================== Merge Sort ===================== */

/* Chia đôi danh sách (src có ít nhất 2 nút) bằng con trỏ nhanh/chậm:
 * fast đi 2 bước, slow đi 1 bước → khi fast hết, slow ở giữa. */
static void splitList(Node *src, Node **front, Node **back) {
    Node *slow = src, *fast = src->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    *front = src;
    *back  = slow->next;
    slow->next = NULL;                          /* cắt đứt hai nửa */
}

/* Trộn hai danh sách đã sắp xếp (ĐỆ QUY).
 * Dừng: một trong hai danh sách rỗng. Bước đệ quy: lấy nút nhỏ hơn làm đầu,
 * nối với kết quả trộn phần còn lại. Dùng <= 0 để giữ ổn định. */
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

/* ĐỆ QUY — dừng khi danh sách có 0 hoặc 1 nút;
 * bước đệ quy: chia đôi, sắp xếp từng nửa, trộn lại. */
void mergeSort(Node **head, CompareFn cmp) {
    if (head == NULL || *head == NULL || (*head)->next == NULL) return;
    Node *a, *b;
    splitList(*head, &a, &b);
    mergeSort(&a, cmp);
    mergeSort(&b, cmp);
    *head = mergeLists(a, b, cmp);
}
