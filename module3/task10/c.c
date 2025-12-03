#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

typedef struct {
    int data[100];
    int count;
    int min_val;
    int max_val;
} shm_t;

int shmid, semid;
shm_t *shm;

void sem_op(int semid, int semnum, int val) {
    struct sembuf sb = { semnum, val, 0 };
    semop(semid, &sb, 1);
}

int main() {
    key_t key = ftok("lock", 65);
    shmid = shmget(key, sizeof(shm_t), 0666);
    shm = (shm_t*) shmat(shmid, NULL, 0);

    semid = semget(key, 1, 0666);

    while (1) {
        sem_op(semid, 0, -1);  // P(1)

        int min = shm->data[0];
        int max = shm->data[0];

        for (int i = 1; i < shm->count; i++) {
            if (shm->data[i] < min) min = shm->data[i];
            if (shm->data[i] > max) max = shm->data[i];
        }

        shm->min_val = min;
        shm->max_val = max;

        sem_op(semid, 0, 1);   // V(0)
    }

    return 0;
}
