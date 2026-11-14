# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Student Manager — a C11 console app (university "Kỹ thuật lập trình" group assignment) that manages students (ID, name, class, 5 subject scores) in a singly linked list and persists them to a binary file. `README.md` (Vietnamese) is the full design spec and grading checklist; `docs/file_io.md` documents the file I/O module contract. Code comments and UI strings are Vietnamese, mostly without diacritics in printed output.

Work is split by team member (TV1–TV5, see README §9). Only part of the README's planned layout exists: `search_sort` and `stats` (TV2) are implemented and unit-tested but **not yet wired into the menu**; `input` and `menu` modules are **not implemented yet** — input handling and the menu currently live in `src/main.c`. When adding a module, also add its `.c` to every relevant target in `CMakeLists.txt`.

## Build & test

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R menu_file_io --output-on-failure   # single test (also: file_io, file_io_sample)
./build/Student_Manager [-f <data_file>]                      # default: data/students.dat, relative to CWD
cmake -S . -B build -DENABLE_DEBUG=ON                         # enables LOG() macro from config.h
```

Run the program from the repo root (CLion's default working dir is `cmake-build-debug/`, which breaks relative data paths).

Regenerate sample data (overwrites `data/students.dat` and `data/report.csv`; run from repo root):
```sh
cmake --build build --target create_sample_data && build/create_sample_data
```

## Architecture

- `config.h` — shared constants (field sizes, `MAX_SUBJECTS`, `FILE_MAGIC`/`FILE_VERSION`, `DEFAULT_DATA_FILE`, `LOG` macro). `DEFAULT_SAMPLE_FILE` is injected by CMake as an absolute source path.
- `student.{h,c}` — `Student` struct (no pointers, so it is `fwrite`n raw) and per-student logic: ID normalization to uppercase, average, `Rank` classification, setters that recompute `avg`.
- `list.{h,c}` — `Node` linked list (the primary store; no static student arrays). Several functions are deliberately **recursive** (count, find, reverse print) to satisfy assignment requirements — keep them recursive.
- `file_io.{h,c}` — binary format `FileHeader{magic, version, count}` followed by `count` raw `Student` records. `loadFromFile` appends to the list and is all-or-nothing (validates strings, scores, duplicates; returns count / 0 for missing file / -1 on error). `readRecordAt` uses `fseek` with a 0-based index. Saves and CSV export write to `<path>.tmp` then atomically rename.
- `main.c` — `fgets`-only input (never mix with `scanf`), do-while/switch menu. Every successful add/edit/delete **autosaves immediately**; exit saves again. Menu 8 imports `data/sample_students.dat` (read-only source), 9 exports CSV from RAM, 10 reads a record from the saved file (user enters 1-based, passes `index-1`).

## Constraints to respect

- Changing `Student`/`FileHeader` layout changes the on-disk format: bump `FILE_VERSION` and regenerate sample data. The binary format is compiler/platform dependent.
- `tests/test_menu.cmake` drives the real menu via stdin scripts and asserts on **exact output strings and menu numbering** (e.g. `"Da doc 18 sinh vien"`, `"DTB moi = 8.08"`). Changing menu text/options requires updating this test. It operates on a copy of the sample data, never the shipped files.
- Sample data invariants relied on by tests: 18 students, 3 classes, 3 students per rank level. All tests read `data/sample_students.dat`, never `data/students.dat` (the live data file the app autosaves to).
- Text input goes through `readNonEmpty`, which rejects (never silently truncates) input longer than the field buffer; limits are in bytes, so Vietnamese diacritics count 2–3 each. `readId` must reject the same characters as `validStudent` in `file_io.c` (any `isspace`), or saving fails for the whole list.
- Naming: functions `camelCase`, types `PascalCase`, constants `UPPER_CASE`; headers use `#ifndef` include guards; `main()` should stay thin.
