#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#define SEM "/sem"
#define SEM_CHECK "/sem_check"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s filename\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];


    sem_t *sem = sem_open(SEM, O_CREAT, 0666, 0);
    sem_t *check = sem_open(SEM_CHECK, O_CREAT, 0666, 0);
    
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    while (1) {
        FILE *f = fopen(filename, "a");
        if (!f) { perror("fopen"); exit(1); }

        int count = rand() % 10 + 3;
        for (int i = 0; i < count; i++) {
            fprintf(f, "%d ", rand() % 100);
        }
        fprintf(f, "\n");
        fclose(f);

        sem_post(sem);
        sem_post(check);
        sleep(1);
        sem_wait(sem);
    }

    sem_close(sem);
    return 0;
}
