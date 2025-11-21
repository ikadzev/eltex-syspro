#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_ARGUMENTS 10
#define MAX_STRING 100

char** parse_args(char *line) {
    size_t size = 4;
    char **args = malloc(size * sizeof(char*));
    size_t argc = 0;

    char *token = strtok(line, " \t\n");
    while (token) {
        if (argc + 1 >= size) {
            size *= 2;
            args = realloc(args, size * sizeof(char*));
        }

        args[argc] = strdup(token);
        argc++;

        token = strtok(NULL, " \t\n");
    }

    args[argc] = NULL;
    return args;
}

int check_exit(char* arg) {
    if (strcmp(arg, "exit") == 0) return 1;
    return 0;
}

int main() {
    while (1) {
        char command[MAX_STRING];
        char** argv;
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        argv = parse_args(command);
        if (check_exit(argv[0])) return 0;
        pid_t forked = fork();
        int rv;
        switch (forked) {
            case -1:
                perror("Fork error!");
                exit(EXIT_FAILURE);
            case 0:
                execv(argv[0], argv);
                perror("Execution error!");
                exit(EXIT_FAILURE);
            default:
                wait(&rv);
                break;
        }
    }
}