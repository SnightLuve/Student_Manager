/*
 * config.h — Hằng số, macro và cấu hình tiền xử lý dùng chung toàn chương trình.
 * Phụ trách: TV1
 */
#ifndef CONFIG_H
#define CONFIG_H

#define MAX_ID        12   /* Mã SV tối đa 11 ký tự + '\0'          */
#define MAX_NAME      51   /* Họ tên tối đa 50 ký tự + '\0'         */
#define MAX_CLASS     21   /* Tên lớp tối đa 20 ký tự + '\0'        */
#define MAX_SUBJECTS  5    /* Số môn học cố định                     */
#define MAX_LINE      256  /* Bộ đệm nhập một dòng                   */

#define MIN_SCORE     0.0f
#define MAX_SCORE     10.0f

#define DEFAULT_DATA_FILE "data/students.dat"
/* CMake truyen duong dan file mau trong source. */
#ifndef DEFAULT_SAMPLE_FILE
#define DEFAULT_SAMPLE_FILE "data/sample_students.dat"
#endif
#define FILE_MAGIC        0x53544D47u  /* "STMG" — nhận dạng file   */
#define FILE_VERSION      1

/* Bật bằng: cmake -DENABLE_DEBUG=ON  hoặc  gcc -DDEBUG */
#ifdef DEBUG
  #include <stdio.h>
  #define LOG(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
  #define LOG(fmt, ...) ((void)0)
#endif

#endif /* CONFIG_H */
