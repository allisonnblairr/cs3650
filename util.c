#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void init_sockaddr(struct sockaddr *in_addr, const char *ip, int port) {
    struct sockaddr_in *addr = (struct sockaddr_in *) in_addr;
    addr->sin_family = AF_INET;
    if (ip == NULL) {
        addr->sin_addr.s_addr = INADDR_ANY;
    } else {
        if(inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
            printf("Error: invalid address or address not supported\n");
            exit(1);
        }
    }
    addr->sin_port = htons(port);
}

void rand_str(char *str_buf, int len) {
    int A_pos = 65;
    int z_pos = 122;
    for (int i=0; i<len; i++) {
        unsigned int rand = random();
        char c = rand % (z_pos - A_pos) + A_pos;
        str_buf[i] = c;
    }
    str_buf[len] = '\0';
}
