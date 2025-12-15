#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>

#define SERVER_PORT 54321
#define BUF_SIZE 65536

int main() {
    int sockfd;
    unsigned char buffer[BUF_SIZE];

    FILE *dump = fopen("udp_dump.bin", "wb");
    if (!dump) {
        perror("fopen");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    printf("listening port %d\n", SERVER_PORT);

    while (1) {
        ssize_t data_size = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
        if (data_size < 0) {
            perror("recvfrom");
            break;
        }
        
        struct iphdr *ip = (struct iphdr *)buffer;
        struct udphdr *udp = (struct udphdr *)(buffer + ip->ihl * 4);

        int src_port = ntohs(udp->source);
        int dst_port = ntohs(udp->dest);

        if (dst_port == SERVER_PORT) {
            fwrite(buffer, 1, data_size, dump);
            fflush(dump);

            unsigned char *payload = buffer + ip->ihl * 4 + sizeof(*udp);
            int payload_len = data_size - ip->ihl * 4 - sizeof(*udp);

            printf("sender: %d\n", src_port);

            printf("text: ");
            fwrite(payload, 1, payload_len, stdout);
            printf("\n\n");

            fwrite(payload, 1, payload_len, dump);
            fflush(dump);
        }
    }

    close(sockfd);
    fclose(dump);
    return 0;
}
