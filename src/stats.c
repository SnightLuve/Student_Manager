/*
 * stats.c — Cài đặt các hàm thống kê.
 * Phụ trách: TV2
 */
#include <stdio.h>
#include <string.h>

#include "stats.h"

/* ===================== Tính toán ===================== */

/* ĐỆ QUY — dừng khi hết danh sách; bước đệ quy: ĐTB nút này + tổng phần sau. */
float sumAvg(const Node *head) {
    if (head == NULL) return 0.0f;
    return head->data.avg + sumAvg(head->next);
}

int overallAverage(const Node *head, float *out) {
    int n = countNodes(head);
    if (n == 0) return 0;                       /* không chia cho 0 */
    *out = sumAvg(head) / (float)n;
    return 1;
}

void countByRank(const Node *head, int rankCount[RANK_COUNT]) {
    for (int i = 0; i < RANK_COUNT; i++) rankCount[i] = 0;
    for (const Node *p = head; p != NULL; p = p->next)
        rankCount[classify(p->data.avg)]++;     /* Rank dùng làm chỉ số mảng */
}

int findTopAndBottom(const Node *head, const Student **best, const Student **worst) {
    if (head == NULL) return 0;
    const Student *hi = &head->data, *lo = &head->data;
    for (const Node *p = head->next; p != NULL; p = p->next) {
        if (p->data.avg > hi->avg) hi = &p->data;   /* > : giữ SV gặp trước khi bằng */
        if (p->data.avg < lo->avg) lo = &p->data;
    }
    *best = hi;
    *worst = lo;
    return 1;
}

int subjectAverages(const Node *head, float out[MAX_SUBJECTS]) {
    float subjectSum[MAX_SUBJECTS] = {0};
    int n = 0;
    for (const Node *p = head; p != NULL; p = p->next, n++)
        for (int i = 0; i < MAX_SUBJECTS; i++)
            subjectSum[i] += p->data.scores[i];
    if (n == 0) return 0;
    for (int i = 0; i < MAX_SUBJECTS; i++)
        out[i] = subjectSum[i] / (float)n;
    return 1;
}

int computeClassStats(const Node *head, ClassStat out[], int maxClasses) {
    int used = 0;
    for (const Node *p = head; p != NULL; p = p->next) {
        int k = 0;
        while (k < used && strcmp(out[k].className, p->data.className) != 0) k++;
        if (k == used) {                        /* lớp mới */
            if (used >= maxClasses) {
                LOG("computeClassStats: vuot qua %d lop, bo qua %s",
                    maxClasses, p->data.className);
                continue;
            }
            memcpy(out[k].className, p->data.className, sizeof out[k].className);
            out[k].count = 0;
            out[k].avg = 0.0f;                  /* tạm dùng làm tổng ĐTB */
            out[k].goodOrAbove = 0;
            used++;
        }
        out[k].count++;
        out[k].avg += p->data.avg;
        if (classify(p->data.avg) <= RANK_GOOD) out[k].goodOrAbove++;
    }
    for (int k = 0; k < used; k++)
        out[k].avg /= (float)out[k].count;      /* count >= 1 với mọi lớp đã ghi */
    return used;
}

/* ===================== In kết quả ===================== */

void showRankStats(const Node *head) {
    int n = countNodes(head);
    if (n == 0) {
        printf("  Danh sach rong.\n");
        return;
    }
    int rankCount[RANK_COUNT];
    countByRank(head, rankCount);
    printf("  %-12s %8s %8s\n", "Xep loai", "So SV", "Ti le");
    for (int r = 0; r < RANK_COUNT; r++)
        printf("  %-12s %8d %7.2f%%\n", rankName((Rank)r), rankCount[r],
               100.0f * (float)rankCount[r] / (float)n);
    float avg;
    if (overallAverage(head, &avg))
        printf("  DTB toan khoa: %.2f (%d sinh vien)\n", avg, n);
}

void showTopAndBottom(const Node *head) {
    const Student *best, *worst;
    if (!findTopAndBottom(head, &best, &worst)) {
        printf("  Danh sach rong.\n");
        return;
    }
    printf("  Sinh vien co DTB cao nhat:\n");
    printTableHeader();
    printStudentRow(0, best);
    printf("  Sinh vien co DTB thap nhat:\n");
    printTableHeader();
    printStudentRow(0, worst);
}

void statsByClass(const Node *head) {
    ClassStat stats[MAX_CLASS_STATS];
    int used = computeClassStats(head, stats, MAX_CLASS_STATS);
    if (used == 0) {
        printf("  Danh sach rong.\n");
        return;
    }
    printf("  %-20s %7s %8s %20s\n", "Lop", "Si so", "DTB lop", "So SV Gioi tro len");
    int counted = 0;
    for (int k = 0; k < used; k++) {
        printf("  %-20s %7d %8.2f %20d\n", stats[k].className, stats[k].count,
               stats[k].avg, stats[k].goodOrAbove);
        counted += stats[k].count;
    }
    /* computeClassStats bỏ qua lớp vượt MAX_CLASS_STATS → báo, không im lặng. */
    int total = countNodes(head);
    if (counted < total)
        printf("  ! Chi hien thi %d lop dau tien; %d sinh vien thuoc cac lop khac "
               "chua duoc thong ke.\n", used, total - counted);
}

void showSubjectAverages(const Node *head) {
    float avg[MAX_SUBJECTS];
    if (!subjectAverages(head, avg)) {
        printf("  Danh sach rong.\n");
        return;
    }
    printf("  %-15s %8s\n", "Mon hoc", "DTB mon");
    for (int i = 0; i < MAX_SUBJECTS; i++)
        printf("  %-15s %8.2f\n", SUBJECT_NAMES[i], avg[i]);
}
