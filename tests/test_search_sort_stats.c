/* Kiem thu module TV2 (search_sort + stats) tren du lieu mau 18 SV / 3 lop.
 * Cach chay: search_sort_stats_tests <duong_dan_students.dat> */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "search_sort.h"
#include "stats.h"

/* Dung CHECK thay assert de test van chay o Release/NDEBUG. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#define SAMPLE_COUNT 18
#define CLOSE(x, y) (fabsf((x) - (y)) < 1e-3f)

static const char *samplePath;
static const float CLASS_AVG = 37.0f / 6.0f;   /* (9.5+8.5+7+5.5+4+2.5)/6 */

static Node *loadSample(void) {
    Node *head = NULL;
    CHECK(loadFromFile(samplePath, &head) == SAMPLE_COUNT);
    return head;
}

static Student makeStudent(const char *id, const char *name, const char *cls, float score) {
    Student s;
    float scores[MAX_SUBJECTS];
    for (int i = 0; i < MAX_SUBJECTS; ++i) scores[i] = score;
    initStudent(&s, id, name, cls, scores);
    return s;
}

/* Danh sach da sap xep dung theo cmp, du 18 nut, du ma SV001..SV018. */
static void checkSorted(const Node *head, CompareFn cmp) {
    int seen[SAMPLE_COUNT] = {0};
    int n = 0;
    for (const Node *p = head; p != NULL; p = p->next, ++n) {
        if (p->next != NULL) CHECK(cmp(&p->data, &p->next->data) <= 0);
        int number = atoi(p->data.id + 2);
        CHECK(strncmp(p->data.id, "SV", 2) == 0 && number >= 1 && number <= SAMPLE_COUNT);
        CHECK(seen[number - 1] == 0);
        seen[number - 1] = 1;
    }
    CHECK(n == SAMPLE_COUNT);
}

/* T7: tim theo ma (de quy, list.c), theo ten, theo lop, loc khoang diem. */
static void testSearch(void) {
    Node *head = loadSample();
    CHECK(findById(head, "SV005") != NULL);
    CHECK(strcmp(findById(head, "SV005")->data.name, "Hoang Thu Ha") == 0);
    CHECK(findById(head, "SV999") == NULL);

    CHECK(containsIgnoreCase("Nguyen Van An", "van"));
    CHECK(containsIgnoreCase("Nguyen Van An", "AN"));
    CHECK(containsIgnoreCase("abc", ""));
    CHECK(!containsIgnoreCase("abc", "abcd"));
    CHECK(!containsIgnoreCase("Le Minh Chau", "an"));
    CHECK(!containsIgnoreCase(NULL, "a"));

    /* An, Tran, Hoang, Tuan, Lan, Thanh(x2), Anh/Quan, Phan, Quang */
    CHECK(searchByName(head, "an") == 10);
    CHECK(searchByName(head, "AN") == 10);
    CHECK(searchByName(head, "khong co ai") == 0);
    CHECK(searchByClass(head, "cntt2") == 6);
    CHECK(searchByClass(head, "CNTT") == SAMPLE_COUNT);
    CHECK(searchByClass(head, "KTPM") == 0);

    CHECK(filterByAvgRange(head, 8.0f, 10.0f) == 6);
    CHECK(filterByAvgRange(head, 10.0f, 8.0f) == 6);    /* dao bien */
    CHECK(filterByAvgRange(head, 2.5f, 2.5f) == 3);     /* bien dung bang */
    CHECK(filterByAvgRange(head, 0.0f, 10.0f) == SAMPLE_COUNT);
    CHECK(filterByAvgRange(head, 9.6f, 10.0f) == 0);
    freeList(&head);
    puts("PASS: T7 tim kiem theo ma, ten, lop, khoang diem");
}

/* T10: danh sach rong khong crash, khong chia cho 0. */
static void testEmptyList(void) {
    Node *head = NULL;
    CompareFn criteria[] = {cmpById, cmpByName, cmpByAvgDesc, cmpByClassThenAvg};
    for (int i = 0; i < 4; ++i) {
        mergeSort(&head, criteria[i]);
        CHECK(head == NULL);
    }
    CHECK(sumAvg(head) == 0.0f);
    float avg = -1.0f;
    CHECK(overallAverage(head, &avg) == 0 && avg == -1.0f);
    int rankCount[RANK_COUNT];
    for (int i = 0; i < RANK_COUNT; ++i) rankCount[i] = -1;
    countByRank(head, rankCount);
    for (int i = 0; i < RANK_COUNT; ++i) CHECK(rankCount[i] == 0);
    const Student *best = NULL, *worst = NULL;
    CHECK(findTopAndBottom(head, &best, &worst) == 0 && best == NULL && worst == NULL);
    float subjects[MAX_SUBJECTS];
    CHECK(subjectAverages(head, subjects) == 0);
    ClassStat stats[MAX_CLASS_STATS];
    CHECK(computeClassStats(head, stats, MAX_CLASS_STATS) == 0);
    CHECK(searchByName(head, "an") == 0);
    CHECK(searchByClass(head, "") == 0);
    CHECK(filterByAvgRange(head, 0.0f, 10.0f) == 0);
    showRankStats(head);
    showTopAndBottom(head);
    statsByClass(head);
    showSubjectAverages(head);
    puts("PASS: T10 danh sach rong");
}

/* T11: sap xep theo 4 tieu chi, danh sach 1 phan tu, khong mat phan tu. */
static void testSort(void) {
    CompareFn criteria[] = {cmpById, cmpByName, cmpByAvgDesc, cmpByClassThenAvg};
    Node *head = loadSample();
    for (int i = 0; i < 4; ++i) {
        mergeSort(&head, criteria[i]);
        checkSorted(head, criteria[i]);
    }
    /* Sau cmpByClassThenAvg: 6 SV dau la CNTT1, DTB giam dan. */
    const Node *p = head;
    for (int i = 0; i < 6; ++i, p = p->next) CHECK(strcmp(p->data.className, "CNTT1") == 0);
    CHECK(strcmp(p->data.className, "CNTT2") == 0);
    CHECK(CLOSE(head->data.avg, 9.5f));

    mergeSort(&head, cmpByAvgDesc);
    CHECK(CLOSE(head->data.avg, 9.5f));
    for (p = head; p->next != NULL; p = p->next) {}
    CHECK(CLOSE(p->data.avg, 2.5f));

    mergeSort(&head, cmpByName);   /* ten: An < Binh < Chau ... < Yen */
    CHECK(strcmp(head->data.name, "Nguyen Van An") == 0);
    CHECK(strcmp(head->next->data.name, "Tran Thi Binh") == 0);

    mergeSort(&head, cmpById);
    int number = 1;
    for (p = head; p != NULL; p = p->next, ++number) {
        char id[16];   /* du cho "SV" + so int bat ky → khong canh bao truncation */
        snprintf(id, sizeof id, "SV%03d", number);
        CHECK(strcmp(p->data.id, id) == 0);
    }
    freeList(&head);

    /* Cung ten, khac ho dem: so ho dem; trung ten khong phan biet hoa thuong. */
    Student a = makeStudent("SV100", "Tran Van An", "X", 5.0f);
    Student b = makeStudent("SV101", "Nguyen Thi An", "X", 5.0f);
    Student c = makeStudent("SV102", "le binh", "X", 5.0f);
    CHECK(cmpByName(&b, &a) < 0 && cmpByName(&a, &b) > 0);
    CHECK(cmpByName(&a, &c) < 0);                 /* An < binh */
    CHECK(cmpByName(&a, &a) == 0);

    /* Danh sach 1 phan tu giu nguyen. */
    Node *single = NULL;
    CHECK(addStudent(&single, &a) == 1);
    Node *only = single;
    for (int i = 0; i < 4; ++i) {
        mergeSort(&single, criteria[i]);
        CHECK(single == only && single->next == NULL);
    }
    freeList(&single);
    puts("PASS: T11 sap xep 4 tieu chi, danh sach 1 phan tu");
}

/* T12: bien xep loai 9.0, 8.0, 6.5, 5.0, 3.5 va ngay duoi bien. */
static void testRankBoundaries(void) {
    const float bounds[] = {9.0f, 8.0f, 6.5f, 5.0f, 3.5f};
    for (int r = 0; r < 5; ++r) {
        Student s = makeStudent("SV200", "Bien Xep Loai", "X", bounds[r]);
        CHECK(classify(s.avg) == (Rank)r);
        Student below = makeStudent("SV201", "Duoi Bien", "X", bounds[r] - 0.01f);
        CHECK(classify(below.avg) == (Rank)(r + 1));
    }
    float scores[MAX_SUBJECTS] = {8.0f, 8.0f, 8.0f, 8.0f, 7.95f};   /* DTB 7.99 */
    Student s;
    initStudent(&s, "SV202", "Gan Gioi", "X", scores);
    CHECK(classify(s.avg) == RANK_FAIR);

    Node *head = loadSample();
    int rankCount[RANK_COUNT];
    countByRank(head, rankCount);
    for (int r = 0; r < RANK_COUNT; ++r) CHECK(rankCount[r] == 3);
    freeList(&head);
    puts("PASS: T12 bien xep loai va dem theo xep loai");
}

/* Thong ke tren du lieu mau. */
static void testStats(void) {
    Node *head = loadSample();
    CHECK(CLOSE(sumAvg(head), CLASS_AVG * SAMPLE_COUNT));
    float avg = 0.0f;
    CHECK(overallAverage(head, &avg) == 1 && CLOSE(avg, CLASS_AVG));

    const Student *best = NULL, *worst = NULL;
    CHECK(findTopAndBottom(head, &best, &worst) == 1);
    CHECK(strcmp(best->id, "SV001") == 0 && CLOSE(best->avg, 9.5f));
    CHECK(strcmp(worst->id, "SV006") == 0 && CLOSE(worst->avg, 2.5f));

    ClassStat stats[MAX_CLASS_STATS];
    CHECK(computeClassStats(head, stats, MAX_CLASS_STATS) == 3);
    const char *classes[] = {"CNTT1", "CNTT2", "CNTT3"};
    for (int k = 0; k < 3; ++k) {
        CHECK(strcmp(stats[k].className, classes[k]) == 0);
        CHECK(stats[k].count == 6);
        CHECK(CLOSE(stats[k].avg, CLASS_AVG));
        CHECK(stats[k].goodOrAbove == 2);
    }
    CHECK(computeClassStats(head, stats, 2) == 2);    /* vuot gioi han khong tran mang */

    float subjects[MAX_SUBJECTS];
    CHECK(subjectAverages(head, subjects) == 1);
    for (int j = 0; j < MAX_SUBJECTS; ++j)
        CHECK(CLOSE(subjects[j], CLASS_AVG + (float)(j - 2) * 0.2f));

    showRankStats(head);
    showTopAndBottom(head);
    statsByClass(head);
    showSubjectAverages(head);
    freeList(&head);
    puts("PASS: thong ke DTB, cao/thap nhat, theo lop, theo mon");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Cach dung: %s <students.dat>\n", argv[0]);
        return EXIT_FAILURE;
    }
    samplePath = argv[1];
    testSearch();
    testEmptyList();
    testSort();
    testRankBoundaries();
    testStats();
    puts("All search/sort/stats checks passed.");
    return 0;
}
