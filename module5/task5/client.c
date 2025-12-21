#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <unistd.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;
char output_buffer[1024];

void main()
{
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return;

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;

    while (1) {
        nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();
        nlh->nlmsg_flags = 0;

        printf("> ");
        fflush(stdout);
        fgets(output_buffer, 1024, stdin);
        if (strcmp(output_buffer, "close\n") == 0)
            break;
        output_buffer[strcspn(output_buffer, "\n")] = 0;
        strncpy(NLMSG_DATA(nlh), output_buffer, MAX_PAYLOAD);

        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        sendmsg(sock_fd, &msg, 0);
        recvmsg(sock_fd, &msg, 0);
        
        printf("> %s\n", (char *)NLMSG_DATA(nlh));
        free(nlh);
    }
    close(sock_fd);
}
