/*
 * main.c — Menu điều khiển module TV1 (student + list) và TV3 (file_io).
 * bằng vòng lặp do-while và switch-case.
 * TV4 có thể tách phần nhập liệu sang input.c và phần menu sang menu.c sau.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include "file_io.h"

/* ===================== Nhập liệu an toàn (fgets) ===================== */

static int inputEnded = 0;

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
    if (fgets(buf, (int)size, stdin) == NULL) {
        inputEnded = 1;
        buf[0] = '\0';
        return;
    }
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
        if (inputEnded) return;
        if (buf[0] == '\0') printf("  ! Khong duoc de trong.\n");
    } while (buf[0] == '\0');
}

/* Đọc số nguyên trong [min, max]. */
static int readInt(const char *prompt, int min, int max) {
    char line[MAX_LINE];
    char *end;
    for (;;) {
        readLine(prompt, line, sizeof line);
        if (inputEnded) return min;
        errno = 0;
        long v = strtol(line, &end, 10);
        if (errno != ERANGE && end != line && *end == '\0' && v >= min && v <= max)
            return (int)v;
        printf("  ! Vui long nhap so nguyen trong khoang [%d, %d].\n", min, max);
    }
}

/* Đọc số thực trong [min, max]. */
static float readFloat(const char *prompt, float min, float max) {
    char line[MAX_LINE];
    char *end;
    for (;;) {
        readLine(prompt, line, sizeof line);
        if (inputEnded) return min;
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
        if (inputEnded) return 0;
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

static int handleSave(const Node *head, const char *dataFile);

/* Ghi ngay sau mỗi thay đổi thành công; lỗi ghi không làm mất dữ liệu trong RAM. */
static void autoSave(const Node *head, const char *dataFile) {
    if (!handleSave(head, dataFile))
        printf("  ! Thay doi moi CHUA DUOC LUU, chi con trong RAM. "
               "Hay khac phuc loi, chuong trinh se thu luu lai khi thoat.\n");
}

static void handleAdd(Node **head, const char *dataFile) {
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

    if (inputEnded) return;                       /* không thêm bản ghi nhập dở */

    initStudent(&s, id, name, className, scores);
    switch (addStudent(head, &s)) {
        case 1:  printf("  => Da them. DTB = %.2f, xep loai: %s\n",
                        s.avg, rankName(classify(s.avg)));
                 autoSave(*head, dataFile);                 break;
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

static void handleEdit(Node *head, const char *dataFile) {
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
                if (inputEnded) return;
                if (setName(&p->data, name)) {
                    printf("  => Da sua ho ten.\n");
                    autoSave(head, dataFile);
                }
                break;
            }
            case 2: {
                char className[MAX_CLASS];
                readNonEmpty("  Lop moi: ", className, sizeof className);
                if (inputEnded) return;
                if (setClassName(&p->data, className)) {
                    printf("  => Da sua lop.\n");
                    autoSave(head, dataFile);
                }
                break;
            }
            case 3: {
                for (int i = 0; i < MAX_SUBJECTS; i++)
                    printf("    %d. %s\n", i + 1, SUBJECT_NAMES[i]);
                int sub = readInt("  Chon mon: ", 1, MAX_SUBJECTS);
                float score = readFloat("  Diem moi: ", MIN_SCORE, MAX_SCORE);
                if (inputEnded) return;
                if (setScore(&p->data, sub - 1, score)) { /* tự tính lại ĐTB */
                    printf("  => Da sua diem. DTB moi = %.2f\n", p->data.avg);
                    autoSave(head, dataFile);
                }
                break;
            }
            case 4: {
                float scores[MAX_SUBJECTS];
                readScores(scores);
                if (inputEnded) return;
                if (setAllScores(&p->data, scores)) {     /* tự tính lại ĐTB */
                    printf("  => Da sua diem. DTB moi = %.2f\n", p->data.avg);
                    autoSave(head, dataFile);
                }
                break;
            }
            case 0:
                printf("  Ket thuc sua.\n");
                break;
        }
    } while (choice != 0);
}

static void handleDelete(Node **head, const char *dataFile) {
    char id[MAX_ID];
    printf("\n--- XOA SINH VIEN ---\n");
    readId("  Nhap ma SV can xoa: ", id);
    Node *p = findById(*head, id);
    if (p == NULL) { printf("  Khong tim thay sinh vien %s.\n", id); return; }

    printStudentDetail(&p->data);
    if (confirm("  Ban chac chan muon xoa? (y/n): ")) {
        if (deleteStudent(head, id)) {
            printf("  => Da xoa sinh vien %s.\n", id);
            autoSave(*head, dataFile);
        }
    } else {
        printf("  Da huy.\n");
    }
}

/* Lưu toàn bộ DSLK, bao gồm các thay đổi thêm/sửa/xóa từ lần lưu trước. */
static int handleSave(const Node *head, const char *dataFile) {
    if (!saveToFile(dataFile, head)) {
        printf("  ! Khong luu duoc %s. Kiem tra thu muc, quyen ghi va file .tmp.\n",
               dataFile);
        return 0;
    }
    printf("  => Da luu %d sinh vien vao %s.\n", countNodes(head), dataFile);
    return 1;
}

/* Doc file mau rieng, bo qua ma da co, khong ghi de sinh vien dang su dung. */
static void handleImportSample(Node **head, const char *dataFile) {
    Node *sample = NULL;
    printf("\n--- IMPORT DU LIEU MAU ---\n");
    printf("  File mau: %s\n", DEFAULT_SAMPLE_FILE);
    int total = loadFromFile(DEFAULT_SAMPLE_FILE, &sample);
    if (total <= 0) {
        printf("  ! Khong doc duoc du lieu mau: file thieu, rong hoac khong hop le.\n");
        freeList(&sample);
        return;
    }
    int skipped = 0;
    Node **link = &sample;
    while (*link != NULL) {
        Node *node = *link;
        if (findById(*head, node->data.id) != NULL) {
            *link = node->next;
            free(node);
            ++skipped;
        } else {
            link = &node->next;
        }
    }
    Node **tail = head;
    while (*tail != NULL) tail = &(*tail)->next;
    *tail = sample;
    printf("  => Da import %d sinh vien mau, bo qua %d ma da ton tai.\n",
           total - skipped, skipped);
    if (total > skipped) autoSave(*head, dataFile);
}

/* CSV dùng dữ liệu hiện tại trong RAM, không cần lưu nhị phân trước. */
static void handleExportCsv(const Node *head, const char *dataFile) {
    char path[MAX_LINE];
    readLine("  File CSV (Enter de dung report.csv): ", path, sizeof path);
    if (inputEnded) return;
    if (path[0] == '\0') strcpy(path, "report.csv");
    if (strcmp(path, dataFile) == 0) {
        printf("  ! Hay chon file CSV khac file nhi phan dang su dung.\n");
        return;
    }
    if (exportCsv(path, head)) printf("  => Da xuat CSV: %s\n", path);
    else printf("  ! Khong xuat duoc CSV. Kiem tra duong dan/quyen ghi/file .tmp.\n");
}

/* STT giao diện bắt đầu từ 1, index của readRecordAt bắt đầu từ 0. */
static void handleReadRecord(const char *dataFile) {
    printf("\n--- XEM BAN GHI TRONG FILE DA LUU ---\n");
    printf("  File: %s (chua bao gom thay doi chua luu).\n", dataFile);
    int stt = readInt("  Nhap STT sinh vien trong file (tu 1): ", 1, INT_MAX);
    if (inputEnded) return;
    Student s;
    if (readRecordAt(dataFile, stt - 1, &s)) printStudentDetail(&s);
    else printf("  ! Khong doc duoc: STT ngoai pham vi hoac file khong hop le.\n");
}

static void printMainMenu(const char *dataFile) {
    printf("\n============================================\n");
    printf("        CHUONG TRINH QUAN LY SINH VIEN\n");
    printf("============================================\n");
    printf("  File du lieu: %s\n", dataFile);
    printf("  Tu dong luu sau moi lan them/sua/xoa thanh cong.\n");
    printf("  1. Them sinh vien\n");
    printf("  2. Hien thi danh sach sinh vien\n");
    printf("  3. Tim sinh vien theo ma\n");
    printf("  4. Sua thong tin sinh vien\n");
    printf("  5. Xoa sinh vien\n");
    printf("  6. In danh sach theo thu tu nguoc\n");
    printf("  7. Dem so sinh vien\n");
    printf("  8. Import du lieu mau\n");
    printf("  9. Xuat bao cao CSV\n");
    printf(" 10. Xem ban ghi trong file da luu\n");
    printf("  0. Thoat\n");
    printf("--------------------------------------------\n");
}

/* ===================== main ===================== */

int main(int argc, char **argv) {
    const char *dataFile = DEFAULT_DATA_FILE;
    if (argc == 3 && strcmp(argv[1], "-f") == 0 && argv[2][0] != '\0') {
        dataFile = argv[2];
    } else if (argc != 1) {
        fprintf(stderr, "Cach dung: %s [-f <file_du_lieu>]\n", argv[0]);
        return EXIT_FAILURE;
    }
    /* Chay tu thu muc build van tao duoc file du lieu mac dinh lan dau. */
    if (strcmp(dataFile, DEFAULT_DATA_FILE) == 0) {
#ifdef _WIN32
        int created = _mkdir("data");
#else
        int created = mkdir("data", 0777);
#endif
        if (created != 0 && errno != EEXIST) {
            fprintf(stderr, "Khong tao duoc thu muc data.\n");
            return EXIT_FAILURE;
        }
    }
    Node *head = NULL;
    int loaded = loadFromFile(dataFile, &head);
    if (loaded < 0) {
        fprintf(stderr, "Khong doc duoc %s: file loi/khong hop le hoac thieu bo nho.\n"
                        "File duoc giu nguyen. Hay kiem tra hoac chon file khac bang -f.\n",
                dataFile);
        return EXIT_FAILURE;
    }
    printf("Da doc %d sinh vien tu %s.\n", loaded, dataFile);
    int choice;
    int result = EXIT_SUCCESS;

    do {
        printMainMenu(dataFile);
        choice = readInt("Lua chon cua ban: ", 0, 10);

        switch (choice) {
            case 1: handleAdd(&head, dataFile); break;
            case 2: printList(head);        break;
            case 3: handleFind(head);       break;
            case 4: handleEdit(head, dataFile); break;
            case 5: handleDelete(&head, dataFile); break;
            case 6: printListReverse(head); break;
            case 7: printf("  So sinh vien: %d\n", countNodes(head)); break;
            case 8: handleImportSample(&head, dataFile); break;
            case 9: handleExportCsv(head, dataFile); break;
            case 10: handleReadRecord(dataFile); break;
            case 0:
                if (handleSave(head, dataFile)) {
                    printf("Tam biet!\n");
                } else if (inputEnded) {
                    result = EXIT_FAILURE;
                } else if (!confirm("  Thoat ma khong luu cac thay doi? (y/n): ")) {
                    choice = -1;            /* trở lại menu để thử lưu lại */
                    if (inputEnded) {
                        choice = 0;
                        result = EXIT_FAILURE;
                    }
                }
                break;
        }
        if (choice != 0) waitEnter();
    } while (choice != 0);

    freeList(&head);                       /* giải phóng toàn bộ bộ nhớ */
    return result;
}
