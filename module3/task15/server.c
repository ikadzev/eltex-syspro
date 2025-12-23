#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CLIENTS     FD_SETSIZE
#define BUF_SIZE        4096
#define ENTER_CMD       "Enter function (+,-,*,/,q,f)\n"
#define ENTER_A         "Enter A\n"
#define ENTER_B         "Enter B\n"
#define WRONG_OP        "Wrong operation\n"

typedef double (*funker)(double,double);

double i_add(double a,double b){return a+b;}
double i_sub(double a,double b){return a-b;}
double i_mul(double a,double b){return a*b;}
double i_div(double a,double b){return a/b;}

typedef enum {
    ST_WAIT_CMD,
    ST_WAIT_A,
    ST_WAIT_B,
    ST_FILE_RECV
} client_state;

typedef struct {
    int fd;
    client_state state;
    funker op;
    double a, b;
    FILE *fp;
    int file_left;
} client_t;

client_t clients[MAX_CLIENTS];

void client_init(client_t *c) {
    c->fd = -1;
    c->state = ST_WAIT_CMD;
    c->op = NULL;
    c->fp = NULL;
    c->file_left = 0;
}

void client_close(client_t *c) {
    if (c->fp) fclose(c->fp);
    close(c->fd);
    client_init(c);
}

void client_work(fd_set rfds, int fd_id) {
    client_t *c = &clients[fd_id];
    if (c->fd == -1) return;
    if (!FD_ISSET(c->fd,&rfds)) return;

    char buf[BUF_SIZE];
    int n = read(c->fd,buf,sizeof(buf)-1);
    if (n <= 0) {
        client_close(c);
        return;
    }
    buf[n]=0;

    switch (c->state) {
    case ST_WAIT_CMD:
        if (buf[0]=='q') {
            client_close(c);
            break;
        }
        if (buf[0]=='f') {
            char fname[256];
            sscanf(buf,"f %s %d",fname,&c->file_left);
            c->fp = fopen(fname,"wb");
            c->state = ST_FILE_RECV;
            break;
        }
        switch (buf[0]) {
        case '+': c->op=i_add; break;
        case '-': c->op=i_sub; break;
        case '*': c->op=i_mul; break;
        case '/': c->op=i_div; break;
        default:
            write(c->fd,WRONG_OP,strlen(WRONG_OP));
            break;
        }
        write(c->fd,ENTER_A,strlen(ENTER_A));
        c->state = ST_WAIT_A;
        break;

    case ST_WAIT_A:
        c->a = atof(buf);
        write(c->fd,ENTER_B,strlen(ENTER_B));
        c->state = ST_WAIT_B;
        break;

    case ST_WAIT_B: {
        c->b = atof(buf);
        double r = c->op(c->a,c->b);
        char out[64];
        snprintf(out,sizeof(out),"%.2f\n",r);
        write(c->fd,out,strlen(out));
        client_close(c);
        break;
    }

    case ST_FILE_RECV:
        fwrite(buf,1,n,c->fp);
        c->file_left -= n;
        if (c->file_left <= 0) {
            fclose(c->fp);
            c->fp = NULL;
            client_close(c);
        }
        break;
    }
}

int main(int argc, char *argv[]) {
    int listen_fd;
    struct sockaddr_in addr;

    if (argc < 2) {
        exit(1);
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) perror("socket"), exit(1);

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    bind(listen_fd,(struct sockaddr*)&addr,sizeof(addr));
    listen(listen_fd,5);

    for (int i=0;i<MAX_CLIENTS;i++) client_init(&clients[i]);

    printf("TCP SELECT_SERVER DEMO\n");

    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(listen_fd,&rfds);

        int maxfd = listen_fd;

        for (int i=0;i<MAX_CLIENTS;i++) {
            if (clients[i].fd != -1) {
                FD_SET(clients[i].fd,&rfds);
                if (clients[i].fd > maxfd)
                    maxfd = clients[i].fd;
            }
        }

        if (select(maxfd+1,&rfds,NULL,NULL,NULL) < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (FD_ISSET(listen_fd,&rfds)) {
            int fd = accept(listen_fd,NULL,NULL);
            for (int i=0;i<MAX_CLIENTS;i++) {
                if (clients[i].fd == -1) {
                    clients[i].fd = fd;
                    write(fd,ENTER_CMD,strlen(ENTER_CMD));
                    break;
                }
            }
        }

        for (int i=0;i<MAX_CLIENTS;i++) {
            client_work(rfds,i);
        }
    }
    close(listen_fd);
    return 0;
}
