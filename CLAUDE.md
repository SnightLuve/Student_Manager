# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Student Manager — a C11 console app (university "Kỹ thuật lập trình" group assignment) that manages students (ID, name, class, 5 subject scores) in a singly linked list and persists them to a binary file. `README.md` (Vietnamese) is the full design spec and grading checklist; `docs/file_io.md` documents the file I/O module contract. Code comments and UI strings are Vietnamese, mostly without diacritics in printed output.

Work is split by team member (TV1–TV5, see README §9). Only part of the README's planned layout exists: `search_sort` and `stats` (TV2) are implemented, unit-tested and wired into the menu (options 11–13); `input` and `menu` modules are **not implemented yet** — input handling and the menu currently live in `src/main.c`. When adding a module, also add its `.c` to every relevant target in `CMakeLists.txt`.

## Build & test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R menu_file_io --output-on-failure   # single test by name (see list below)
./build/Student_Manager [-f <data_file>]                      # default: data/students.dat, relative to CWD
cmake -S . -B build -DENABLE_DEBUG=ON                         # enables LOG() macro from config.h
```

Run the program from the repo root (CLion's default working dir is `cmake-build-debug/`, which breaks relative data paths).

CTest tests (all defined in `CMakeLists.txt`):
- `file_io`, `file_io_sample` — `tests/test_file_io.c` (round trip, invalid files, CSV escaping and rounding; `_sample` also checks `data/sample_students.dat`).
- `search_sort_stats` — `tests/test_search_sort_stats.c` (TV2 modules against the sample file, including the >`MAX_CLASS_STATS` classes warning).
- `student_list` — `tests/test_student_list.c` (TV1 `student` + `list`: init/normalize, setters, add/find/delete/update, print output, deep list for recursion).
- `menu_input` — `tests/test_input.cmake` drives the real binary to check input validation (too-long/empty/whitespace input, out-of-range numbers, CSV refusing to overwrite a data file, bad CLI args).
- `menu_file_io` — `tests/test_menu.cmake` drives the real binary through the file menu options (8–10, autosave) and the TV2 options (11–13).

C tests use a local `CHECK` macro (not `assert`, so they work under `NDEBUG`) and `tests/test_capture.h` (`captureBegin`/`captureEnd`) to assert on what print functions write to stdout.

Regenerate sample data (run from repo root). With no arguments the tool writes `data/students.dat` and `data/report.csv`; tests read `data/sample_students.dat`, so pass that path to regenerate the test fixture:
```sh
cmake --build build --target create_sample_data
build/create_sample_data data/sample_students.dat data/report.csv
```

## Architecture

- `config.h` — shared constants (field sizes, `MAX_SUBJECTS`, `FILE_MAGIC`/`FILE_VERSION`, `DEFAULT_DATA_FILE`, `LOG` macro). `DEFAULT_SAMPLE_FILE` is injected by CMake as an absolute source path.
- `student.{h,c}` — `Student` struct (no pointers, so it is `fwrite`n raw) and per-student logic: ID normalization to uppercase, average, `Rank` classification, setters that recompute `avg`.
- `list.{h,c}` — `Node` linked list (the primary store; no static student arrays). Several functions are deliberately **recursive** (count, find, reverse print) to satisfy assignment requirements — keep them recursive.
- `file_io.{h,c}` — binary format `FileHeader{magic, version, count}` followed by `count` raw `Student` records. `loadFromFile` appends to the list and is all-or-nothing (validates strings, scores, duplicates; returns count / 0 for missing file / -1 on error). `readRecordAt` uses `fseek` with a 0-based index. Saves and CSV export write to `<path>.tmp` (created exclusively, so a leftover `.tmp` from an interrupted run blocks saving until removed) then atomically rename. CSV scores are formatted with `"%.2f"` exactly like the screen (`printStudentRow`), with any locale decimal comma turned into a dot.
- `search_sort.{h,c}` (TV2) — case-insensitive search by name/class, filter by average range, `mergeSort` on the list with a `CompareFn` (`cmpById`, `cmpByName`, `cmpByAvgDesc`, `cmpByClassThenAvg`).
- `stats.{h,c}` (TV2) — overall/per-subject averages, count by rank, top/bottom student, per-class stats (capped at `MAX_CLASS_STATS` = 50 classes; `statsByClass` prints a warning with the number of students left out instead of dropping them silently).
- `main.c` — `fgets`-only input (never mix with `scanf`), do-while/switch menu (options 1–13, 0 = exit; new options are appended rather than renumbering, because the menu tests depend on the numbers). Every successful add/edit/delete **autosaves immediately**; exit saves again. Menu 8 imports `data/sample_students.dat` (read-only source), 9 exports CSV from RAM (refuses a target that equals the data file path or is any binary student file, detected by reading `FILE_MAGIC`), 10 reads a record from the saved file (user enters 1-based, passes `index-1`), 11 is a search/filter submenu (name, class, average range), 12 sorts via an array of `CompareFn` and **autosaves the new order**, 13 prints all stats. `readLine` returns 0 when a line overflowed the buffer; `readInt`/`readFloat`/CSV path reject such lines.

## Constraints to respect

- Changing `Student`/`FileHeader` layout changes the on-disk format: bump `FILE_VERSION` and regenerate sample data. The binary format is compiler/platform dependent.
- `tests/test_menu.cmake` and `tests/test_input.cmake` drive the real menu via stdin scripts and assert on **exact output strings, message counts and menu numbering** (e.g. `"Da doc 18 sinh vien"`, `"DTB moi = 8.08"`, `"Ma SV khong duoc chua khoang trang"`). Changing menu text/options or error messages requires updating these tests. They operate on a copy of the sample data in the build dir, never the shipped files.
- The `.cmake` test scripts run with `cmake -P`, where policies are unset unless the script calls `cmake_minimum_required`. Without it, `while(TRUE)`/`if(TRUE)` treat `TRUE` as an (undefined) variable name, so loops silently never run.
- Sample data invariants relied on by tests: 18 students, 3 classes, 3 students per rank level. All tests read `data/sample_students.dat`, never `data/students.dat` (the live data file the app autosaves to).
- Text input goes through `readNonEmpty`, which rejects (never silently truncates) input longer than the field buffer; limits are in bytes, so Vietnamese diacritics count 2–3 each. `readId` must reject the same characters as `validStudent` in `file_io.c` (any `isspace`), or saving fails for the whole list.
- Naming: functions `camelCase`, types `PascalCase`, constants `UPPER_CASE`; headers use `#ifndef` include guards; `main()` should stay thin.
