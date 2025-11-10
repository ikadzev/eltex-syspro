#include "phonebook.h"

AllocEntry* allocNewEntry(PhoneBook* book) {
    AllocEntry* newEntry = malloc(sizeof(AllocEntry));
    if (book->count == 0) {
        book->firstEntry = newEntry;
        newEntry->ident = book->count++;
        newEntry->prev = newEntry;
    } else {
        book->lastEntry->next = newEntry;
        newEntry->ident = book->lastEntry->ident + 1;
        newEntry->prev = book->lastEntry;
        book->count++;
    }
    book->lastEntry = newEntry;
    newEntry->next = book->firstEntry;
    return newEntry;
}

int getCount(const PhoneBook* book) { return book->count; }

Entry getEntry(PhoneBook* book, int ident) { 
    AllocEntry* cur = book->firstEntry;
    while (cur->ident < ident) cur = cur->next;
    return cur->data;
}

AllocEntry* getAE(PhoneBook* book, int ident) {
    AllocEntry* cur = book->firstEntry;
    while (cur->ident < ident) cur = cur->next;
    return cur;
}

int checkIdent(PhoneBook* book, int ident) {
    AllocEntry* cur = book->firstEntry;
    while (cur->ident < ident) cur = cur->next;
    return cur->ident == ident;
}

int addPB(PhoneBook* book, const char* name, const char* surname) {
    AllocEntry* entry = allocNewEntry(book);
    strcpy(entry->data.name, name);
    strcpy(entry->data.second_name, surname);
    return entry->ident;
}

int editPB(PhoneBook* book, int ident, char* args, ...) {
    AllocEntry* entry = book->firstEntry;
    while (entry->ident < ident) entry = entry->next;
    if (entry == book->firstEntry && entry->ident != ident) return NO_ENTRY;
    va_list argument_list;
    va_start(argument_list, args);
    while(*args != 0) {
        switch (*(args++)) {
        case 'n':
            strcpy(entry->data.name, va_arg(argument_list, char*));
            break;
        case 's':
            strcpy(entry->data.second_name, va_arg(argument_list, char*));
            break;
        case 'l':
            strcpy(entry->data.last_name, va_arg(argument_list, char*));
            break;
        case 'j':
            strcpy(entry->data.job, va_arg(argument_list, char*));
            break;
        case 'p':
            strcpy(entry->data.job_post, va_arg(argument_list, char*));
            break;
        case 'e':
            if (entry->data.emails_count == MAXEMAIL) return OVERFLOW_E;
            strcpy(entry->data.emails[entry->data.emails_count++], va_arg(argument_list, char*));
            break;
        case 'f':
            if (entry->data.phones_count == MAXPHONE) return OVERFLOW_P;
            strcpy(entry->data.phones[entry->data.phones_count++], va_arg(argument_list, char*));
            break;
        case 't':
            strcpy(entry->data.profiles.TG, va_arg(argument_list, char*));
            break;
        case 'w':
            strcpy(entry->data.profiles.WA, va_arg(argument_list, char*));
            break;
        case 'i':
            strcpy(entry->data.profiles.LI, va_arg(argument_list, char*));
            break;
        case 'g':
            strcpy(entry->data.profiles.IG, va_arg(argument_list, char*));
            break;
        case 'E':
            if (entry->data.emails_count <= 0) {
                entry->data.emails_count = 0;
                return OVERFLOW_E;
            }
            entry->data.emails_count--;
            break;
        case 'F':
            if (entry->data.phones_count <= 0) {
                entry->data.phones_count = 0;
                return OVERFLOW_P;
            }
            entry->data.phones_count--;
            break;
        default:
            va_end(argument_list);
            return UNKNOWN_EXPRESSION;
        }
    }
    va_end(argument_list);
    return ident;
}

int entryPB(PhoneBook* book, const char* filter, int* found_indices) {
    if (book->count == 0) return 0;
    AllocEntry* currentEntry = book->firstEntry;
    int found = 0;

    do {
        if (filter == NULL || 
            strstr(currentEntry->data.name, filter) || 
            strstr(currentEntry->data.second_name, filter) ||
            strstr(currentEntry->data.last_name, filter) || 
            strstr(currentEntry->data.job, filter) || 
            strstr(currentEntry->data.job_post, filter)) {
                found_indices[found++] = currentEntry->ident;
            }
        currentEntry = currentEntry->next;
    } while (currentEntry->ident > currentEntry->prev->ident);

    return found;
}

int deletePB(PhoneBook* book, int ident) {
    if (book->count == 0) return NO_ENTRY;
    if (checkIdent(book, ident) == 0) return NO_ENTRY;
    AllocEntry* removable = getAE(book, ident);
    if (book->count == 1) {
        PhoneBook new = {0};
        *book = new;
        free(removable);
        return ident;
    }
    if (removable == book->firstEntry) book->firstEntry = removable->next;
    if (removable == book->lastEntry) book->lastEntry = removable->prev;
    removable->next->prev = removable->prev;
    removable->prev->next = removable->next;
    free(removable);
    book->count--;
    return ident;
}

void freePB(PhoneBook* book) {
    if (book->count == 0) return;
    AllocEntry* cur = book->firstEntry;
    AllocEntry* temp;
    for (int i = 0; i < book->count; i++) {
        temp = cur->next;
        free(cur);
        cur = temp;
    }
    return;
}