/* Tao du lieu mau bang chinh module TV3. Chay tu thu muc goc du an.
 * Khong truyen path: dung data/students.dat va data/report.csv. */
#include <stdio.h>
#include "file_io.h"

int main(int argc, char **argv) {
    const char *names[] = {
        "Nguyen Van An", "Tran Thi Binh", "Le Minh Chau",
        "Pham Quoc Dung", "Hoang Thu Ha", "Vu Tuan Hung",
        "Dang Ngoc Lan", "Bui Duc Long", "Do Thi Mai",
        "Ngo Thanh Nam", "Duong Hong Nhung", "Ly Minh Phuc",
        "Trinh Anh Quan", "Dinh Thanh Son", "Mai Phuong Thao",
        "Phan Bao Tram", "Cao Quang Vinh", "Ta Hai Yen"
    };
    const char *classes[] = {"CNTT1", "CNTT2", "CNTT3"};
    const float levels[] = {9.5f, 8.5f, 7.0f, 5.5f, 4.0f, 2.5f};
    if (argc > 3) {
        fprintf(stderr, "Usage: %s [students.dat [report.csv]]\n", argv[0]);
        return 1;
    }
    Node *head = NULL;
    for (int i = 0; i < 18; ++i) {
        char id[MAX_ID];
        float scores[MAX_SUBJECTS];
        for (int j = 0; j < MAX_SUBJECTS; ++j)
            scores[j] = levels[i % 6] + (j - 2) * 0.2f;
        snprintf(id, sizeof id, "SV%03d", i + 1);
        Student s;
        initStudent(&s, id, names[i], classes[i / 6], scores);
        if (addStudent(&head, &s) != 1) {
            freeList(&head);
            return 1;
        }
    }
    int ok = saveToFile(argc > 1 ? argv[1] : DEFAULT_DATA_FILE, head)
             && exportCsv(argc > 2 ? argv[2] : "data/report.csv", head);
    freeList(&head);
    if (!ok) {
        fprintf(stderr, "Khong ghi duoc file mau. Kiem tra thu muc cha/quyen ghi.\n");
        return 1;
    }
    puts("Da tao 18 sinh vien, 3 lop, du 6 muc xep loai (DAT + CSV).");
    return 0;
}
