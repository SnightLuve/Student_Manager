/*
 * student.c — Cài đặt các hàm xử lý một sinh viên.
 * Phụ trách: TV1
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "student.h"

const char *SUBJECT_NAMES[MAX_SUBJECTS] = {
    "Toan cao cap", "Vat ly", "Lap trinh C", "Tieng Anh", "Triet hoc"
};

/* Sao chép chuỗi an toàn: luôn kết thúc bằng '\0', cắt nếu quá dài. */
static void copyString(char *dst, const char *src, size_t size) {
    if (src == NULL) src = "";
    strncpy(dst, src, size - 1);
    dst[size - 1] = '\0';
}

void normalizeId(char *id) {
    for (; *id != '\0'; id++)
        *id = (char)toupper((unsigned char)*id);
}

int isValidScore(float score) {
    return score >= MIN_SCORE && score <= MAX_SCORE;
}

void initStudent(Student *s, const char *id, const char *name,
                 const char *className, const float scores[]) {
    memset(s, 0, sizeof *s);   /* xóa sạch padding → file nhị phân ổn định */
    copyString(s->id, id, sizeof s->id);
    normalizeId(s->id);
    copyString(s->name, name, sizeof s->name);
    copyString(s->className, className, sizeof s->className);
    for (int i = 0; i < MAX_SUBJECTS; i++)
        s->scores[i] = (scores != NULL) ? scores[i] : 0.0f;
    computeAverage(s);
}

/* Truyền con trỏ để hàm cập nhật trực tiếp trường avg của sinh viên. */
void computeAverage(Student *s) {
    float sum = 0.0f;
    for (int i = 0; i < MAX_SUBJECTS; i++)     /* duyệt mảng điểm — Ch.1 */
        sum += s->scores[i];
    s->avg = sum / MAX_SUBJECTS;
}

Rank classify(float avg) {
    /* Sai số nhỏ: ĐTB tính bằng float có thể ra 7.9999 thay vì 8.0
     * → không cộng eps sẽ bị xếp nhầm xuống mức dưới (kiểm thử T12). */
    const float eps = 1e-4f;
    if (avg + eps >= 9.0f) return RANK_EXCELLENT;
    if (avg + eps >= 8.0f) return RANK_GOOD;
    if (avg + eps >= 6.5f) return RANK_FAIR;
    if (avg + eps >= 5.0f) return RANK_AVERAGE;
    if (avg + eps >= 3.5f) return RANK_WEAK;
    return RANK_POOR;
}

const char *rankName(Rank r) {
    static const char *names[RANK_COUNT] = {
        "Xuat sac", "Gioi", "Kha", "Trung binh", "Yeu", "Kem"
    };
    if (r < 0 || r >= RANK_COUNT) return "?";
    return names[r];
}

int setName(Student *s, const char *name) {
    if (name == NULL || name[0] == '\0') return 0;
    copyString(s->name, name, sizeof s->name);
    return 1;
}

int setClassName(Student *s, const char *className) {
    if (className == NULL || className[0] == '\0') return 0;
    copyString(s->className, className, sizeof s->className);
    return 1;
}

int setScore(Student *s, int subjectIndex, float score) {
    if (subjectIndex < 0 || subjectIndex >= MAX_SUBJECTS || !isValidScore(score))
        return 0;
    s->scores[subjectIndex] = score;
    computeAverage(s);           /* sửa điểm → bắt buộc tính lại ĐTB */
    return 1;
}

int setAllScores(Student *s, const float scores[]) {
    for (int i = 0; i < MAX_SUBJECTS; i++)      /* kiểm tra hết trước khi ghi */
        if (!isValidScore(scores[i])) return 0;
    for (int i = 0; i < MAX_SUBJECTS; i++)
        s->scores[i] = scores[i];
    computeAverage(s);
    return 1;
}

void printTableHeader(void) {
    printf("%-4s %-11s %-25s %-10s", "STT", "Ma SV", "Ho ten", "Lop");
    for (int i = 0; i < MAX_SUBJECTS; i++)
        printf("   M%-2d", i + 1);           /* cột điểm môn i: M1, M2, ... */
    printf(" %5s  %-10s\n", "DTB", "Xep loai");
    for (int i = 0; i < 4 + 1 + 11 + 1 + 25 + 1 + 10 + MAX_SUBJECTS * 6 + 6 + 2 + 10; i++)
        putchar('-');
    putchar('\n');
}

void printStudentRow(int stt, const Student *s) {
    if (stt > 0) printf("%-4d ", stt);
    else         printf("%-4s ", "-");
    printf("%-11s %-25.25s %-10.10s", s->id, s->name, s->className);
    for (int i = 0; i < MAX_SUBJECTS; i++)
        printf(" %5.2f", s->scores[i]);
    printf(" %5.2f  %-10s\n", s->avg, rankName(classify(s->avg)));
}

void printStudentDetail(const Student *s) {
    printf("  %-13s: %s\n", "Ma SV", s->id);
    printf("  %-13s: %s\n", "Ho ten", s->name);
    printf("  %-13s: %s\n", "Lop", s->className);
    for (int i = 0; i < MAX_SUBJECTS; i++)
        printf("  %-13s: %.2f\n", SUBJECT_NAMES[i], s->scores[i]);
    printf("  %-13s: %.2f\n", "Diem TB", s->avg);
    printf("  %-13s: %s\n", "Xep loai", rankName(classify(s->avg)));
}
