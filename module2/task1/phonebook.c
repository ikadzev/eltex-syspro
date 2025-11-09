#include "phonebook.h"

int set_ident(PhoneBook* book) {
    for (int i = 0; i < IDENT(MAXENTRY); i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t temp_mask = 1 << j;
            if ((book->mask[i] & temp_mask) == 0) {
                book->mask[i] |= temp_mask;
                return i * 8 + j;
            }
        }
    }
    return OVERFLOW; // в теории - невозможный случай, учитывая что count проверяется на переполнение перед этим
}

int get_ident(PhoneBook* book, int ident) { return (book->mask[ident / 8] & (1 << (ident % 8)) != 0); } 

Entry get_entry(PhoneBook* book, int ident) { return book->entries[ident]; }

int entryPB(PhoneBook* book, const char* filter, int* found_indices, int max_found) {
    int found = 0;

    for (int i = 0; i < MAXENTRY && found < max_found; i++) {
        if (!(book->mask[i / 8] & (1 << (i % 8)))) continue;

        Entry* e = &book->entries[i];

        if (filter == NULL || strstr(e->name, filter) || strstr(e->second_name, filter) ||
            strstr(e->last_name, filter) || strstr(e->job, filter) || strstr(e->job_post, filter)) {
            found_indices[found++] = i;
        }
    }

    return found;
}

int addPB(PhoneBook* book, const char* name, const char* surname) {
    if (book->count >= MAXENTRY) {
        return OVERFLOW;
    }
    int ident = set_ident(book);
    if (ident == OVERFLOW) return OVERFLOW;
    book->count++;
    Entry* profile = &book->entries[ident];
    strcpy(profile->name, name);
    strcpy(profile->second_name, surname);
    profile->ident = ident;
    return ident;
}

int deletePB(PhoneBook* book, int ident) {
    if (!get_ident(book, ident)) return NO_ENTRY;
    book->mask[ident / 8] &= ~(1 << (ident % 8));
    Entry temp = {0};
    book->entries[ident] = temp;
    book->count--;
    return ident;
}

int editPB(PhoneBook* book, int ident, char* args, ...) {
    Entry* profile = &book->entries[ident];
    va_list argument_list;
    va_start(argument_list, args);
    while(*args != 0) {
        switch (*(args++)) {
        case 'n':
            strcpy(profile->name, va_arg(argument_list, char*));
            break;
        case 's':
            strcpy(profile->second_name, va_arg(argument_list, char*));
            break;
        case 'l':
            strcpy(profile->last_name, va_arg(argument_list, char*));
            break;
        case 'j':
            strcpy(profile->job, va_arg(argument_list, char*));
            break;
        case 'p':
            strcpy(profile->job_post, va_arg(argument_list, char*));
            break;
        case 'e':
            if (profile->emails_count == MAXEMAIL) return OVERFLOW_E;
            strcpy(profile->emails[profile->emails_count++], va_arg(argument_list, char*));
            break;
        case 'f':
            if (profile->phones_count == MAXPHONE) return OVERFLOW_P;
            strcpy(profile->phones[profile->phones_count++], va_arg(argument_list, char*));
            break;
        case 't':
            strcpy(profile->profiles.TG, va_arg(argument_list, char*));
            break;
        case 'w':
            strcpy(profile->profiles.WA, va_arg(argument_list, char*));
            break;
        case 'i':
            strcpy(profile->profiles.LI, va_arg(argument_list, char*));
            break;
        case 'g':
            strcpy(profile->profiles.IG, va_arg(argument_list, char*));
            break;
        case 'E':
            if (profile->emails_count <= 0) {
                profile->emails_count = 0;
                return OVERFLOW_E;
            }
            profile->emails_count--;
            break;
        case 'F':
            if (profile->phones_count <= 0) {
                profile->phones_count = 0;
                return OVERFLOW_P;
            }
            profile->phones_count--;
            break;
        default:
            return UNKNOWN_EXPRESSION;
        }
    }
    return ident;
}
