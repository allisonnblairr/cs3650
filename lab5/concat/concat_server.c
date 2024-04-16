
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// helper functions implemented in util.c

/* This function initializes the given "struct sockaddr".
 * For servers, set "ip" to "NULL"; for clients, set "ip" to the server's IP
 */
void init_sockaddr(struct sockaddr *in_addr, const char *ip, int port);

/* This function fills in "len" number of random chars to "str_buf" and a tailing '\0'.
 * The function assumes that "str_buf" points to a memory of size at lease "len+1".
 */
void rand_str(char *str_buf, int len);


void server(int port) {
    /*
     * Server should establish a listening socket and accept
     * connections from clients.
     * Here is HOWTO:
     *  - Create a socket for the server on localhost
     *  - Create "struct sockaddr" and use "init_sockaddr()" to initialize it
     *  - Bind the socket to the server port
     *  - Put server's socket in LISTEN mode
     *  - Call accept to wait for a connection
     *  - When encountering errors, print errors using "perror" and quit
     * */

    /* TODO: your code here */
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
       perror("socket");
       exit(EXIT_FAILURE);
    }
    struct sockaddr addr;
    init_sockaddr(&addr, NULL, port);
    if (bind(socketfd, &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(socketfd, 128) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr clientaddr;
    socklen_t addr_size = sizeof(clientaddr);
    int new_fd = accept(socketfd, &clientaddr, &addr_size);
    if (new_fd < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while(1) {
        /*
         * Here, server receives a string, concatenates a 10-char string, and
         * returns the new string.
         * Here is HOTWO:
         *  - Receive data over the socket returned by the accept() method
         *    -- Exit the loop when client closes connection
         *  - Generate a new string of length 10 using rand_str()
         *    -- pay attention to the tailing '\0'
         *  - Append the string to the data received
         *    -- "strcat" is useful
         *  - Send the new string back to the client
         *  - When encountering errors, print errors using "perror" and quit
         */

        /* TODO: your code here */
        int bytes_rec, bytes_sent;
        char buf[1024];
        bytes_rec = recv(new_fd, buf, 1024, 0);
        if (bytes_rec < 0) {
            perror("recv");
            exit(EXIT_FAILURE);
        } else if (bytes_rec == 0) {
                   break;
        }
        buf[bytes_rec] = '\0';
        char str_buf[11];
        rand_str(str_buf, 10);
        strcat(buf, str_buf);
        bytes_sent = send(new_fd, buf, strlen(buf), 0);
        if (bytes_sent < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
     }

    // TODO: close all sockets that were created
     close(socketfd);
     close(new_fd);
}


int main(int argc, char *argv[]) {
    if (argc < 2 || argc >3) {
        printf("Usage: concat_server server_port [random_seed]\n");
        exit(1);
    }

    if (argc == 3) {
        srandom(atoi(argv[2]));
    } else {
        srandom(3650);
    }

    int port = atoi(argv[1]);
    server(port);
}

