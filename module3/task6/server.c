#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#define MAX_MTEXT 256
#define SERVER 10

struct msgbuf {
    long mType;
    int mSender;
    char mText[MAX_MTEXT];
};

int clients[100];
int nclients = 0;
int que;

void add_client(int id) {
    for (int i = 0; i < nclients; i++)
        if (clients[i] == id) return;
    clients[nclients++] = id;
    printf("Client %d connected\n", id);
}

void remove_client(int id) {
    for (int i = 0; i < nclients; i++) {
        if (clients[i] == id) {
            clients[i] = clients[nclients-1];
            nclients--;
            return;
        }
    }
}

void int_handler(int sig) {
    if (msgctl(que, IPC_RMID, NULL) == -1) {
        perror("msgctl failed:: ");
        exit(4);
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    key_t key;
    struct msgbuf msg;
    

    if ((key = ftok("server", 6)) == -1) {
        perror("ftok failed:: ");
        exit(1);
    }
    if ((que = msgget(key, IPC_CREAT)) == -1) {
        perror("msgget failed:: ");
        exit(2);
    }

    printf("Server started. Queue id = %d\n", que);
    signal(SIGINT, int_handler);

    while (1) {
        if (msgrcv(que, &msg, sizeof(msg) - sizeof(long), SERVER, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }

        int from = msg.mSender;
        add_client(from);

        printf("From %d: %s\n", from, msg.mText);

        if (strcmp(msg.mText, "shutdown") == 0) {
            printf("Client %d disconnected\n", from);
            remove_client(from);
            continue;
        }

        for (int i = 0; i < nclients; i++) {
            if (clients[i] == from) continue;

            struct msgbuf out;
            out.mType = clients[i];
            out.mSender = from;
            strcpy(out.mText, msg.mText);

            msgsnd(que, &out, sizeof(out) - sizeof(long), 0);
        }
    }
    return 0;
}