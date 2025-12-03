#include "phonebook.h"

static void insertSorted(PhoneBook* book, AllocEntry* node) {
    if (!book->firstEntry) {
        book->firstEntry = book->lastEntry = node;
        node->prev = node->next = NULL;
        return;
    }

    AllocEntry* cur = book->firstEntry;

    while (cur && strcmp(cur->data.second_name, node->data.second_name) < 0)
        cur = cur->next;

    if (!cur) {
        node->prev = book->lastEntry;
        node->next = NULL;
        book->lastEntry->next = node;
        book->lastEntry = node;
        return;
    }

    if (cur == book->firstEntry) {
        node->prev = NULL;
        node->next = cur;
        cur->prev = node;
        book->firstEntry = node;
        return;
    }

    node->next = cur;
    node->prev = cur->prev;
    cur->prev->next = node;
    cur->prev = node;
}

static void detach(PhoneBook* book, AllocEntry* node) {
    if (!node) return;

    if (node == book->firstEntry)
        book->firstEntry = node->next;
    if (node == book->lastEntry)
        book->lastEntry = node->prev;

    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;

    node->prev = node->next = NULL;
}

static AllocEntry* findById(PhoneBook* book, int ident) {
    AllocEntry* cur = book->firstEntry;
    while (cur) {
        if (cur->ident == ident) return cur;
        cur = cur->next;
    }
    return NULL;
}

int getCount(const PhoneBook* book) { return book->count; }

Entry getEntry(PhoneBook* book, int ident) {
    AllocEntry* e = findById(book, ident);
    Entry empty = {0};
    return e ? e->data : empty;
}

int addPB(PhoneBook* book, const char* name, const char* surname) {
    AllocEntry* entry = malloc(sizeof(AllocEntry));
    memset(entry, 0, sizeof(*entry));

    strcpy(entry->data.name, name);
    strcpy(entry->data.second_name, surname);

    entry->ident = book->nextIdent++;

    insertSorted(book, entry);
    book->count++;
    return entry->ident;
}

int editPB(PhoneBook* book, int ident, char* args, ...) {
    AllocEntry* e = findById(book, ident);
    if (!e) return NO_ENTRY;

    char oldSurname[64];
    strcpy(oldSurname, e->data.second_name);

    va_list vl;
    va_start(vl, args);

    while (*args) {
        switch (*args++) {
        case 'n': strcpy(e->data.name, va_arg(vl, char*)); break;
        case 's': strcpy(e->data.second_name, va_arg(vl, char*)); break;
        case 'l': strcpy(e->data.last_name, va_arg(vl, char*)); break;
        case 'j': strcpy(e->data.job, va_arg(vl, char*)); break;
        case 'p': strcpy(e->data.job_post, va_arg(vl, char*)); break;

        case 'e':
            if (e->data.emails_count == MAXEMAIL) return OVERFLOW_E;
            strcpy(e->data.emails[e->data.emails_count++], va_arg(vl, char*));
            break;

        case 'f':
            if (e->data.phones_count == MAXPHONE) return OVERFLOW_P;
            strcpy(e->data.phones[e->data.phones_count++], va_arg(vl, char*));
            break;

        case 'E':
            if (e->data.emails_count > 0) e->data.emails_count--;
            else return OVERFLOW_E;
            break;

        case 'F':
            if (e->data.phones_count > 0) e->data.phones_count--;
            else return OVERFLOW_P;
            break;

        case 't': strcpy(e->data.profiles.TG, va_arg(vl, char*)); break;
        case 'w': strcpy(e->data.profiles.WA, va_arg(vl, char*)); break;
        case 'i': strcpy(e->data.profiles.LI, va_arg(vl, char*)); break;
        case 'g': strcpy(e->data.profiles.IG, va_arg(vl, char*)); break;

        default:
            va_end(vl);
            return UNKNOWN_EXPRESSION;
        }
    }

    va_end(vl);

    if (strcmp(oldSurname, e->data.second_name) != 0) {
        detach(book, e);
        insertSorted(book, e);
    }

    return ident;
}

int entryPB(PhoneBook* book, const char* filter, int* found) {
    int count = 0;
    AllocEntry* cur = book->firstEntry;

    while (cur) {
        if (!filter ||
            strstr(cur->data.name, filter) ||
            strstr(cur->data.second_name, filter) ||
            strstr(cur->data.last_name, filter) ||
            strstr(cur->data.job, filter) ||
            strstr(cur->data.job_post, filter)) {

            found[count++] = cur->ident;
        }
        cur = cur->next;
    }

    return count;
}

int deletePB(PhoneBook* book, int ident) {
    AllocEntry* e = findById(book, ident);
    if (!e) return NO_ENTRY;
 
    detach(book, e);
    free(e);
    book->count--;
 
    return ident;
}

void freePB(PhoneBook* book) {
    AllocEntry* cur = book->firstEntry;
    while (cur) {
        AllocEntry* next = cur->next;
        free(cur);
        cur = next;
    }
}