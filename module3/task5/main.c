#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#define COUNTER_FILE "counter.txt"

int COUNT = 0;
int INTCOUNT = 0;
int fd;

void alarm_listener(int sig) {
    char buff[10];
    int len = snprintf(buff, sizeof(buff), "%d\n", COUNT++);
    write(fd, buff, len);
    alarm(1);
}

void int_listener(int sig) {
    if (++INTCOUNT == 3) {
        close(fd);
        exit(0);
    }
    write(fd, "Interupted!\n", 12);
}

void quit_listener(int sig) {
    write(fd, "Interupted to the DEATH!", 24);
    close(fd);
    exit(0);
}

int main() {
    fd = open(COUNTER_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    signal(SIGALRM, alarm_listener);
    signal(SIGINT, int_listener);
    signal(SIGQUIT, quit_listener);
    raise(SIGALRM);
    while(1) pause;
}