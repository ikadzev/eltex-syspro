#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>

void sem_wait(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}

void sem_signal(int semid) {
    struct sembuf op = {0, +1, 0};
    semop(semid, &op, 1);
}

void V(int semid) {
    struct sembuf op = {1, 1, 0};
    semop(semid, &op, 1);
}

void S(int semid) {
    struct sembuf op = {1, -1, 0};
    semop(semid, &op, 1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s filename\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    key_t key = ftok(filename, 'S');
    int semid = semget(key, 2, IPC_CREAT | 0666);

    semctl(semid, 0, SETVAL, 1);

    srand(time(NULL) ^ getpid());

    while (1) {
        V(semid);
        sem_wait(semid);

        FILE *f = fopen(filename, "a");
        if (!f) { perror("fopen"); exit(1); }

        int count = rand() % 10 + 3; // 3..12
        for (int i = 0; i < count; i++) {
            fprintf(f, "%d ", rand() % 100);
        }
        fprintf(f, "\n");

        fclose(f);

        sem_signal(semid);

        sleep(1);
    }
}
