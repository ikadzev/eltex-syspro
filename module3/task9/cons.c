#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SEM "/sem"
#define SEM_CHECK "/sem_check"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s filename\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    sem_t *sem = sem_open(SEM, 0);
    sem_t *check = sem_open(SEM_CHECK, 0);
    
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    char line[1024];

    while (1) {
        sem_wait(check);
        sem_wait(sem);

        FILE *f = fopen(filename, "r+");
        if (!f) { perror("fopen"); exit(1); }

        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            sem_post(sem);
            usleep(300000);
            continue;
        }

        char rest[8192];
        size_t n = fread(rest, 1, sizeof(rest), f);

        freopen(filename, "w", f);
        fwrite(rest, 1, n, f);

        fclose(f);
        sem_post(sem);

        int min = 999999, max = -999999;
        int x;
        char *p = strtok(line, " ");
        while (p) {
            x = atoi(p);
            if (x < min) min = x;
            if (x > max) max = x;
            p = strtok(NULL, " \n");
        }

        printf("min=%d max=%d\n", min, max);
        fflush(stdout);
    }

    sem_close(sem);
    return 0;
}
