#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

// Constants used in phonebook.c
#define MAXSTRING 100
#define MAXEMAIL 10
#define MAXPHONE 10

#define OVERFLOW -1
#define OVERFLOW_E -2
#define OVERFLOW_P -3
#define NO_ENTRY -4
#define UNKNOWN_EXPRESSION -5

// Types used in phonebook.c
typedef struct IM {
    char TG[MAXSTRING];
    char WA[MAXSTRING];
    char LI[MAXSTRING];
    char IG[MAXSTRING];
} IM;

typedef struct Entry {
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

typedef struct AllocEntry {
    int ident;
    Entry data;
    struct AllocEntry* next;
    struct AllocEntry* prev;
} AllocEntry;

typedef struct PhoneBook {
    int count;
    AllocEntry* firstEntry;
    AllocEntry* lastEntry;
} PhoneBook;

AllocEntry* allocNewEntry(PhoneBook* book);
int getCount(const PhoneBook* book);
Entry getEntry(PhoneBook* book, int ident);
AllocEntry* getAE(PhoneBook* book, int ident);
int checkIdent(PhoneBook* book, int ident);

int addPB(PhoneBook* book, const char* name, const char* surname);
int editPB(PhoneBook* book, int ident, char* args, ...);
int deletePB(PhoneBook* book, int ident);
int entryPB(PhoneBook* book, const char* filter, int* found_indices);
void freePB(PhoneBook* book);

#endif // PHONEBOOK_H
