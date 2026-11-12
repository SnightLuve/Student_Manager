/* file_io.c - File nhi phan, CSV, truy cap ngau nhien. Phu trach: TV3. */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#endif

#include "file_io.h"

/* Dung nguyen dinh dang version 1 trong system design (README muc 7). */
typedef struct {
    unsigned int magic;
    int version;
    int count;
} FileHeader;

/* Chuoi tu file phai co ky tu ket thuc trong gioi han va khong chi co space. */
static int validText(const char *text, size_t size) {
    const char *end = memchr(text, '\0', size);
    if (end == NULL) return 0;
    for (const char *p = text; p < end; ++p)
        if (!isspace((unsigned char)*p)) return 1;
    return 0;
}

/* Kiem tra truoc khi goi cac ham xu ly chuoi cua module student/list. */
static int validStudent(const Student *s) {
    if (!validText(s->id, sizeof s->id)
        || !validText(s->name, sizeof s->name)
        || !validText(s->className, sizeof s->className)) return 0;
    for (const char *p = s->id; *p; ++p)
        if (isspace((unsigned char)*p)) return 0;
    for (int i = 0; i < MAX_SUBJECTS; ++i)
        if (!isfinite(s->scores[i]) || !isValidScore(s->scores[i])) return 0;
    /* avg la du lieu suy ra, khong tin gia tri luu trong file. */
    return 1;
}

/* Tao ban sao sach padding, chuan hoa ma va tinh lai DTB. */
static void canonicalStudent(Student *out, const Student *in) {
    initStudent(out, in->id, in->name, in->className, in->scores);
}

/* Kiem tra ca kich thuoc file: tu choi header thieu, count sai, byte du/thieu.
 * Gioi han theo long vi fseek/ftell cua C su dung long. */
static int readHeader(FILE *file, FileHeader *header) {
    if (fread(header, sizeof *header, 1, file) != 1
        || header->magic != FILE_MAGIC || header->version != FILE_VERSION
        || header->count < 0) return 0;
    if ((unsigned long)header->count >
        ((unsigned long)LONG_MAX - sizeof *header) / sizeof(Student)) return 0;
    long expected = (long)sizeof *header + (long)header->count * (long)sizeof(Student);
    if (fseek(file, 0, SEEK_END) != 0 || ftell(file) != expected) return 0;
    return fseek(file, (long)sizeof *header, SEEK_SET) == 0;
}

/* Dem, kiem tra du lieu va ma trung sau chuan hoa truoc khi xuat. */
static int validateList(const Node *head, int *count) {
    *count = 0;
    for (const Node *p = head; p != NULL; p = p->next) {
        if (*count == INT_MAX || !validStudent(&p->data)) return 0;
        Student s;
        canonicalStudent(&s, &p->data);
        for (const Node *q = head; q != p; q = q->next) {
            Student previous;
            canonicalStudent(&previous, &q->data);
            if (strcmp(s.id, previous.id) == 0) return 0;
        }
        ++*count;
    }
    return 1;
}

/* File tam nam cung thu muc de co the thay the file dich sau khi dong. */
static FILE *openTemporary(const char *path, char **temporaryPath) {
    if (path == NULL || *path == '\0') return NULL;
    size_t length = strlen(path);
    if (length > (size_t)-1 - 5) return NULL;
    *temporaryPath = malloc(length + 5);
    if (*temporaryPath == NULL) return NULL;
    memcpy(*temporaryPath, path, length);
    memcpy(*temporaryPath + length, ".tmp", 5);
    /* Exclusive create: khong ghi de file tam da co cua lan chay khac. */
#ifdef _WIN32
    /* CRT di kem MinGW co the khong ho tro mode "x" cua fopen. */
    int descriptor = _open(*temporaryPath, _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY,
                           _S_IREAD | _S_IWRITE);
    FILE *file = descriptor < 0 ? NULL : _fdopen(descriptor, "wb");
    if (descriptor >= 0 && file == NULL) {
        _close(descriptor);
        remove(*temporaryPath);
    }
#else
    FILE *file = fopen(*temporaryPath, "wbx");
#endif
    if (file == NULL) {
        free(*temporaryPath);
        *temporaryPath = NULL;
    }
    return file;
}

/* Khong remove file dich truoc: neu thay the that bai, file cu van con. */
static int finishTemporary(FILE *file, char *temporaryPath, const char *path, int ok) {
    if (fclose(file) != 0) ok = 0;
    if (ok) {
#ifdef _WIN32
        ok = MoveFileExA(temporaryPath, path,
                         MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
        ok = rename(temporaryPath, path) == 0;
#endif
    }
    if (!ok) remove(temporaryPath);
    free(temporaryPath);
    return ok;
}

int saveToFile(const char *path, const Node *head) {
    int count;
    if (!validateList(head, &count)) return 0;
    char *temporaryPath = NULL;
    FILE *file = openTemporary(path, &temporaryPath);
    if (file == NULL) return 0;
    FileHeader header = {FILE_MAGIC, FILE_VERSION, count};
    int ok = fwrite(&header, sizeof header, 1, file) == 1;
    for (const Node *p = head; ok && p != NULL; p = p->next) {
        Student s;
        canonicalStudent(&s, &p->data);
        ok = fwrite(&s, sizeof s, 1, file) == 1;
    }
    return finishTemporary(file, temporaryPath, path, ok);
}

int loadFromFile(const char *path, Node **head) {
    if (path == NULL || *path == '\0' || head == NULL) return -1;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return errno == ENOENT ? 0 : -1;
    FileHeader header;
    Node *loaded = NULL;
    int ok = readHeader(file, &header);
    for (int i = 0; ok && i < header.count; ++i) {
        Student raw, s;
        if (fread(&raw, sizeof raw, 1, file) != 1 || !validStudent(&raw)) {
            ok = 0;
            break;
        }
        canonicalStudent(&s, &raw);
        if (findById(*head, s.id) != NULL || addStudent(&loaded, &s) != 1) ok = 0;
    }
    if (fclose(file) != 0) ok = 0;
    if (!ok) {
        freeList(&loaded);
        return -1;
    }
    /* Chi ghep khi toan bo file da doc va dong thanh cong. */
    Node **tail = head;
    while (*tail != NULL) tail = &(*tail)->next;
    *tail = loaded;
    return header.count;
}

int readRecordAt(const char *path, int index, Student *out) {
    if (path == NULL || *path == '\0' || out == NULL || index < 0) return 0;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    FileHeader header;
    Student raw;
    int ok = readHeader(file, &header) && index < header.count;
    if (ok) {
        long offset = (long)sizeof header + (long)index * (long)sizeof(Student);
        ok = fseek(file, offset, SEEK_SET) == 0
             && fread(&raw, sizeof raw, 1, file) == 1 && validStudent(&raw);
    }
    if (fclose(file) != 0) ok = 0;
    if (ok) canonicalStudent(out, &raw);
    return ok;
}

/* Theo quy tac CSV: bao chuoi co dau phay/quote/xuong dong bang quote,
 * moi quote trong chuoi duoc ghi thanh hai quote. */
static int writeCsvText(FILE *file, const char *text) {
    int quoted = strpbrk(text, ",\"\r\n") != NULL;
    if (quoted && fputc('"', file) == EOF) return 0;
    for (const char *p = text; *p; ++p) {
        if (*p == '"' && fputc('"', file) == EOF) return 0;
        if (fputc((unsigned char)*p, file) == EOF) return 0;
    }
    return !quoted || fputc('"', file) != EOF;
}

/* Dinh dang CSV co dau cham thap phan ngay ca khi caller doi LC_NUMERIC. */
static int writeCsvScore(FILE *file, float score) {
    int hundredths = (int)(score * 100.0f + 0.5f);
    return fprintf(file, ",%d.%02d", hundredths / 100, hundredths % 100) >= 0;
}

int exportCsv(const char *path, const Node *head) {
    int count;
    if (!validateList(head, &count)) return 0;
    char *temporaryPath = NULL;
    FILE *file = openTemporary(path, &temporaryPath);
    if (file == NULL) return 0;
    int ok = fputs("MaSV,HoTen,Lop", file) != EOF;
    for (int i = 0; ok && i < MAX_SUBJECTS; ++i)
        ok = fputc(',', file) != EOF && writeCsvText(file, SUBJECT_NAMES[i]);
    if (ok) ok = fputs(",DTB,XepLoai\r\n", file) != EOF;
    for (const Node *p = head; ok && p != NULL; p = p->next) {
        Student s;
        canonicalStudent(&s, &p->data);
        ok = writeCsvText(file, s.id) && fputc(',', file) != EOF
             && writeCsvText(file, s.name) && fputc(',', file) != EOF
             && writeCsvText(file, s.className);
        for (int i = 0; ok && i < MAX_SUBJECTS; ++i)
            ok = writeCsvScore(file, s.scores[i]);
        if (ok) ok = writeCsvScore(file, s.avg) && fputc(',', file) != EOF
                     && writeCsvText(file, rankName(classify(s.avg)))
                     && fputs("\r\n", file) != EOF;
    }
    return finishTemporary(file, temporaryPath, path, ok);
}
