#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int sockfd;

void ender(int sig) {
    close(sockfd);
    exit(0);
}

int main() {
    int n;
    char sendline[1000], recvline[1000];
    struct sockaddr_in servaddr, cliaddr;
    
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(12345);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr*) &cliaddr, sizeof(cliaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(54321);

    if(inet_aton("127.0.0.1", &servaddr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }

    pid_t frk = fork();

    if (frk == 0) {
        signal(SIGUSR1, ender);
        while (1) {
            if ((n = recvfrom(sockfd, recvline, 999, 0, (struct sockaddr*) NULL, NULL)) < 0) {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
            recvline[n] = '\0';
            printf("\nYou got mailed! %s\n> ", recvline);
            fflush(stdout);
        }
    } else if (frk > 0) {
        while (1) {
            printf("> ");
            fflush(stdout);
            fgets(sendline, 1000, stdin);
            sendline[strcspn(sendline, "\n")] = 0;
            if (strcmp(sendline, "exit") == 0) break;
            if (sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                perror(NULL);
                close(sockfd);
                exit(1);
            }
        }
    } else {
        perror("fork");
        close(sockfd);
        exit(1);
    }

    kill(frk, SIGUSR1);
    close(sockfd);
    exit(0);
}
