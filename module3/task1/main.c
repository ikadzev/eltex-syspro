#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int check_number(const char* string) {
    // int isreal = 0;
    // int isnegative = 0;
    while (*string != '\0') {
        if (!isdigit(*string++)) {
            return 1;
        }
        // string++;
    }
    return 0;
}

void mod_printf(int s, int e, const char* v[], const char* name) {
    for (int i = s; i < e; i++) {
        printf("%s: ", name);
        if (check_number(v[i])) {
            printf("%s ", v[i]);
        } else {
            printf("%i -> %i ", atoi(v[i]), atoi(v[i]) * 2);
        }
        printf("\n");
    }
}

int main(int argc, const char* argv[]) {
    pid_t pid = fork();
    switch (pid) {
        case -1:
            perror("FE");
            exit(EXIT_FAILURE);
        case 0:
            mod_printf((argc - 1) / 2 + 1, argc, argv, "KID");
            break;
        default:
            mod_printf(1, (argc - 1) / 2 + 1, argv, "PARENT");
            break;
    }
    return 0;
}