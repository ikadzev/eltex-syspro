#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#define MAX_STR 128
#define QUEUE_A_TO_B "/queueAtoB"
#define QUEUE_B_TO_A "/queueBtoA"

int main() {
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_STR;
    
    mqd_t qAB = mq_open(QUEUE_A_TO_B, O_WRONLY | O_CREAT, 0666, &attr);
    mqd_t qBA = mq_open(QUEUE_B_TO_A, O_RDONLY | O_CREAT, 0666, &attr);

    if (qAB == -1 || qBA == -1) {
        perror("mq_open");
        exit(1);
    }

    char buff[MAX_STR];
    unsigned prio;

    while (1) {
        printf("> ");
        fgets(buff, MAX_STR, stdin);
        buff[strcspn(buff, "\n")] = 0;
        mq_send(qAB, buff, strlen(buff) + 1, strcmp(buff, "10") ? 1 : 10);
        if (strcmp(buff, "10") == 0) break;

        ssize_t n = mq_receive(qBA, buff, MAX_STR, &prio);
        if (n == -1) {
            perror("mq_receive");
            break;
        }

        printf("[B] %s\n", buff);
        if (prio == 10) break;
    }

    sleep(1);
    mq_close(qAB);
    mq_close(qBA);
    mq_unlink(QUEUE_A_TO_B);
    mq_unlink(QUEUE_B_TO_A);

    return 0;
}
