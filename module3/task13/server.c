#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void dostuff(int); 

void error(const char *msg) { 
    perror(msg);
    exit(1);
}

int nclients = 0;

void printusers() {
    if(nclients) {
        printf("%d user on-line\n", nclients);
    } else {
        printf("No User on line\n");
    }
}

typedef double (*funker)(double, double);

typedef struct {
    char chara;
    funker func;
} Operation;

double i_add(double arg1, double arg2) { return arg1 + arg2; }
double i_sub(double arg1, double arg2) { return arg1 - arg2; }
double i_mul(double arg1, double arg2) { return arg1 * arg2; }
double i_div(double arg1, double arg2) { return arg1 / arg2; }

void get_file(int sock, char *filename);

Operation ops[] = {
    {'+', i_add},
    {'-', i_sub},
    {'*', i_mul},
    {'/', i_div}
};

int main(int argc, char *argv[]) {
    char buff[1024];
    int sockfd, newsockfd;
    int portno;
    int pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    printf("TCP SERVER DEMO\n");

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");
        nclients++;

        struct hostent *hst;

        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect!\n", (hst) ? hst->h_name : "Unknown host", (char*)inet_ntoa(cli_addr.sin_addr));
        printusers();

        pid = fork();
        if (pid < 0) error("ERROR on fork");
        if (pid == 0) {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else {
            close(newsockfd);
            nclients--;
        }
    }
    close(sockfd);
    return 0;
}

void dostuff (int sock) {
    int bytes_recv;
    double a,b;
    funker operation = NULL;
    char buff[20 * 1024];

    #define str0 "Enter function (+,-,*,/,quit,file)\r\n"
    #define str1 "Enter 1 parameter\r\n"
    #define str2 "Enter 2 parameter\r\n"
    #define strerr "Wrong function\r\n"

    write(sock, str0, sizeof(str0));

    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    switch (buff[0])
    {
    case '+':
        operation = i_add;
        break;
    case '-':
        operation = i_sub;
        break;
    case '*':
        operation = i_mul;
        break;
    case '/':
        operation = i_div;
        break;
    case 'f':
        get_file(sock, &buff[0]);
        return;
    default:
        write(sock, strerr, sizeof(strerr));
    case 'q':
        nclients--;
        printf("-disconnect\n");
        printusers();
        return;
    }

    write(sock,str1,sizeof(str1));
    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    a = atof(buff);

    write(sock,str2,sizeof(str2));
    bytes_recv = read(sock,&buff[0],sizeof(buff));
    if (bytes_recv < 0) error("ERROR reading from socket");
    b = atof(buff);
    
    snprintf(buff, sizeof(buff), "%.2f", operation(a, b));
    buff[strlen(buff)] = '\n';

    write(sock,&buff[0], sizeof(buff));
    nclients--;
    printf("-disconnect\n");
    printusers();
    return;
}

void get_file(int sock, char *buff) {
    char fname[256];
    int filesize;
    sscanf(buff, "f %s %d", fname, &filesize);
    printf("Receiving file '%s', size %d bytes\n", fname, filesize);

    FILE *fp = fopen(fname, "wb");
    if (fp == NULL) {
        error("ERROR opening file");
        return;
    }

    int received = 0;
    while (received < filesize) {
        int n = read(sock, buff, 1024);
        if (n < 0) {
            error("ERROR reading from socket");
            fclose(fp);
            return;
        }
        fwrite(buff, 1, n, fp);
        received += n;
    }

    fclose(fp);
    printf("Received file: %s (%d bytes)\n", fname, filesize);
    nclients--;
    printf("-disconnect\n");
    printusers();
}