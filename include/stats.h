/*
 * stats.h — Thống kê danh sách sinh viên: ĐTB toàn khóa, theo xếp loại,
 * cao nhất/thấp nhất, theo lớp, theo môn.
 * Phụ trách: TV2
 *
 * Hàm compute/find/count chỉ tính và trả số liệu (không in);
 * hàm show/statsByClass in kết quả ra màn hình.
 */
#ifndef STATS_H
#define STATS_H

#include "list.h"

#define MAX_CLASS_STATS 50   /* Số lớp tối đa trong bảng thống kê theo lớp */

/* Số liệu một lớp. */
typedef struct {
    char  className[MAX_CLASS];
    int   count;          /* Sĩ số                      */
    float avg;            /* ĐTB lớp                    */
    int   goodOrAbove;    /* Số SV xếp loại Giỏi trở lên */
} ClassStat;

/* ---------- Tính toán ---------- */

/* Tổng ĐTB của mọi SV (ĐỆ QUY). Danh sách rỗng → 0. */
float sumAvg(const Node *head);

/* ĐTB toàn khóa = sumAvg / countNodes, ghi vào *out.
 * Trả về 1 thành công, 0 nếu danh sách rỗng (không chia cho 0, *out giữ nguyên). */
int overallAverage(const Node *head, float *out);

/* Đếm số SV theo từng mức xếp loại vào rankCount[RANK_COUNT] (đặt 0 trước khi đếm). */
void countByRank(const Node *head, int rankCount[RANK_COUNT]);

/* Tìm SV có ĐTB cao nhất (*best) và thấp nhất (*worst); bằng điểm giữ SV gặp trước.
 * Trả về 1 thành công, 0 nếu danh sách rỗng. */
int findTopAndBottom(const Node *head, const Student **best, const Student **worst);

/* ĐTB từng môn ghi vào out[MAX_SUBJECTS].
 * Trả về 1 thành công, 0 nếu danh sách rỗng (out giữ nguyên). */
int subjectAverages(const Node *head, float out[MAX_SUBJECTS]);

/* Thống kê theo lớp, theo thứ tự lớp xuất hiện đầu tiên trong danh sách.
 * Ghi tối đa maxClasses lớp vào out; trả về số lớp đã ghi. */
int computeClassStats(const Node *head, ClassStat out[], int maxClasses);

/* ---------- In kết quả (danh sách rỗng → "Danh sach rong.") ---------- */

/* In số lượng, tỉ lệ % từng mức xếp loại và ĐTB toàn khóa. */
void showRankStats(const Node *head);

/* In SV có ĐTB cao nhất và thấp nhất. */
void showTopAndBottom(const Node *head);

/* In bảng: Lop | Si so | DTB lop | So SV Gioi tro len. */
void statsByClass(const Node *head);

/* In ĐTB từng môn. */
void showSubjectAverages(const Node *head);

#endif /* STATS_H */
