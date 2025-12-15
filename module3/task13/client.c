#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define h_addr h_addr_list[0]

int my_sock, portno, n;

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void send_file(char *filename) {
    char header[256];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("ERROR opening file");
        return;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    snprintf(header, sizeof(header), "f %s %ld", filename, size*sizeof(char));
    send(my_sock, header, sizeof(header), 0);
    printf("Sending file %s...\n", filename);

    sleep(1);

    char buff[1024];
    size_t nread;
    while ((nread = fread(buff, 1, sizeof(buff), fp)) > 0) {
        send(my_sock, buff, nread, 0);
    }
    printf("Sended!\n");
    fclose(fp);
    close(my_sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[1024];

    printf("TCP DEMO CLIENT\n");

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n",
        argv[0]);
        exit(0);
    }
    
    portno = atoi(argv[2]);

    my_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (my_sock < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(my_sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) error("ERROR connecting");

    while ((n = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) > 0){

        buff[n] = 0;

        printf("S=>C:%s", buff);

        printf("S<=C:");
        fgets(&buff[0], sizeof(buff) - 1, stdin);

        if (!strcmp(&buff[0], "quit\n")) {
            send(my_sock, &buff[0], strlen(&buff[0]), 0);
            printf("Exit...");
            close(my_sock);
            return 0;
        }

        if (strncmp(buff, "file", 4) == 0) {
            char fname[256];
            sscanf(buff, "file %s", fname);
            send_file(fname);
            continue;
        }

        send(my_sock, &buff[0], strlen(&buff[0]), 0);
    }
    printf("Recv error \n");
    close(my_sock);
    return 1;
}
