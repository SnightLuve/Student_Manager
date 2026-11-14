/* Kiem thu don vi module TV1 (student + list): khoi tao, chuan hoa, setter,
 * them/xoa/sua/tim/dem/in/giai phong DSLK, va danh sach lon (do sau de quy). */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "search_sort.h"
#include "stats.h"
#include "test_capture.h"

/* Dung CHECK thay assert de test van chay o Release/NDEBUG. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#define CLOSE(x, y) (fabsf((x) - (y)) < 1e-4f)
#define CAPTURE_FILE "student_list_capture.txt"

static Student make(const char *id, float score) {
    Student s;
    float scores[MAX_SUBJECTS];
    for (int i = 0; i < MAX_SUBJECTS; ++i) scores[i] = score;
    initStudent(&s, id, "Nguyen Van A", "CNTT1", scores);
    return s;
}

/* Thu tu ma SV trong danh sach phai dung bang ids (NULL ket thuc). */
static void checkOrder(const Node *head, const char *const ids[]) {
    int i = 0;
    for (const Node *p = head; p != NULL; p = p->next, ++i) {
        CHECK(ids[i] != NULL);
        CHECK(strcmp(p->data.id, ids[i]) == 0);
    }
    CHECK(ids[i] == NULL);
}

/* ===================== student.c ===================== */

static void testInitStudent(void) {
    float scores[MAX_SUBJECTS] = {10.0f, 9.0f, 8.0f, 7.0f, 6.0f};
    Student s;
    initStudent(&s, "sv001", "Tran Thi B", "CNTT2", scores);
    CHECK(strcmp(s.id, "SV001") == 0);                 /* chuan hoa in hoa */
    CHECK(strcmp(s.name, "Tran Thi B") == 0);
    CHECK(strcmp(s.className, "CNTT2") == 0);
    CHECK(CLOSE(s.avg, 8.0f));

    /* Chuoi qua dai bi cat dung kich thuoc, luon co '\0'. */
    char longText[200];
    memset(longText, 'x', sizeof longText - 1);
    longText[sizeof longText - 1] = '\0';
    initStudent(&s, longText, longText, longText, scores);
    CHECK(strlen(s.id) == MAX_ID - 1);
    CHECK(strlen(s.name) == MAX_NAME - 1);
    CHECK(strlen(s.className) == MAX_CLASS - 1);
    CHECK(s.id[0] == 'X');

    /* NULL → chuoi rong, diem 0. */
    initStudent(&s, NULL, NULL, NULL, NULL);
    CHECK(s.id[0] == '\0' && s.name[0] == '\0' && s.className[0] == '\0');
    for (int i = 0; i < MAX_SUBJECTS; ++i) CHECK(s.scores[i] == 0.0f);
    CHECK(s.avg == 0.0f);

    char id[] = "ab-c1z";
    normalizeId(id);
    CHECK(strcmp(id, "AB-C1Z") == 0);
    puts("PASS: initStudent, normalizeId, cat chuoi qua dai, tham so NULL");
}

static void testScoresAndRank(void) {
    CHECK(isValidScore(0.0f) && isValidScore(10.0f) && isValidScore(5.5f));
    CHECK(!isValidScore(-0.01f) && !isValidScore(10.01f));
    CHECK(!isValidScore(NAN) && !isValidScore(INFINITY) && !isValidScore(-INFINITY));

    CHECK(classify(10.0f) == RANK_EXCELLENT);
    CHECK(classify(0.0f) == RANK_POOR);
    CHECK(strcmp(rankName(RANK_EXCELLENT), "Xuat sac") == 0);
    CHECK(strcmp(rankName(RANK_POOR), "Kem") == 0);
    CHECK(strcmp(rankName(RANK_COUNT), "?") == 0);
    CHECK(strcmp(rankName((Rank)-1), "?") == 0);
    puts("PASS: isValidScore (ca NaN/Inf), classify bien 0/10, rankName ngoai pham vi");
}

static void testSetters(void) {
    Student s = make("SV001", 5.0f);

    CHECK(setName(&s, "Le Van C") == 1 && strcmp(s.name, "Le Van C") == 0);
    CHECK(setName(&s, "") == 0 && setName(&s, NULL) == 0);
    CHECK(strcmp(s.name, "Le Van C") == 0);            /* that bai → giu nguyen */
    CHECK(setClassName(&s, "CNTT9") == 1 && strcmp(s.className, "CNTT9") == 0);
    CHECK(setClassName(&s, "") == 0 && strcmp(s.className, "CNTT9") == 0);

    CHECK(setScore(&s, 0, 10.0f) == 1);
    CHECK(s.scores[0] == 10.0f && CLOSE(s.avg, 6.0f)); /* tu tinh lai DTB */
    CHECK(setScore(&s, -1, 5.0f) == 0);
    CHECK(setScore(&s, MAX_SUBJECTS, 5.0f) == 0);
    CHECK(setScore(&s, 1, 11.0f) == 0 && setScore(&s, 1, NAN) == 0);
    CHECK(s.scores[1] == 5.0f && CLOSE(s.avg, 6.0f));

    float good[MAX_SUBJECTS] = {1, 2, 3, 4, 5};
    CHECK(setAllScores(&s, good) == 1 && CLOSE(s.avg, 3.0f));
    float bad[MAX_SUBJECTS] = {9, 9, 9, 9, -1};        /* mon cuoi sai */
    CHECK(setAllScores(&s, bad) == 0);
    CHECK(s.scores[0] == 1.0f && CLOSE(s.avg, 3.0f));  /* khong ghi mot phan */
    puts("PASS: setName/setClassName/setScore/setAllScores, that bai khong doi du lieu");
}

/* ===================== list.c ===================== */

static void testAddFind(void) {
    Node *head = NULL;
    CHECK(countNodes(head) == 0 && findById(head, "SV001") == NULL);

    Student a = make("SV001", 9.0f), b = make("SV002", 8.0f), c = make("SV003", 7.0f);
    CHECK(addStudent(&head, &a) == 1);
    CHECK(addStudent(&head, &b) == 1);
    CHECK(addStudent(&head, &c) == 1);
    const char *const order[] = {"SV001", "SV002", "SV003", NULL};
    checkOrder(head, order);                            /* them vao CUOI */
    CHECK(countNodes(head) == 3);

    Student dup = make("SV002", 1.0f);
    CHECK(addStudent(&head, &dup) == 0);               /* trung ma */
    checkOrder(head, order);
    CHECK(CLOSE(findById(head, "SV002")->data.avg, 8.0f));

    CHECK(findById(head, "SV001") == head);
    CHECK(findById(head, "SV003") == head->next->next);
    CHECK(findById(head, "SV999") == NULL);
    CHECK(findById(head, "sv001") == NULL);            /* phan biet hoa thuong: main chuan hoa */

    /* addStudent tu tinh lai DTB du caller truyen avg sai. */
    Student wrong = make("SV004", 6.0f);
    wrong.avg = 99.0f;
    CHECK(addStudent(&head, &wrong) == 1);
    CHECK(CLOSE(findById(head, "SV004")->data.avg, 6.0f));

    /* createNode sao chep, khong tro vao bien goc. */
    a.scores[0] = 0.0f;
    CHECK(head->data.scores[0] == 9.0f);

    freeList(&head);
    CHECK(head == NULL);
    freeList(&head);                                    /* goi lai tren DS rong van an toan */
    puts("PASS: addStudent (cuoi, trung ma, tinh lai DTB), findById, countNodes, freeList");
}

static void testDelete(void) {
    Node *head = NULL;
    const char *ids[] = {"SV001", "SV002", "SV003", "SV004", "SV005"};
    for (int i = 0; i < 5; ++i) {
        Student s = make(ids[i], 5.0f);
        CHECK(addStudent(&head, &s) == 1);
    }
    CHECK(deleteStudent(&head, "SV999") == 0 && countNodes(head) == 5);
    CHECK(deleteStudent(&head, "SV001") == 1);         /* dau */
    const char *const o1[] = {"SV002", "SV003", "SV004", "SV005", NULL};
    checkOrder(head, o1);
    CHECK(deleteStudent(&head, "SV003") == 1);         /* giua */
    const char *const o2[] = {"SV002", "SV004", "SV005", NULL};
    checkOrder(head, o2);
    CHECK(deleteStudent(&head, "SV005") == 1);         /* cuoi */
    const char *const o3[] = {"SV002", "SV004", NULL};
    checkOrder(head, o3);
    CHECK(deleteStudent(&head, "SV005") == 0);         /* xoa lai → khong thay */

    /* Them sau khi xoa cuoi: next cua nut cuoi moi phai la NULL. */
    Student s = make("SV006", 5.0f);
    CHECK(addStudent(&head, &s) == 1);
    const char *const o4[] = {"SV002", "SV004", "SV006", NULL};
    checkOrder(head, o4);

    CHECK(deleteStudent(&head, "SV002") == 1);
    CHECK(deleteStudent(&head, "SV004") == 1);
    CHECK(deleteStudent(&head, "SV006") == 1);
    CHECK(head == NULL);                                /* xoa nut duy nhat */
    CHECK(deleteStudent(&head, "SV006") == 0);         /* xoa tren DS rong */
    puts("PASS: deleteStudent dau/giua/cuoi/khong ton tai/nut duy nhat/DS rong");
}

static void testUpdate(void) {
    Node *head = NULL;
    Student a = make("SV001", 5.0f), b = make("SV002", 5.0f);
    CHECK(addStudent(&head, &a) == 1 && addStudent(&head, &b) == 1);

    float scores[MAX_SUBJECTS] = {10, 10, 10, 10, 5};
    Student data;
    initStudent(&data, "SV002", "Ten Moi", "LopMoi", scores);
    data.avg = -5.0f;                                   /* bi bo qua, tinh lai */
    CHECK(updateStudent(head, &data) == 1);
    Node *p = findById(head, "SV002");
    CHECK(strcmp(p->data.id, "SV002") == 0);
    CHECK(strcmp(p->data.name, "Ten Moi") == 0);
    CHECK(strcmp(p->data.className, "LopMoi") == 0);
    CHECK(CLOSE(p->data.avg, 9.0f));
    CHECK(CLOSE(head->data.avg, 5.0f));                /* SV khac khong doi */

    initStudent(&data, "SV999", "X", "Y", scores);
    CHECK(updateStudent(head, &data) == 0);
    CHECK(countNodes(head) == 2);
    freeList(&head);
    puts("PASS: updateStudent giu ma, tinh lai DTB, khong anh huong SV khac");
}

static void testPrint(void) {
    Node *head = NULL;
    captureBegin(CAPTURE_FILE);
    printList(head);
    printListReverse(head);
    char *out = captureEnd();
    CHECK(strstr(out, "Danh sach rong") != NULL);
    free(out);

    Student a = make("SV001", 9.5f), b = make("SV002", 7.0f), c = make("SV003", 2.0f);
    addStudent(&head, &a);
    addStudent(&head, &b);
    addStudent(&head, &c);

    captureBegin(CAPTURE_FILE);
    printList(head);
    out = captureEnd();
    char *p1 = strstr(out, "SV001"), *p2 = strstr(out, "SV002"), *p3 = strstr(out, "SV003");
    CHECK(p1 && p2 && p3 && p1 < p2 && p2 < p3);
    CHECK(strstr(out, "Tong so: 3 sinh vien") != NULL);
    CHECK(strstr(out, "Xuat sac") && strstr(out, "Kha") && strstr(out, "Kem"));
    free(out);

    captureBegin(CAPTURE_FILE);
    printListReverse(head);
    out = captureEnd();
    p1 = strstr(out, "SV001"); p2 = strstr(out, "SV002"); p3 = strstr(out, "SV003");
    CHECK(p1 && p2 && p3 && p3 < p2 && p2 < p1);       /* nguoc thu tu */
    CHECK(strstr(out, "3    SV003") != NULL);          /* STT theo vi tri goc */
    CHECK(strstr(out, "1    SV001") != NULL);
    free(out);

    /* Ten dai hon cot 25 ky tu: bi cat khi in bang, in chi tiet thi day du. */
    Student longName = make("SV004", 5.0f);
    setName(&longName, "Nguyen Thi Phuong Thao Hoang Ngoc Bich");
    captureBegin(CAPTURE_FILE);
    printStudentRow(4, &longName);
    printStudentDetail(&longName);
    out = captureEnd();
    CHECK(strstr(out, "Nguyen Thi Phuong Thao Ho ") != NULL);
    CHECK(strstr(out, "Nguyen Thi Phuong Thao Hoang Ngoc Bich") != NULL);
    free(out);

    freeList(&head);
    puts("PASS: printList / printListReverse (thu tu, STT, DS rong), cat ten dai");
}

/* Ham de quy (countNodes, findById, printReverseRec, mergeLists, sumAvg) co do sau
 * bang so nut → kiem tra voi danh sach lon hon nhieu so voi thuc te. */
#define BIG 10000
static void testLargeList(void) {
    Node *head = NULL;
    char id[MAX_ID];
    for (int i = BIG; i >= 1; --i) {                    /* them nguoc de sort co viec lam */
        snprintf(id, sizeof id, "SV%05d", i);
        Student s = make(id, (float)(i % 11));
        CHECK(addStudent(&head, &s) == 1);
    }
    CHECK(countNodes(head) == BIG);
    CHECK(findById(head, "SV00001") != NULL);           /* nut cuoi: de quy sau nhat */

    mergeSort(&head, cmpById);
    CHECK(strcmp(head->data.id, "SV00001") == 0);
    int n = 0;
    for (const Node *p = head; p->next != NULL; p = p->next, ++n)
        CHECK(strcmp(p->data.id, p->next->data.id) < 0);
    CHECK(n == BIG - 1);

    float avg;
    CHECK(overallAverage(head, &avg) == 1 && avg >= 0.0f && avg <= 10.0f);

    captureBegin(CAPTURE_FILE);
    printListReverse(head);
    char *out = captureEnd();
    CHECK(strstr(out, "SV10000") < strstr(out, "SV00001"));
    free(out);

    freeList(&head);
    CHECK(head == NULL);
    printf("PASS: danh sach %d nut (de quy count/find/sort/sum/in nguoc)\n", BIG);
}

int main(void) {
    testInitStudent();
    testScoresAndRank();
    testSetters();
    testAddFind();
    testDelete();
    testUpdate();
    testPrint();
    testLargeList();
    puts("All student/list checks passed.");
    return 0;
}
