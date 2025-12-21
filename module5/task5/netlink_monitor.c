#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/net_namespace.h>

#define NETLINK_USER 31
struct sock *nl_sk = NULL;

void reverse_string(char *str)
{
    int i = 0;
    int j = strlen(str) - 1;
    char tmp;

    while (i < j)
    {
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;

        i++;
        j--;
    }
}

static void hello_nl_recv_msg(struct sk_buff *skb)
{

    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int msg_size;
    char *msg;
    int res;

    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "netlink_monitor: recived: %s\n", (char *)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid;

    msg = (char *)nlmsg_data(nlh);
    reverse_string(msg);
    msg_size = strlen(msg);

    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out)
    {
        printk(KERN_ERR "netlink_monitor: Failed to allocate new skb\n");
        return;
    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0;
        strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);

    if (res < 0) printk(KERN_INFO "netlink_monitor: Error while sending back to user\n");
}

struct netlink_kernel_cfg cfg = {
   .groups  = 1,
   .input = hello_nl_recv_msg,
};

static int __init hello_init(void)
{
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk)
    {
        printk(KERN_ALERT "netlink_monitor: Error creating socket.\n");
        return -10;
    }

    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "netlink_monitor: exiting module\n");
    netlink_kernel_release(nl_sk);
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Ikadzev");
MODULE_DESCRIPTION("Netlink monitor module");
MODULE_LICENSE("GPL");
