#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_CMDS 20
#define MAX_STRING 200

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
    return arg && strcmp(arg, "exit") == 0;
}

typedef struct {
    char** argv;
    char* in_file;
    char* out_file;
} Command;

Command parse_command(char* text) {
    Command cmd = {0};
    char *copy = strdup(text);

    cmd.in_file = NULL;
    cmd.out_file = NULL;

    char *in = strstr(copy, "<");
    if (in) {
        *in = 0;
        in++;
        cmd.in_file = strdup(strtok(in, " \t"));
    }

    char *out = strstr(copy, ">");
    if (out) {
        *out = 0;
        out++;
        cmd.out_file = strdup(strtok(out, " \t"));
    }

    cmd.argv = parse_args(copy);
    free(copy);
    return cmd;
}

int main() {
    char line[MAX_STRING];

    while (1) {
        printf("> ");
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) continue;

        char* pipeline[MAX_CMDS];
        int cmd_count = 0;

        pipeline[cmd_count++] = strtok(line, "|");

        char* part;
        while ((part = strtok(NULL, "|")) != NULL) {
            pipeline[cmd_count++] = part;
        }

        Command cmds[MAX_CMDS];
        for (int i = 0; i < cmd_count; i++) {
            cmds[i] = parse_command(pipeline[i]);
        }

        if (check_exit(cmds[0].argv[0])) return 0;

        int pipes[MAX_CMDS][2];
        for (int i = 0; i < cmd_count - 1; i++) {
            pipe(pipes[i]);
        }

        for (int i = 0; i < cmd_count; i++) {
            pid_t pid = fork();

            if (pid == 0) {
                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                }

                if (i < cmd_count - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                }

                if (cmds[i].in_file) {
                    int fd = open(cmds[i].in_file, O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }

                if (cmds[i].out_file) {
                    int fd = open(cmds[i].out_file,
                                   O_WRONLY | O_CREAT | O_TRUNC,
                                   0644);
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                for (int k = 0; k < cmd_count - 1; k++) {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }

                execvp(cmds[i].argv[0], cmds[i].argv);
                perror("exec error");
                exit(1);
            }
        }

        for (int i = 0; i < cmd_count - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < cmd_count; i++) {
            wait(NULL);
        }
    }
}
