#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int not_digit = 0;
    for (int i = 1; i < argc; i++) {
        char* ch = argv[i];
        not_digit = 0;
        while (*ch != '\0') {
            if (!isdigit(*(ch++))) {
                not_digit++;
                break;
            }
        }
        ch = argv[i];
        if (not_digit) {
            printf("%s ", ch);
        } else {
            printf("%i ", atoi(ch) % 2);
        }
    }
    return 0;
}