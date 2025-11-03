#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

uint32_t ip_str_to_uint(const char *ip_str) {
    uint32_t a, b, c, d;
    sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d);
    return (a << 24) | (b << 16) | (c << 8) | d;
}

void uint_to_ip_str(uint32_t ip, char *buffer) {
    sprintf(buffer, "%u.%u.%u.%u",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF);
}

uint32_t parse_mask(const char *mask_str) {
    if (mask_str[0] == '/') {
        int prefix_len = atoi(mask_str + 1);
        return 0xFFFFFFFF << (32 - prefix_len);
    } else {
        return ip_str_to_uint(mask_str);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: ./main [gateway_ip] [mask (255.255.255.0 или /24)] [N]\n");
        return 0;
    }

    const char *gateway_str = argv[1];
    const char *mask_str = argv[2];
    int N = atoi(argv[3]);

    uint32_t gateway = ip_str_to_uint(gateway_str);
    uint32_t netmask = parse_mask(mask_str);
    uint32_t network = gateway & netmask;

    int local_count = 0, external_count = 0;

    char mask_ip[16];
    uint_to_ip_str(netmask, mask_ip);

    for (int i = 0; i < N; i++) {
        uint32_t random_ip = (uint32_t)rand() << 16 | rand();
        char ip_str[16];
        uint_to_ip_str(random_ip, ip_str);

        int same_subnet = ((random_ip & netmask) == network);
        if (same_subnet) local_count++; else external_count++;

        if (same_subnet) printf("Packet %3d -> %-15s\n", i + 1, ip_str);
    }
    printf("Packets: %d\n", N);
    printf("in own network: %d (%.2f%%)\n", local_count, 100.0 * local_count / N);
    printf("in diff network:  %d (%.2f%%)\n", external_count, 100.0 * external_count / N);

    return 0;
}
