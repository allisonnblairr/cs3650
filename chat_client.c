#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// implemented in util.c
extern void init_sockaddr(struct sockaddr *in_addr, const char *ip, int port);

// printing function; please don't touch
void print_msg(char *msg) {
    printf("%s\n", msg);
}


/* Connect this client to the chat server and wait user's input from stdin */
void client(char *ip, int port) {
    /*
     * Here is HOWTO:
     *  - Create a socket for the client
     *  - Connect client to the server at the given port
     *  - When encountering errors, print errors using "perror" and quit
     */
    /* TODO: your code here */
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    struct  sockaddr serv_addr;
    init_sockaddr(&serv_addr, ip, port);
    if (connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;
    // Repeat until server goes down or user stops entering in data
    while(1) {
        /* Next:
         *  - Send data to server if stdin has data (i.e. when user typed in data and pressed enter)
         *  - Receive data from server if socket has data coming in
         *  - When encountering errors, print errors using "perror" and quit
         *  - Print the received message on screen using print_msg().
         */

        /* TODO: your code here */
        char str_buf[1024];
        int bytes_sent, bytes_rec;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        int maxfd;

        if (sockfd > STDIN_FILENO) {
           maxfd = sockfd;
        } else {
           maxfd = STDIN_FILENO;
        }

        if ((select(maxfd+1, &readfds, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
           //fgets(str_buf, sizeof(str_buf), STDIN_FILENO);
           bytes_sent = send(sockfd, str_buf, sizeof(str_buf), 0);
        if (bytes_sent < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
        }

        if (FD_ISSET(sockfd, &readfds)) {
        bytes_rec = recv(sockfd, str_buf, sizeof(str_buf), 0);
        if (bytes_rec < 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        }

        str_buf[bytes_rec] = '\0';
        print_msg(str_buf);

    }

}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Usage: concat_client server_port [server_address]\n");
        exit(1);
    }

    char *ip = "127.0.0.1";
    if (argc > 2) {
        ip = argv[2];
    }
    int port = atoi(argv[1]);
    client(ip, port);
}
