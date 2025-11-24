#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_MTEXT 256
#define SERVER 10

int myid;
int que;

struct msgbuf {
    long mType;
    int mSender;
    char mText[MAX_MTEXT];
};

void* listener() {
    struct msgbuf msg;
    while (1) {
        if (msgrcv(que, &msg, sizeof(msg) - sizeof(long), myid, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }
        printf("\n[from %d] %s\n> ", msg.mSender, msg.mText);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_id>\n", argv[0]);
        return 1;
    }

    myid = atoi(argv[1]);
    key_t key;
    struct msgbuf msg;
    struct msqid_ds msgCtlBuf;

    if ((key = ftok("server", 6)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }
    if ((que = msgget(key, IPC_CREAT)) == -1) {
        perror("msgget failed:: ");
        exit(2);
    }

    pid_t forked = fork();
    switch(forked) {
        case -1:
            perror("FE");
            exit(3);
        case 0:
            listener();
            exit(0);
        default:
    }

    printf("Client %d started\n", myid);

    while (1) {
        struct msgbuf msg = {0};
        msg.mType = SERVER;
        msg.mSender = myid;

        printf("> ");
        fgets(msg.mText, sizeof(msg.mText), stdin);
        msg.mText[strcspn(msg.mText, "\n")] = 0;

        msgsnd(que, &msg, sizeof(msg) - sizeof(long), 0);

        if (strcmp(msg.mText, "shutdown") == 0) {
            break;
        }
    }

    return 0;
}