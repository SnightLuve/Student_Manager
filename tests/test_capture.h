/* test_capture.h - Bat output stdout cua cac ham in (printList, statsByClass...)
 * vao chuoi de test kiem tra noi dung. Chi dung trong tests/. */
#ifndef TEST_CAPTURE_H
#define TEST_CAPTURE_H

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#define TC_DUP    _dup
#define TC_DUP2   _dup2
#define TC_CLOSE  _close
#define TC_FILENO _fileno
#else
#include <unistd.h>
#define TC_DUP    dup
#define TC_DUP2   dup2
#define TC_CLOSE  close
#define TC_FILENO fileno
#endif

static int captureSavedFd = -1;
static const char *capturePath = NULL;

/* Chuyen huong stdout vao file path cho toi khi goi captureEnd. */
static inline void captureBegin(const char *path) {
    fflush(stdout);
    captureSavedFd = TC_DUP(TC_FILENO(stdout));
    FILE *f = fopen(path, "w");
    if (captureSavedFd < 0 || f == NULL) {
        fprintf(stderr, "captureBegin: khong mo duoc %s\n", path);
        exit(EXIT_FAILURE);
    }
    TC_DUP2(TC_FILENO(f), TC_FILENO(stdout));
    fclose(f);
    capturePath = path;
}

/* Khoi phuc stdout, tra ve noi dung da in (malloc, caller free). */
static inline char *captureEnd(void) {
    fflush(stdout);
    TC_DUP2(captureSavedFd, TC_FILENO(stdout));
    TC_CLOSE(captureSavedFd);
    FILE *f = fopen(capturePath, "rb");
    if (f == NULL) exit(EXIT_FAILURE);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *text = malloc((size_t)size + 1);
    if (text == NULL) exit(EXIT_FAILURE);
    size_t n = fread(text, 1, (size_t)size, f);
    text[n] = '\0';
    fclose(f);
    remove(capturePath);
    return text;
}

#endif /* TEST_CAPTURE_H */
