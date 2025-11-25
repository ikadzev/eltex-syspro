#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#define MAX_STR 128
#define QUEUE_A_TO_B "/queueAtoB"
#define QUEUE_B_TO_A "/queueBtoA"

int main() {
    mqd_t qAB = mq_open(QUEUE_A_TO_B, O_RDONLY | O_CREAT, 0666, NULL);
    mqd_t qBA = mq_open(QUEUE_B_TO_A, O_WRONLY | O_CREAT, 0666, NULL);

    if (qAB == -1 || qBA == -1) {
        perror("mq_open");
        exit(1);
    }

    char buff[128];
    unsigned prio;

    while (1) {
        ssize_t n = mq_receive(qAB, buff, MAX_STR, &prio);
        if (n == -1) {
            perror("mq_receive");
            break;
        }

        printf("[A] %s\n", buff);
        if (prio == 10) break;

        printf("> ");
        fgets(buff, MAX_STR, stdin);
        buff[strcspn(buff, "\n")] = 0;
        mq_send(qBA, buff, strlen(buff) + 1, strcmp(buff, "10") ? 1 : 10);
        if (strcmp(buff, "10") == 0) break;
    }

    mq_close(qAB);
    mq_close(qBA);

    return 0;
}
