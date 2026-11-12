/*
 * student.h — Kiểu dữ liệu Student và các hàm xử lý MỘT sinh viên
 * (tính ĐTB, xếp loại, cập nhật trường, in thông tin).
 * Phụ trách: TV1
 */
#ifndef STUDENT_H
#define STUDENT_H

#include "config.h"

/* Dữ liệu một sinh viên.
 * KHÔNG chứa con trỏ → có thể fwrite/fread nguyên struct vào file nhị phân. */
typedef struct {
    char  id[MAX_ID];               /* Mã SV — khóa, duy nhất (in hoa)  */
    char  name[MAX_NAME];           /* Họ tên                           */
    char  className[MAX_CLASS];     /* Lớp                              */
    float scores[MAX_SUBJECTS];     /* Điểm các môn (mảng — Ch.1)       */
    float avg;                      /* Điểm trung bình (tự tính)        */
} Student;

/* Xếp loại học lực — vừa để in tên, vừa làm chỉ số mảng đếm khi thống kê. */
typedef enum {
    RANK_EXCELLENT,   /* Xuất sắc   : 9.0 – 10    */
    RANK_GOOD,        /* Giỏi       : 8.0 – < 9.0 */
    RANK_FAIR,        /* Khá        : 6.5 – < 8.0 */
    RANK_AVERAGE,     /* Trung bình : 5.0 – < 6.5 */
    RANK_WEAK,        /* Yếu        : 3.5 – < 5.0 */
    RANK_POOR,        /* Kém        : < 3.5       */
    RANK_COUNT        /* Số lượng mức xếp loại    */
} Rank;

/* Tên các môn học — định nghĩa trong student.c */
extern const char *SUBJECT_NAMES[MAX_SUBJECTS];

/* ---------- Khởi tạo / chuẩn hóa ---------- */

/* Khởi tạo một Student từ các giá trị cho trước (cắt chuỗi nếu quá dài,
 * chuẩn hóa mã về in hoa) và tự tính ĐTB. scores có thể NULL → mọi điểm = 0. */
void initStudent(Student *s, const char *id, const char *name,
                 const char *className, const float scores[]);

/* Chuyển mã SV về chữ in hoa để "sv001" và "SV001" là một. */
void normalizeId(char *id);

/* Kiểm tra điểm có nằm trong [MIN_SCORE, MAX_SCORE]. Trả về 1 nếu hợp lệ. */
int isValidScore(float score);

/* ---------- ĐTB và xếp loại ---------- */

/* Tính lại ĐTB các môn và ghi vào s->avg (tham số con trỏ). */
void computeAverage(Student *s);

/* Trả về mức xếp loại tương ứng với ĐTB. */
Rank classify(float avg);

/* Trả về tên (không dấu) của mức xếp loại. */
const char *rankName(Rank r);

/* ---------- Sửa từng trường (dùng cho chức năng "Sửa") ---------- */
/* Các hàm trả về 1 nếu thành công, 0 nếu dữ liệu không hợp lệ.          */
/* Mã SV là khóa nên KHÔNG có hàm sửa mã.                                */

int setName(Student *s, const char *name);
int setClassName(Student *s, const char *className);
int setScore(Student *s, int subjectIndex, float score);      /* tự tính lại ĐTB */
int setAllScores(Student *s, const float scores[]);           /* tự tính lại ĐTB */

/* ---------- In ---------- */

void printTableHeader(void);
void printStudentRow(int stt, const Student *s);   /* stt <= 0 → in "-" */
void printStudentDetail(const Student *s);

#endif /* STUDENT_H */
