#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
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
    int semid = semget(key, 2, 0666);

    char line[1024];

    while (1) {
        S(semid);
        sem_wait(semid);

        FILE *f = fopen(filename, "r+");
        if (!f) { perror("fopen"); exit(1); }

        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            sem_signal(semid);
            sleep(1);
            continue;
            // perror("fgets");
            // exit(1);
        }

        char rest[8192];
        size_t used = fread(rest, 1, sizeof(rest), f);

        freopen(filename, "w", f);
        fwrite(rest, 1, used, f);

        fclose(f);
        sem_signal(semid);

        int min = 99999, max = -99999, x;
        char *p = strtok(line, " ");
        while (p) {
            x = atoi(p);
            if (x < min) min = x;
            if (x > max) max = x;
            p = strtok(NULL, " ");
        }

        printf("min=%d max=%d\n", min, max);
        fflush(stdout);

        usleep(200000);
    }
}
