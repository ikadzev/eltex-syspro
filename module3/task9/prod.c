#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s filename\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];

    char sem_name[256];
    snprintf(sem_name, sizeof(sem_name), "/sem_%s", filename);
    for (char *p = sem_name; *p; p++) {
        if (*p == '/' || *p == '.') *p = '_';
    }

    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    srand(time(NULL) ^ getpid());

    while (1) {
        sem_wait(sem);

        FILE *f = fopen(filename, "a");
        if (!f) { perror("fopen"); exit(1); }

        int count = rand() % 10 + 3;
        for (int i = 0; i < count; i++) {
            fprintf(f, "%d ", rand() % 100);
        }
        fprintf(f, "\n");
        fclose(f);

        sem_post(sem);

        sleep(1);
    }

    sem_close(sem);
    return 0;
}
