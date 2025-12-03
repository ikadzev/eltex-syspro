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

// AllocEntry* OLDallocNewEntry(PhoneBook* book) {
//     AllocEntry* newEntry = malloc(sizeof(AllocEntry));
//     if (book->count == 0) {
//         book->firstEntry = newEntry;
//         newEntry->ident = book->count++;
//         newEntry->prev = newEntry;
//     } else {
//         book->lastEntry->next = newEntry;
//         newEntry->ident = book->lastEntry->ident + 1;
//         newEntry->prev = book->lastEntry;
//         book->count++;
//     }
//     book->lastEntry = newEntry;
//     newEntry->next = book->firstEntry;
//     return newEntry;
// }

int getCount(const PhoneBook* book) { return book->count; }

// Entry OLDgetEntry(PhoneBook* book, int ident) { 
//     AllocEntry* cur = book->firstEntry;
//     while (cur->ident < ident) cur = cur->next;
//     return cur->data;
// }
Entry getEntry(PhoneBook* book, int ident) {
    AllocEntry* e = findById(book, ident);
    Entry empty = {0};
    return e ? e->data : empty;
}

// AllocEntry* OLDgetAE(PhoneBook* book, int ident) {
//     AllocEntry* cur = book->firstEntry;
//     while (cur->ident < ident) cur = cur->next;
//     return cur;
// }

// int OLDcheckIdent(PhoneBook* book, int ident) {
//     AllocEntry* cur = book->firstEntry;
//     while (cur->ident < ident) cur = cur->next;
//     return cur->ident == ident;
// }

// int OLDaddPB(PhoneBook* book, const char* name, const char* surname) {
//     AllocEntry* entry = allocNewEntry(book);
//     strcpy(entry->data.name, name);
//     strcpy(entry->data.second_name, surname);
//     return entry->ident;
// }
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

// int OLDeditPB(PhoneBook* book, int ident, char* args, ...) {
//     AllocEntry* entry = book->firstEntry;
//     while (entry->ident < ident) entry = entry->next;
//     if (entry == book->firstEntry && entry->ident != ident) return NO_ENTRY;
//     va_list argument_list;
//     va_start(argument_list, args);
//     while(*args != 0) {
//         switch (*(args++)) {
//         case 'n':
//             strcpy(entry->data.name, va_arg(argument_list, char*));
//             break;
//         case 's':
//             strcpy(entry->data.second_name, va_arg(argument_list, char*));
//             break;
//         case 'l':
//             strcpy(entry->data.last_name, va_arg(argument_list, char*));
//             break;
//         case 'j':
//             strcpy(entry->data.job, va_arg(argument_list, char*));
//             break;
//         case 'p':
//             strcpy(entry->data.job_post, va_arg(argument_list, char*));
//             break;
//         case 'e':
//             if (entry->data.emails_count == MAXEMAIL) return OVERFLOW_E;
//             strcpy(entry->data.emails[entry->data.emails_count++], va_arg(argument_list, char*));
//             break;
//         case 'f':
//             if (entry->data.phones_count == MAXPHONE) return OVERFLOW_P;
//             strcpy(entry->data.phones[entry->data.phones_count++], va_arg(argument_list, char*));
//             break;
//         case 't':
//             strcpy(entry->data.profiles.TG, va_arg(argument_list, char*));
//             break;
//         case 'w':
//             strcpy(entry->data.profiles.WA, va_arg(argument_list, char*));
//             break;
//         case 'i':
//             strcpy(entry->data.profiles.LI, va_arg(argument_list, char*));
//             break;
//         case 'g':
//             strcpy(entry->data.profiles.IG, va_arg(argument_list, char*));
//             break;
//         case 'E':
//             if (entry->data.emails_count <= 0) {
//                 entry->data.emails_count = 0;
//                 return OVERFLOW_E;
//             }
//             entry->data.emails_count--;
//             break;
//         case 'F':
//             if (entry->data.phones_count <= 0) {
//                 entry->data.phones_count = 0;
//                 return OVERFLOW_P;
//             }
//             entry->data.phones_count--;
//             break;
//         default:
//             va_end(argument_list);
//             return UNKNOWN_EXPRESSION;
//         }
//     }
//     va_end(argument_list);
//     return ident;
// }
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

// int OLDentryPB(PhoneBook* book, const char* filter, int* found_indices) {
//     AllocEntry* currentEntry = book->firstEntry;
//     int found = 0;

//     do {
//         if (filter == NULL || 
//             strstr(currentEntry->data.name, filter) || 
//             strstr(currentEntry->data.second_name, filter) ||
//             strstr(currentEntry->data.last_name, filter) || 
//             strstr(currentEntry->data.job, filter) || 
//             strstr(currentEntry->data.job_post, filter)) {
//                 found_indices[found++] = currentEntry->ident;
//             }
//         currentEntry = currentEntry->next;
//     } while (currentEntry->ident > currentEntry->prev->ident);

//     return found;
// }
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

// int OLDdeletePB(PhoneBook* book, int ident) {
//     if (book->count == 0) return NO_ENTRY;
//     if (checkIdent(book, ident) == 0) return NO_ENTRY;
//     AllocEntry* removable = getAE(book, ident);
//     if (book->count == 1) {
//         PhoneBook new = {0};
//         *book = new;
//         free(removable);
//         return ident;
//     }
//     if (removable == book->firstEntry) book->firstEntry = removable->next;
//     if (removable == book->lastEntry) book->lastEntry = removable->prev;
//     removable->next->prev = removable->prev;
//     removable->prev->next = removable->next;
//     free(removable);
//     book->count--;
//     return ident;
// }
int deletePB(PhoneBook* book, int ident) {
    AllocEntry* e = findById(book, ident);
    if (!e) return NO_ENTRY;
 
    detach(book, e);
    free(e);
    book->count--;
 
    return ident;
}

// void OLDfreePB(PhoneBook* book) {
//     if (book->count == 0) return;
//     AllocEntry* cur = book->firstEntry;
//     AllocEntry* temp;
//     for (int i = 0; i < book->count; i++) {
//         temp = cur->next;
//         free(cur);
//         cur = temp;
//     }
//     return;
// }
void freePB(PhoneBook* book) {
    AllocEntry* cur = book->firstEntry;
    while (cur) {
        AllocEntry* next = cur->next;
        free(cur);
        cur = next;
    }
}