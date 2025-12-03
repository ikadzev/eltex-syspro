#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

typedef struct {
    int data[100];
    int count;
    int min_val;
    int max_val;
} shm_t;

int shmid, semid;
shm_t *shm;
pid_t child_pid;

int sets_processed = 0;

void parent_cleanup(int sig) {
    printf("\nОбработано наборов: %d\n", sets_processed);

    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    kill(child_pid, SIGTERM);

    exit(0);
}

void sem_op(int semid, int semnum, int val) {
    struct sembuf sb = { semnum, val, 0 };
    semop(semid, &sb, 1);
}

int main() {
    key_t key = ftok("lock", 65);
    shmid = shmget(key, sizeof(shm_t), 0666 | IPC_CREAT);
    shm = (shm_t*) shmat(shmid, NULL, 0);

    semid = semget(key, 1, 0666 | IPC_CREAT);

    semctl(semid, 0, SETVAL, 0);

    signal(SIGINT, parent_cleanup);

    child_pid = fork();

    if (child_pid == 0) {
        execl("./child", "./child", NULL);
        perror("execl");
        exit(1);
    }

    while (1) {
        int n = rand() % 10 + 5;
        shm->count = n;

        for (int i = 0; i < n; i++)
            shm->data[i] = rand() % 100;

        sem_op(semid, 0, 1);   // V(1)

        printf("min = %d, max = %d\n", shm->min_val, shm->max_val);
        sets_processed++;

        sleep(1);
        sem_op(semid, 0, -1);
    }

    return 0;
}
