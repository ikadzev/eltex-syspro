#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

// === Константы ===
#define MAXSTRING 100
#define MAXEMAIL 10
#define MAXPHONE 10
#define MAXENTRY 50
#define MAXCOMMAND 256

// === Коды ошибок ===
#define OVERFLOW -1
#define OVERFLOW_E -2
#define OVERFLOW_P -3
#define NO_ENTRY -4
#define UNKNOWN_EXPRESSION -5

// === Макросы ===
#define IDENT(x) ((x) % 8 == 0 ? (x) / 8 : (x) / 8 + 1)

// === Структуры ===

typedef struct IM {
    char TG[MAXSTRING];
    char WA[MAXSTRING];
    char LI[MAXSTRING];
    char IG[MAXSTRING];
} IM;

typedef struct Entry {
    int ident;
    char name[MAXSTRING];
    char second_name[MAXSTRING];
    char last_name[MAXSTRING];
    char job[MAXSTRING];
    char job_post[MAXSTRING];
    char emails[MAXEMAIL][MAXSTRING];
    int emails_count;
    char phones[MAXPHONE][12];
    int phones_count;
    IM profiles;
} Entry;

typedef struct PhoneBook {
    int count;
    Entry entries[MAXENTRY];
    uint8_t mask[IDENT(MAXENTRY)];
} PhoneBook;

// === Прототипы функций ===

// Работа с записями
int set_ident(PhoneBook* book);
int get_ident(PhoneBook* book, int ident);
Entry get_entry(PhoneBook* book, int ident);

int addPB(PhoneBook* book, const char* name, const char* surname);
int deletePB(PhoneBook* book, int ident);
int editPB(PhoneBook* book, int ident, char* args, ...);
int entryPB(PhoneBook* book, const char* filter, int* found_indices, int max_found);

// CLI-интерфейс
int startCLI(PhoneBook* book);
int mainLoop(PhoneBook* book);
void addCLI(char* args, PhoneBook* book);
void editCLI(char* args, PhoneBook* book);
void deleteCLI(char* args, PhoneBook* book);
void entryCLI(char* args, PhoneBook* book);

#endif // MAIN_H