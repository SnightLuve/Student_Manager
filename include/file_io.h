/* file_io.h - Doc/ghi du lieu, phu trach: TV3. */
#ifndef FILE_IO_H
#define FILE_IO_H

#include "list.h"

/* Ghi header + cac Student (khong ghi Node/con tro).
 * Tra ve 1 thanh cong, 0 khi loi. Chi thay file dich sau khi ghi xong file tam.
 * Thu muc cha phai ton tai; khong dung dong thoi cung mot path. */
int saveToFile(const char *path, const Node *head);

/* Them du lieu doc duoc vao CUOI *head, giu nguyen thu tu trong file.
 * Tra ve so SV da doc; 0 neu file chua ton tai hoac file rong hop le;
 * -1 neu loi I/O, sai dinh dang, du lieu sai/trung ma, hoac het bo nho.
 * Neu loi, *head giu nguyen. Ma duoc chuan hoa, DTB duoc tinh lai tu diem. */
int loadFromFile(const char *path, Node **head);

/* Doc truc tiep ban ghi index (bat dau tu 0) bang fseek.
 * Tra ve 1 thanh cong, 0 khi loi/ngoai pham vi; khi loi khong thay doi *out.
 * DTB duoc tinh lai, ma duoc chuan hoa nhu loadFromFile. */
int readRecordAt(const char *path, int index, Student *out);

/* Xuat CSV dau phay, dong CRLF, chuoi duoc escape khi can; DTB 2 chu so le.
 * Tra ve 1 thanh cong, 0 khi loi. Chi xuat CSV, khong nhap CSV.
 * Cung quy tac file tam va thu muc cha nhu saveToFile. */
int exportCsv(const char *path, const Node *head);

#endif /* FILE_IO_H */
