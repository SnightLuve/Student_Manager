/*
 * search_sort.c — Cài đặt tìm kiếm, lọc và Merge Sort với con trỏ hàm.
 * Phụ trách: TV2
 */
#include <stdio.h>
#include <string.h>

#include "search_sort.h"

/* ===================== Hàm phụ ===================== */

/* Decode UTF-8 independently of the active locale; invalid bytes compare literally. */
static unsigned int nextUtf8CodePoint(const unsigned char **text) {
    const unsigned char *p = *text;
    unsigned int codePoint;

    if (p[0] < 0x80) {
        *text = p + 1;
        return p[0];
    }
    if (p[0] >= 0xC2 && p[0] <= 0xDF
        && (p[1] & 0xC0) == 0x80) {
        codePoint = ((unsigned int)(p[0] & 0x1F) << 6) | (p[1] & 0x3F);
        *text = p + 2;
        return codePoint;
    }
    if (p[0] >= 0xE0 && p[0] <= 0xEF
        && (p[1] & 0xC0) == 0x80 && p[1] != '\0'
        && (p[2] & 0xC0) == 0x80
        && !(p[0] == 0xE0 && p[1] < 0xA0)
        && !(p[0] == 0xED && p[1] >= 0xA0)) {
        codePoint = ((unsigned int)(p[0] & 0x0F) << 12)
                    | ((unsigned int)(p[1] & 0x3F) << 6) | (p[2] & 0x3F);
        *text = p + 3;
        return codePoint;
    }
    if (p[0] >= 0xF0 && p[0] <= 0xF4
        && (p[1] & 0xC0) == 0x80 && p[1] != '\0'
        && (p[2] & 0xC0) == 0x80 && p[2] != '\0'
        && (p[3] & 0xC0) == 0x80
        && !(p[0] == 0xF0 && p[1] < 0x90)
        && !(p[0] == 0xF4 && p[1] > 0x8F)) {
        codePoint = ((unsigned int)(p[0] & 0x07) << 18)
                    | ((unsigned int)(p[1] & 0x3F) << 12)
                    | ((unsigned int)(p[2] & 0x3F) << 6) | (p[3] & 0x3F);
        *text = p + 4;
        return codePoint;
    }

    *text = p + 1;
    return p[0];
}

/* Fold ASCII and Latin case pairs, including precomposed Vietnamese letters. */
static unsigned int foldCodePoint(unsigned int codePoint) {
    if (codePoint >= 'A' && codePoint <= 'Z') return codePoint + ('a' - 'A');
    if ((codePoint >= 0x00C0 && codePoint <= 0x00D6)
        || (codePoint >= 0x00D8 && codePoint <= 0x00DE))
        return codePoint + 0x20;
    if ((codePoint >= 0x0100 && codePoint <= 0x012F && codePoint % 2 == 0)
        || (codePoint >= 0x0132 && codePoint <= 0x0137 && codePoint % 2 == 0)
        || (codePoint >= 0x014A && codePoint <= 0x0177 && codePoint % 2 == 0)
        || (codePoint >= 0x1E00 && codePoint <= 0x1E95 && codePoint % 2 == 0)
        || (codePoint >= 0x1EA0 && codePoint <= 0x1EF9 && codePoint % 2 == 0))
        return codePoint + 1;
    if (codePoint >= 0x0139 && codePoint <= 0x0148 && codePoint % 2 == 1)
        return codePoint + 1;
    if (codePoint >= 0x0179 && codePoint <= 0x017E && codePoint % 2 == 1)
        return codePoint + 1;
    if (codePoint == 0x0178) return 0x00FF;
    if (codePoint == 0x1E9E) return 0x00DF;
    return codePoint;
}

static int compareIgnoreCase(const char *a, const char *b) {
    const unsigned char *left = (const unsigned char *)a;
    const unsigned char *right = (const unsigned char *)b;
    while (*left != '\0' && *right != '\0') {
        unsigned int ca = foldCodePoint(nextUtf8CodePoint(&left));
        unsigned int cb = foldCodePoint(nextUtf8CodePoint(&right));
        if (ca != cb) return ca < cb ? -1 : 1;
    }
    return *left == *right ? 0 : (*left == '\0' ? -1 : 1);
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
    if (*keyword == '\0') return 1;
    const unsigned char *start = (const unsigned char *)text;
    while (*start != '\0') {
        const unsigned char *candidate = start;
        const unsigned char *query = (const unsigned char *)keyword;
        int match = 1;
        while (*candidate != '\0' && *query != '\0') {
            unsigned int a = foldCodePoint(nextUtf8CodePoint(&candidate));
            unsigned int b = foldCodePoint(nextUtf8CodePoint(&query));
            if (a != b) {
                match = 0;
                break;
            }
        }
        if (match && *query == '\0') return 1;
        nextUtf8CodePoint(&start);
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
        if (p->data.avg >= a && p->data.avg <= b)
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
