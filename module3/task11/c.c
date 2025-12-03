#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

typedef struct {
    int data[100];
    int count;
    int min_val;
    int max_val;
} shm_t;

#define SHM_NAME "/posix_shm_example"
#define SEMAPHORE "/sem"
// #define SEM_CHILD  "/sem_child"

shm_t *shm;
sem_t *semaphore;
// sem_t *sem_child;

int main() {
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    shm = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    semaphore = sem_open(SEMAPHORE, 0);
    // sem_child  = sem_open(SEM_CHILD, 0);

    while (1) {
        // sem_wait(sem_child);
        sem_wait(semaphore);

        int min = shm->data[0];
        int max = shm->data[0];

        for (int i = 1; i < shm->count; i++) {
            if (shm->data[i] < min) min = shm->data[i];
            if (shm->data[i] > max) max = shm->data[i];
        }

        shm->min_val = min;
        shm->max_val = max;

        // sem_post(sem_parent);
        sem_post(semaphore);
    }

    return 0;
}
