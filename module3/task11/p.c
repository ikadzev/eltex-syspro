#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>

typedef struct {
    int data[100];
    int count;
    int min_val;
    int max_val;
} shm_t;

#define SHM_NAME "/posix_shm_example"
#define SEMAPHORE "/sem"
#define SEM_CHILD  "/sem_child"

shm_t *shm;
sem_t *semaphore;
// sem_t *sem_child;
pid_t child_pid;

volatile sig_atomic_t sets_processed = 0;

void cleanup(int sig) {
    printf("\nОбработано наборов: %d\n", sets_processed);

    munmap(shm, sizeof(shm_t));
    shm_unlink(SHM_NAME);

    sem_close(semaphore);
    // sem_close(sem_child);
    sem_unlink(SEMAPHORE);
    // sem_unlink(SEM_CHILD);

    kill(child_pid, SIGTERM);

    exit(0);
}

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(shm_t));

    shm = mmap(NULL, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    semaphore = sem_open(SEMAPHORE, O_CREAT, 0666, 0);
    // sem_child  = sem_open(SEM_CHILD,  O_CREAT, 0666, 0);

    signal(SIGINT, cleanup);

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

        sem_post(semaphore);
        sleep(1);
        sem_wait(semaphore);
        
        printf("min = %d, max = %d\n", shm->min_val, shm->max_val);
        sets_processed++;
    }

    return 0;
}
