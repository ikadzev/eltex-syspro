#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_bits(mode_t mode) {
    for (int i = 8; i >= 0; i--)
        printf("%d", (mode >> i) & 1);
    printf("\n");
}

void print_symbolic(mode_t mode) {
    char s[10];
    s[0] = (mode & S_IRUSR) ? 'r' : '-';
    s[1] = (mode & S_IWUSR) ? 'w' : '-';
    s[2] = (mode & S_IXUSR) ? 'x' : '-';
    s[3] = (mode & S_IRGRP) ? 'r' : '-';
    s[4] = (mode & S_IWGRP) ? 'w' : '-';
    s[5] = (mode & S_IXGRP) ? 'x' : '-';
    s[6] = (mode & S_IROTH) ? 'r' : '-';
    s[7] = (mode & S_IWOTH) ? 'w' : '-';
    s[8] = (mode & S_IXOTH) ? 'x' : '-';
    s[9] = '\0';
    printf("%s\n", s);
}

void modify_mode(mode_t *mode, const char *cmd) {
    mode_t target = 0;
    int set = 0;
    while (set == 0)
        switch(*cmd++) {
            case 'u': target |= S_IRWXU; break;
            case 'g': target |= S_IRWXG; break;
            case 'o': target |= S_IRWXO; break;
            case 'a': target = S_IRWXU | S_IRWXG | S_IRWXO; break;
            case '+': set = 1; break;
            case '-': set = 2; break;
            case '=': set = 3; break;
    }
    mode_t priv = 0;
    while (*cmd != '\0' || *cmd != ' ') {
        switch (*cmd++) {
            case 'r': priv |= S_IRUSR | S_IRGRP | S_IROTH; break;
            case 'w': priv |= S_IWUSR | S_IWGRP | S_IWOTH; break;
            case 'x': priv |= S_IXUSR | S_IXGRP | S_IXOTH; break;
        }
    }
    if (set == 1) *mode |= (priv & target);
    if (set == 2) *mode &= ~(priv & target);
    if (set == 3) *mode = (priv & target);
}

int main(int argc, char *argv[]) {

    mode_t mode = 0;
    struct stat st;
    const char *file = NULL;

    if (argc == 1) {
        printf("Usage:\n"
                "  %s -n [num_mask] [FILE]\n"
                "  %s -s [sym_mask] [FILE]\n"
                "  %s [FILE]\n", argv[0], argv[0], argv[0]);
        return 0;
    } 
    else if (argc == 2) {
        if (argv[1][0] == '-') {
            printf("Wrong argument!\n");
            return 1;
        }
        file = argv[1];
        if (stat(file, &st) == -1) {
            perror("stat");
            return 1;
        }
        mode = st.st_mode & 0777;
    } 
    else if (argc >= 3) {
        file = argv[3];
        if (strcmp(argv[1], "-n") == 0) {
            mode = strtol(argv[2], NULL, 8);
        } 
        else if (strcmp(argv[1], "-s") == 0) {
            if (stat(file, &st) == -1) {
                perror("stat");
                return 1;
            }
            mode = st.st_mode & 0777;
            modify_mode(&mode, argv[2]);
        }
    }

    if (file)
        printf("File: %s\n", file);

    printf("Bits: "); 
    print_bits(mode);
    printf("Numbers: %03o\n", mode);
    printf("Letters: "); 
    print_symbolic(mode);

    return 0;
}
