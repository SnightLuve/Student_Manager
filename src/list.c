/*
 * list.c — Cài đặt danh sách liên kết đơn chứa sinh viên.
 * Phụ trách: TV1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

Node *createNode(const Student *s) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        LOG("createNode: malloc that bai");
        return NULL;
    }
    node->data = *s;          /* sao chép toàn bộ struct */
    node->next = NULL;
    return node;
}

/* ĐỆ QUY — dừng khi hết danh sách hoặc tìm thấy mã. */
Node *findById(Node *head, const char *id) {
    if (head == NULL) return NULL;                       /* dừng: hết DS  */
    if (strcmp(head->data.id, id) == 0) return head;     /* dừng: thấy    */
    return findById(head->next, id);                     /* bước đệ quy   */
}

/* Nhận Node** vì khi danh sách rỗng phải thay đổi chính con trỏ head. */
int addStudent(Node **head, const Student *s) {
    if (findById(*head, s->id) != NULL) return 0;      /* trùng mã */

    Node *node = createNode(s);
    if (node == NULL) return -1;                       /* hết bộ nhớ */
    computeAverage(&node->data);                       /* đảm bảo ĐTB luôn đúng */

    if (*head == NULL) {                               /* danh sách rỗng */
        *head = node;
    } else {
        Node *p = *head;
        while (p->next != NULL) p = p->next;           /* tới nút cuối */
        p->next = node;
    }
    LOG("addStudent: da them %s", s->id);
    return 1;
}

/* Nhận Node** vì xóa nút đầu sẽ thay đổi head. */
int deleteStudent(Node **head, const char *id) {
    Node *cur = *head, *prev = NULL;
    while (cur != NULL && strcmp(cur->data.id, id) != 0) {
        prev = cur;
        cur  = cur->next;
    }
    if (cur == NULL) return 0;              /* không tìm thấy */

    if (prev == NULL) *head = cur->next;    /* xóa nút đầu       */
    else              prev->next = cur->next; /* xóa nút giữa/cuối */
    free(cur);                              /* giải phóng — tránh rò rỉ bộ nhớ */
    LOG("deleteStudent: da xoa %s", id);
    return 1;
}

int updateStudent(Node *head, const Student *newData) {
    Node *node = findById(head, newData->id);
    if (node == NULL) return 0;

    /* Sửa trực tiếp qua con trỏ; không đổi mã (khóa). */
    strcpy(node->data.name, newData->name);
    strcpy(node->data.className, newData->className);
    for (int i = 0; i < MAX_SUBJECTS; i++)
        node->data.scores[i] = newData->scores[i];
    computeAverage(&node->data);            /* sửa điểm → tính lại ĐTB */
    LOG("updateStudent: da sua %s", newData->id);
    return 1;
}

/* ĐỆ QUY — dừng khi gặp NULL. */
int countNodes(const Node *head) {
    return head == NULL ? 0 : 1 + countNodes(head->next);
}

void printList(const Node *head) {
    if (head == NULL) {
        printf("  Danh sach rong.\n");
        return;
    }
    printTableHeader();
    int stt = 1;
    for (const Node *p = head; p != NULL; p = p->next)
        printStudentRow(stt++, &p->data);
    printf("  Tong so: %d sinh vien.\n", stt - 1);
}

/* Hàm phụ đệ quy: in phần sau trước, rồi mới in nút hiện tại.
 * stt được đánh theo vị trí gốc trong danh sách. */
static void printReverseRec(const Node *node, int position) {
    if (node == NULL) return;                       /* dừng */
    printReverseRec(node->next, position + 1);      /* bước đệ quy */
    printStudentRow(position, &node->data);
}

void printListReverse(const Node *head) {
    if (head == NULL) {
        printf("  Danh sach rong.\n");
        return;
    }
    printTableHeader();
    printReverseRec(head, 1);
}

void freeList(Node **head) {
    while (*head != NULL) {
        Node *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
    /* sau vòng lặp *head == NULL → tránh con trỏ treo */
}
