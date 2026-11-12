/* Kiem thu hop dong TV3; file thu duoc tao trong working directory cua CTest. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"

/* Dung CHECK thay assert de test van chay o Release/NDEBUG. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

typedef struct {
    unsigned int magic;
    int version;
    int count;
} TestHeader;

static const char *binaryPath = "file_io_test.dat";
static const char *csvPath = "file_io_test.csv";
static const char *badPath = "file_io_test_bad.dat";

/* So sanh du lieu logic, khong so sanh padding cua struct. */
static void sameStudent(const Student *a, const Student *b) {
    CHECK(strcmp(a->id, b->id) == 0);
    CHECK(strcmp(a->name, b->name) == 0);
    CHECK(strcmp(a->className, b->className) == 0);
    CHECK(fabsf(a->avg - b->avg) < 0.0001f);
    for (int i = 0; i < MAX_SUBJECTS; ++i)
        CHECK(a->scores[i] == b->scores[i]);
}

static void sameList(const Node *a, const Node *b) {
    while (a && b) {
        sameStudent(&a->data, &b->data);
        a = a->next;
        b = b->next;
    }
    CHECK(a == NULL && b == NULL);
}

static Student makeStudent(const char *id, float score) {
    Student s;
    float scores[MAX_SUBJECTS];
    for (int i = 0; i < MAX_SUBJECTS; ++i) scores[i] = score;
    initStudent(&s, id, "Nguyen Van An", "CNTT1", scores);
    return s;
}

/* Tao fixture doc lap voi saveToFile de kiem tra file hong/thieu. */
static void writeRaw(TestHeader header, const Student *records, int written, int extra) {
    FILE *f = fopen(badPath, "wb");
    CHECK(f != NULL);
    CHECK(fwrite(&header, sizeof header, 1, f) == 1);
    for (int i = 0; i < written; ++i)
        CHECK(fwrite(&records[i], sizeof records[i], 1, f) == 1);
    if (extra) CHECK(fputc('X', f) != EOF);
    CHECK(fclose(f) == 0);
}

/* Moi loi phai giu nguyen danh sach va bien ket qua cua caller. */
static void rejectFile(Node **head) {
    Node *original = *head;
    Student before = original->data;
    int count = countNodes(*head);
    CHECK(loadFromFile(badPath, head) == -1);
    CHECK(*head == original && countNodes(*head) == count);
    sameStudent(&(*head)->data, &before);
    Student out = before;
    CHECK(readRecordAt(badPath, 0, &out) == 0);
    sameStudent(&out, &before);
}

static void testRoundTrip(void) {
    Node *head = NULL, *loaded = NULL;
    Student records[] = {makeStudent("SV001", 9.0f), makeStudent("SV002", 6.5f),
                         makeStudent("SV003", 3.5f)};
    for (int i = 0; i < 3; ++i) CHECK(addStudent(&head, &records[i]) == 1);
    CHECK(saveToFile(binaryPath, head) == 1);
    CHECK(loadFromFile(binaryPath, &loaded) == 3);
    sameList(head, loaded);
    Student out;
    CHECK(readRecordAt(binaryPath, 0, &out) == 1);
    sameStudent(&out, &records[0]);
    CHECK(readRecordAt(binaryPath, 2, &out) == 1);
    sameStudent(&out, &records[2]);
    CHECK(readRecordAt(binaryPath, -1, &out) == 0);
    CHECK(readRecordAt(binaryPath, 3, &out) == 0);
    sameStudent(&out, &records[2]);
    /* Khong bo qua ma trung voi danh sach da co. */
    CHECK(loadFromFile(binaryPath, &loaded) == -1);
    sameList(head, loaded);
    freeList(&loaded);

    /* Sua + xoa + luu de len file cu: so luong va noi dung phai cap nhat. */
    CHECK(setScore(&head->data, 0, 5.0f) == 1);
    CHECK(deleteStudent(&head, "SV002") == 1);
    CHECK(saveToFile(binaryPath, head) == 1);
    CHECK(loadFromFile(binaryPath, &loaded) == 2);
    CHECK(findById(loaded, "SV002") == NULL);
    sameList(head, loaded);
    CHECK(readRecordAt(binaryPath, 1, &out) == 1);
    CHECK(strcmp(out.id, "SV003") == 0);
    CHECK(readRecordAt(binaryPath, 2, &out) == 0);
    freeList(&loaded);

    /* File rong hop le la header count=0, khong phai file 0 byte. */
    CHECK(saveToFile(binaryPath, NULL) == 1);
    CHECK(loadFromFile(binaryPath, &loaded) == 0 && loaded == NULL);
    CHECK(readRecordAt(binaryPath, 0, &out) == 0);
    /* Hop dong append: file rong khong xoa danh sach dang co. */
    CHECK(loadFromFile(binaryPath, &head) == 0 && countNodes(head) == 2);
    freeList(&head);
    puts("PASS: binary round trip, edit/delete/save, random access, empty list");
}

static void testInvalidFiles(void) {
    Node *head = NULL;
    Student existing = makeStudent("KEEP", 7.0f);
    CHECK(addStudent(&head, &existing) == 1);
    Student s = makeStudent("SV001", 8.0f);
    TestHeader good = {FILE_MAGIC, FILE_VERSION, 1}, h;

    h = good; h.magic = 0; writeRaw(h, &s, 1, 0); rejectFile(&head);
    h = good; h.version++; writeRaw(h, &s, 1, 0); rejectFile(&head);
    h = good; h.count = -1; writeRaw(h, &s, 1, 0); rejectFile(&head);
    h = good; h.count = 2; writeRaw(h, &s, 1, 0); rejectFile(&head);
    writeRaw(good, &s, 1, 1); rejectFile(&head);
    /* Header bi cat va file 0 byte. */
    FILE *f = fopen(badPath, "wb"); CHECK(f != NULL);
    CHECK(fputc('X', f) != EOF); CHECK(fclose(f) == 0); rejectFile(&head);
    f = fopen(badPath, "wb"); CHECK(f != NULL);
    CHECK(fclose(f) == 0); rejectFile(&head);

    Student invalid = s;
    memset(invalid.name, 'X', sizeof invalid.name);
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);
    invalid = s; invalid.className[0] = '\0';
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);
    invalid = s; strcpy(invalid.id, "SV 1");
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);
    invalid = s; invalid.scores[0] = 11.0f;
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);
    invalid = s; invalid.scores[0] = NAN;
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);
    invalid.scores[0] = INFINITY;
    writeRaw(good, &invalid, 1, 0); rejectFile(&head);

    Student duplicates[] = {s, s};
    strcpy(duplicates[1].id, "sv001");
    h = good; h.count = 2;
    writeRaw(h, duplicates, 2, 0);
    CHECK(loadFromFile(badPath, &head) == -1 && countNodes(head) == 1);

    /* avg cu/sai duoc tinh lai, ma chu thuong duoc chuan hoa. */
    s.avg = NAN; strcpy(s.id, "sv001");
    writeRaw(good, &s, 1, 0);
    Student out;
    CHECK(readRecordAt(badPath, 0, &out) == 1);
    CHECK(out.avg == 8.0f && strcmp(out.id, "SV001") == 0);
    CHECK(loadFromFile(badPath, &head) == 1 && countNodes(head) == 2);
    CHECK(strcmp(head->next->data.id, "SV001") == 0);
    CHECK(head->next->data.avg == 8.0f);
    freeList(&head);
    puts("PASS: corrupt/truncated files, duplicate IDs, validation, transactional load");
}

static void testCsvAndErrors(void) {
    Node *head = NULL;
    Student s = makeStudent("SV001", 8.5f);
    strcpy(s.name, "An, \"Binh\"\nChi");
    CHECK(addStudent(&head, &s) == 1);
    CHECK(exportCsv(csvPath, head) == 1);
    FILE *f = fopen(csvPath, "rb"); CHECK(f != NULL);
    char buffer[2048];
    size_t n = fread(buffer, 1, sizeof buffer - 1, f);
    CHECK(!ferror(f)); buffer[n] = '\0'; CHECK(fclose(f) == 0);
    CHECK(strcmp(buffer,
        "MaSV,HoTen,Lop,Toan cao cap,Vat ly,Lap trinh C,Tieng Anh,Triet hoc,DTB,XepLoai\r\n"
        "SV001,\"An, \"\"Binh\"\"\nChi\",CNTT1,8.50,8.50,8.50,8.50,8.50,8.50,Gioi\r\n") == 0);
    CHECK(exportCsv(csvPath, NULL) == 1);
    f = fopen(csvPath, "rb"); CHECK(f != NULL);
    n = fread(buffer, 1, sizeof buffer - 1, f); buffer[n] = '\0';
    CHECK(fclose(f) == 0);
    CHECK(strcmp(buffer,
        "MaSV,HoTen,Lop,Toan cao cap,Vat ly,Lap trinh C,Tieng Anh,Triet hoc,DTB,XepLoai\r\n") == 0);

    CHECK(saveToFile(binaryPath, head) == 1);
    head->data.scores[0] = -1.0f;
    CHECK(saveToFile(binaryPath, head) == 0);
    CHECK(exportCsv(csvPath, head) == 0);
    Student out;
    CHECK(readRecordAt(binaryPath, 0, &out) == 1 && out.scores[0] == 8.5f);
    head->data.scores[0] = 8.5f;
    /* File tam da ton tai phai duoc giu nguyen, khong ghi de. */
    f = fopen("file_io_test.dat.tmp", "wb"); CHECK(f != NULL);
    CHECK(fputs("KEEP", f) != EOF); CHECK(fclose(f) == 0);
    CHECK(saveToFile(binaryPath, head) == 0);
    f = fopen("file_io_test.dat.tmp", "rb"); CHECK(f != NULL);
    n = fread(buffer, 1, sizeof buffer - 1, f); buffer[n] = '\0';
    CHECK(fclose(f) == 0 && strcmp(buffer, "KEEP") == 0);
    CHECK(remove("file_io_test.dat.tmp") == 0);
    CHECK(readRecordAt(binaryPath, 0, &out) == 1);

    CHECK(saveToFile(NULL, head) == 0);
    CHECK(saveToFile("", head) == 0);
    CHECK(exportCsv(NULL, head) == 0);
    CHECK(loadFromFile(NULL, &head) == -1);
    CHECK(loadFromFile(binaryPath, NULL) == -1);
    CHECK(readRecordAt(binaryPath, 0, NULL) == 0);
    CHECK(saveToFile("file_io_missing_directory/test.dat", head) == 0);
    CHECK(exportCsv("file_io_missing_directory/test.csv", head) == 0);
    CHECK(loadFromFile("file_io_missing_directory/test.dat", &head) == 0);
    CHECK(readRecordAt("file_io_missing_directory/test.dat", 0, &out) == 0);
    freeList(&head);
    puts("PASS: CSV escaping, empty CSV, failed save preserves file, bad arguments");
}

static void testSample(const char *path) {
    Node *head = NULL;
    CHECK(loadFromFile(path, &head) == 18);
    int ranks[RANK_COUNT] = {0};
    int classes[3] = {0};
    for (Node *p = head; p; p = p->next) {
        ++ranks[classify(p->data.avg)];
        if (!strcmp(p->data.className, "CNTT1")) ++classes[0];
        if (!strcmp(p->data.className, "CNTT2")) ++classes[1];
        if (!strcmp(p->data.className, "CNTT3")) ++classes[2];
    }
    for (int i = 0; i < RANK_COUNT; ++i) CHECK(ranks[i] == 3);
    for (int i = 0; i < 3; ++i) CHECK(classes[i] == 6);
    Student out;
    CHECK(readRecordAt(path, 17, &out) == 1 && !strcmp(out.id, "SV018"));
    freeList(&head);
    puts("PASS: shipped sample has 18 students, 3 classes, all 6 ranks");
}

int main(int argc, char **argv) {
    if (argc == 2) {
        testSample(argv[1]);
        return 0;
    }
    testRoundTrip();
    testInvalidFiles();
    testCsvAndErrors();
    CHECK(remove(binaryPath) == 0);
    CHECK(remove(csvPath) == 0);
    CHECK(remove(badPath) == 0);
    puts("All file I/O checks passed.");
    return 0;
}
