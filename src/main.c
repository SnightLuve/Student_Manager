/*
 * main.c — Menu điều khiển các chức năng của module TV1 (student + list)
 * bằng vòng lặp do-while và switch-case.
 * TV4 có thể tách phần nhập liệu sang input.c và phần menu sang menu.c sau.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "list.h"

/* ===================== Nhập liệu an toàn (fgets) ===================== */

/* Xóa khoảng trắng đầu/cuối chuỗi. */
static void trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

/* Đọc một dòng, bỏ '\n'; nếu dòng dài hơn bộ đệm thì xả phần thừa. */
static void readLine(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (fgets(buf, (int)size, stdin) == NULL) { buf[0] = '\0'; return; }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    trim(buf);
}

/* Đọc chuỗi không rỗng. */
static void readNonEmpty(const char *prompt, char *buf, size_t size) {
    do {
        readLine(prompt, buf, size);
        if (buf[0] == '\0') printf("  ! Khong duoc de trong.\n");
    } while (buf[0] == '\0');
}

/* Đọc số nguyên trong [min, max]. */
static int readInt(const char *prompt, int min, int max) {
    char line[MAX_LINE];
    char *end;
    for (;;) {
        readLine(prompt, line, sizeof line);
        long v = strtol(line, &end, 10);
        if (end != line && *end == '\0' && v >= min && v <= max) return (int)v;
        printf("  ! Vui long nhap so nguyen trong khoang [%d, %d].\n", min, max);
    }
}

/* Đọc số thực trong [min, max]. */
static float readFloat(const char *prompt, float min, float max) {
    char line[MAX_LINE];
    char *end;
    for (;;) {
        readLine(prompt, line, sizeof line);
        float v = strtof(line, &end);
        if (end != line && *end == '\0' && v >= min && v <= max) return v;
        printf("  ! Vui long nhap so trong khoang [%.1f, %.1f].\n", min, max);
    }
}

/* Hỏi y/n, trả về 1 nếu đồng ý. */
static int confirm(const char *prompt) {
    char line[MAX_LINE];
    for (;;) {
        readLine(prompt, line, sizeof line);
        if (line[0] == 'y' || line[0] == 'Y') return 1;
        if (line[0] == 'n' || line[0] == 'N') return 0;
        printf("  ! Nhap y hoac n.\n");
    }
}

/* Đọc mã SV: không rỗng, không chứa dấu cách, chuyển về in hoa. */
static void readId(const char *prompt, char *id) {
    for (;;) {
        readNonEmpty(prompt, id, MAX_ID);
        if (strchr(id, ' ') == NULL) { normalizeId(id); return; }
        printf("  ! Ma SV khong duoc chua dau cach.\n");
    }
}

/* Đọc họ tên: không rỗng, không chứa chữ số. */
static void readName(const char *prompt, char *name) {
    for (;;) {
        readNonEmpty(prompt, name, MAX_NAME);
        int hasDigit = 0;
        for (const char *p = name; *p != '\0'; p++)
            if (isdigit((unsigned char)*p)) { hasDigit = 1; break; }
        if (!hasDigit) return;
        printf("  ! Ho ten khong duoc chua chu so.\n");
    }
}

/* Nhập điểm cho cả MAX_SUBJECTS môn. */
static void readScores(float scores[]) {
    char prompt[64];
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        snprintf(prompt, sizeof prompt, "  Diem %-13s: ", SUBJECT_NAMES[i]);
        scores[i] = readFloat(prompt, MIN_SCORE, MAX_SCORE);
    }
}

static void waitEnter(void) {
    char tmp[MAX_LINE];
    readLine("\nNhan Enter de tiep tuc...", tmp, sizeof tmp);
}

/* ===================== Xử lý từng mục menu ===================== */

static void handleAdd(Node **head) {
    Student s;
    char id[MAX_ID], name[MAX_NAME], className[MAX_CLASS];
    float scores[MAX_SUBJECTS];

    printf("\n--- THEM SINH VIEN ---\n");
    for (;;) {                                   /* chặn trùng mã ngay khi nhập */
        readId("  Ma SV        : ", id);
        if (findById(*head, id) == NULL) break;
        printf("  ! Ma %s da ton tai, nhap ma khac.\n", id);
    }
    readName("  Ho ten       : ", name);
    readNonEmpty("  Lop          : ", className, sizeof className);
    readScores(scores);

    initStudent(&s, id, name, className, scores);
    switch (addStudent(head, &s)) {
        case 1:  printf("  => Da them. DTB = %.2f, xep loai: %s\n",
                        s.avg, rankName(classify(s.avg)));   break;
        case 0:  printf("  ! Trung ma sinh vien.\n");          break;
        default: printf("  ! Loi: het bo nho.\n");             break;
    }
}

static void handleFind(Node *head) {
    char id[MAX_ID];
    printf("\n--- TIM THEO MA SV ---\n");
    readId("  Nhap ma SV: ", id);
    Node *p = findById(head, id);
    if (p == NULL) printf("  Khong tim thay sinh vien %s.\n", id);
    else           printStudentDetail(&p->data);
}

static void handleEdit(Node *head) {
    char id[MAX_ID];
    printf("\n--- SUA THONG TIN ---\n");
    readId("  Nhap ma SV can sua: ", id);
    Node *p = findById(head, id);
    if (p == NULL) { printf("  Khong tim thay sinh vien %s.\n", id); return; }

    int choice;
    do {                                          /* menu con: do-while + switch */
        printf("\n");
        printStudentDetail(&p->data);
        printf("\n  1. Sua ho ten\n"
               "  2. Sua lop\n"
               "  3. Sua diem mot mon\n"
               "  4. Sua toan bo diem\n"
               "  0. Xong\n");
        choice = readInt("  Chon: ", 0, 4);

        switch (choice) {
            case 1: {
                char name[MAX_NAME];
                readName("  Ho ten moi: ", name);
                setName(&p->data, name);
                printf("  => Da sua ho ten.\n");
                break;
            }
            case 2: {
                char className[MAX_CLASS];
                readNonEmpty("  Lop moi: ", className, sizeof className);
                setClassName(&p->data, className);
                printf("  => Da sua lop.\n");
                break;
            }
            case 3: {
                for (int i = 0; i < MAX_SUBJECTS; i++)
                    printf("    %d. %s\n", i + 1, SUBJECT_NAMES[i]);
                int sub = readInt("  Chon mon: ", 1, MAX_SUBJECTS);
                float score = readFloat("  Diem moi: ", MIN_SCORE, MAX_SCORE);
                setScore(&p->data, sub - 1, score);       /* tự tính lại ĐTB */
                printf("  => Da sua diem. DTB moi = %.2f\n", p->data.avg);
                break;
            }
            case 4: {
                float scores[MAX_SUBJECTS];
                readScores(scores);
                setAllScores(&p->data, scores);           /* tự tính lại ĐTB */
                printf("  => Da sua diem. DTB moi = %.2f\n", p->data.avg);
                break;
            }
            case 0:
                printf("  Ket thuc sua.\n");
                break;
        }
    } while (choice != 0);
}

static void handleDelete(Node **head) {
    char id[MAX_ID];
    printf("\n--- XOA SINH VIEN ---\n");
    readId("  Nhap ma SV can xoa: ", id);
    Node *p = findById(*head, id);
    if (p == NULL) { printf("  Khong tim thay sinh vien %s.\n", id); return; }

    printStudentDetail(&p->data);
    if (confirm("  Ban chac chan muon xoa? (y/n): ")) {
        deleteStudent(head, id);
        printf("  => Da xoa sinh vien %s.\n", id);
    } else {
        printf("  Da huy.\n");
    }
}

static void printMainMenu(void) {
    printf("\n============================================\n");
    printf("        CHUONG TRINH QUAN LY SINH VIEN\n");
    printf("============================================\n");
    printf("  1. Them sinh vien\n");
    printf("  2. Hien thi danh sach sinh vien\n");
    printf("  3. Tim sinh vien theo ma\n");
    printf("  4. Sua thong tin sinh vien\n");
    printf("  5. Xoa sinh vien\n");
    printf("  6. In danh sach theo thu tu nguoc\n");
    printf("  7. Dem so sinh vien\n");
    printf("  0. Thoat\n");
    printf("--------------------------------------------\n");
}

/* ===================== main ===================== */

int main(void) {
    Node *head = NULL;
    int choice;

    do {
        printMainMenu();
        choice = readInt("Lua chon cua ban: ", 0, 7);

        switch (choice) {
            case 1: handleAdd(&head);       break;
            case 2: printList(head);        break;
            case 3: handleFind(head);       break;
            case 4: handleEdit(head);       break;
            case 5: handleDelete(&head);    break;
            case 6: printListReverse(head); break;
            case 7: printf("  So sinh vien: %d\n", countNodes(head)); break;
            case 0: printf("Tam biet!\n");  break;
        }
        if (choice != 0) waitEnter();
    } while (choice != 0);

    freeList(&head);                       /* giải phóng toàn bộ bộ nhớ */
    return 0;
}
