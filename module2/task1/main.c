#include "main.h"

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

int startCLI(PhoneBook* book) {
    printf("Phonebook by Ikadzev for Eltex School Module 2 Task 1\n");
    printf("Usage: add edit delete entry exit\n");
    int out = mainLoop(book);
}

void addCLI(char* args, PhoneBook* book) {
    if (args == NULL) {
        printf("Usage: add\n -n <Name> [required]\n -s <Second name> [required]\n -l <Patronymic>\n" \
               " -j <Job>\n -p <Job post>\n -e <Email>\n -f <Phone>\n" \
               " -t -w -i -g <TG, WA, LI, IG>\n");
        return;
    }
    Entry temp = {0};
    char buffer[MAXSTRING];
    args = strtok(args, "-");
    if (*args == '\0') { args = strtok(NULL, "-"); }
    while (args != NULL) {
        switch (*args) {
        case 'n':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.name, buffer);
            break;
        case 's':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.second_name, buffer);
            break;
        case 'l':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.last_name, buffer);
            break;
        case 'j':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.job, buffer);
            break;
        case 'p':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.job_post, buffer);
            break;
        case 'e':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.emails[temp.emails_count++], buffer);
            break;
        case 'f':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.phones[temp.phones_count++], buffer);
            break;
        case 't':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.profiles.TG, buffer);
            break;
        case 'w':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.profiles.WA, buffer);
            break;
        case 'i':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.profiles.LI, buffer);
            break;
        case 'g':
            sscanf(args + 2, "%99s", buffer);
            strcpy(temp.profiles.IG, buffer);
            break;
        default:
            printf("Unknown type: %c\n", *args);
            return;
        }
        args = strtok(NULL, "-");
    }
    if (temp.name[0] == '\0' || temp.second_name[0] == '\0') {
        printf("Name and Surname are required!\n");
        return;
    }
    int ident = addPB(book, temp.name, temp.second_name);
    editPB(book, ident, "l", temp.last_name);
    editPB(book, ident, "j", temp.job);
    editPB(book, ident, "p", temp.job_post);
    editPB(book, ident, "t", temp.profiles.TG);
    editPB(book, ident, "w", temp.profiles.WA);
    editPB(book, ident, "i", temp.profiles.LI);
    editPB(book, ident, "g", temp.profiles.IG);
    for (int i = 0; i < temp.emails_count; i++) { 
        if (editPB(book, ident, "e", temp.emails[i]) == OVERFLOW_E) {
            printf("Added %d emails, others ignored due to overflow\n", i);
            break;
        }
    }
    for (int i = 0; i < temp.phones_count; i++) { 
        if (editPB(book, ident, "f", temp.phones[i]) == OVERFLOW_P) {
            printf("Added %d phones, others ignored due to overflow\n", i);
            break;
        }
    }
    printf("Done! Indentificator of profile: %d\n", ident);
}

void editCLI(char* args, PhoneBook* book) {
    if (args == NULL) {
        printf("Usage: edit IDENT [-n name] [-s surname] [-l last_name] [-j job] [-p job_post]\n"
               "[-e email] [-E] [-f phone] [-F]\n"
               "[-t TG] [-w WA] [-i LI] [-g IG]\n");
        return;
    }

    char* id_str = strtok(args, " ");
    if (id_str == NULL) {
        printf("Identifier not specified!\n");
        return;
    }

    int ident = atoi(id_str);
    if (ident < 0) {
        printf("Invalid identifier: %d\n", ident);
        return;
    }

    char* token = strtok(NULL, "-");
    while (token != NULL) {
        char buffer[MAXSTRING] = {0};

        switch (*token) {
        case 'n':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "n", buffer);
            break;
        case 's':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "s", buffer);
            break;
        case 'l':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "l", buffer);
            break;
        case 'j':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "j", buffer);
            break;
        case 'p':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "p", buffer);
            break;
        case 'e':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "e", buffer);
            break;
        case 'E':
            editPB(book, ident, "E");
            break;
        case 'f':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "f", buffer);
            break;
        case 'F':
            editPB(book, ident, "F");
            break;
        case 't':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "t", buffer);
            break;
        case 'w':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "w", buffer);
            break;
        case 'i':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "i", buffer);
            break;
        case 'g':
            sscanf(token + 2, "%99s", buffer);
            editPB(book, ident, "g", buffer);
            break;
        default:
            printf("Unknown option: %c\n", *token);
            break;
        }

        token = strtok(NULL, "-");
    }

    printf("Entry %d edited successfully.\n", ident);
}

void deleteCLI(char* args, PhoneBook* book) {
    if (args == NULL) {
        printf("Usage: delete IDENT\n");
        return;
    }
    char* id_str = strtok(args, "\t\n");
    if (id_str == NULL) {
        printf("Usage: delete IDENT\n");
        return;
    }
    int ident = atoi(id_str);
    if (ident < 0) {
        printf("Wrong indetifier!");
        return;
    }
    int err = deletePB(book, ident);
    if (err == NO_ENTRY) {
        printf("Entry is missing, nothing happened\n");
    } else {
        printf("Entry %d deleted\n", ident);
    }
    return;
}

void entryCLI(char* args, PhoneBook* book) {
    if (args == NULL) {
        printf("Usage:\n"
               "  entry -l          List all entries\n"
               "  entry <filter>    Search by keyword\n");
        return;
    }

    const char* filter = NULL;
    if (strcmp(args, "-l") != 0)
        filter = args;

    int found_indices[MAXENTRY];
    int count = entryPB(book, filter, found_indices, MAXENTRY);

    if (count == 0) {
        printf("No matching entries found.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        Entry profile = get_entry(book, found_indices[i]);

        printf("=== Entry %d ===\n", profile.ident);
        printf("Name: %s\n", profile.name);
        printf("Second name: %s\n", profile.second_name);
        if (*profile.last_name) printf("Last name: %s\n", profile.last_name);
        if (*profile.job) printf("Job: %s\n", profile.job);
        if (*profile.job_post) printf("Post: %s\n", profile.job_post);

        for (int j = 0; j < profile.emails_count; j++)
            printf("Email[%d]: %s\n", j + 1, profile.emails[j]);
        for (int j = 0; j < profile.phones_count; j++)
            printf("Phone[%d]: %s\n", j + 1, profile.phones[j]);

        if (*profile.profiles.TG) printf("Post: %s\n", profile.profiles.TG);
        if (*profile.profiles.WA) printf("Post: %s\n", profile.profiles.WA);
        if (*profile.profiles.LI) printf("Post: %s\n", profile.profiles.LI);
        if (*profile.profiles.IG) printf("Post: %s\n", profile.profiles.IG);
    }
}


int mainLoop(PhoneBook* book) {
    while (1) {
        printf("PB> ");

        char command[MAXCOMMAND];
        char* token;

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (strlen(command) == 0) continue;
        token = strtok(command, " ");
        if (!token) continue;

        char* args = strtok(NULL, "");

        if (strcmp(token, "add") == 0) {
            addCLI(args, book);
        } else if (strcmp(token, "edit") == 0) {
            editCLI(args, book);
        } else if (strcmp(token, "delete") == 0) {
            deleteCLI(args, book);
        } else if (strcmp(token, "entry") == 0) {
            entryCLI(args, book);
        } else if(strcmp(token, "exit") == 0) {
            break;
        } else {
            printf("Unknown command\nUsage: add edit delete entry exit\n");
        }
        continue;
    }
}

int main() {
    PhoneBook PB = {0};
    startCLI(&PB);
}