#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// implemented in util.c
extern void init_sockaddr(struct sockaddr *in_addr, const char *ip, int port);


char *bad_words[3] =  {"virus", "worm", "malware"};
char *good_words[3] = {"groot", "hulk", "ironman"};

/* Replace bad words in "str" to the corresponding good words.
 * - Bad/good words are defined in "bad_words"/"good_words" (arrays defined above)
 * - Partial word does not count: for example, you can ignore "malwareXYZ".
 * - You should return a string with replaced words.
 * - hints:
 *   -- strtok() is useful
 *   -- Notice that the corresponding bad and good words are of the same length.
 */
char *replace_bad_words(char *str) {
    /* TODO: your code here */
    const char sep[] = " ";
    char *token = strtok(str, sep);
    while (token != NULL) {
           for (int i = 0; i < 3; i++) {
                if (strcmp(token, bad_words[i]) == 0) {
                    strcpy(token, good_words[i]);
                    break;
                }
           }
           token = strtok(NULL, sep);
    }
    return str;
}


/* the chat server will accept two clients and forward messages between them. */
void server(int port) {
    /* For server:
     *  - Create a socket to listen on port for new connections.
     *  - Bind the server's socket to port.
     *  - Put listener_socket in LISTEN mode
     *  - Accept two connections from two clients
     *  - When encountering errors, print errors using "perror" and quit
     */
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
    if (listen(socketfd, 2) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int fds[2] = {0, 0};

    struct sockaddr clientaddr;
    socklen_t addr_size = sizeof(clientaddr);
    fds[0] = accept(socketfd, &clientaddr, &addr_size);
    if (fds[0] < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    //struct sockaddr clientaddr2;
    //socklen_t addr_size2 = sizeof(clientaddr2);
    fds[1] = accept(socketfd, &clientaddr, &addr_size);
    if (fds[1] < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    fd_set readfds;

    while(1) {
        /* Next:
         *  - Use "select" to see which socket is available to read from
         *    -- we don't care about writefds and exceptfds; we don't timeout
         *    -- remember to clear "fd_set"
         *  - Check to see if connection is closed
         *  - Replace bad words using "replace_bad_words()"
         *  - Forward to the other socket
         *  - Quit when either client closes the socket
         *  - When encountering errors, print errors using "perror" and quit
         */

        /* TODO: your code here */
        char buf[1024];
        int bytes_rec, bytes_sent;
        FD_ZERO(&readfds);
        for (int i = 0; i < 2; i++) {
             FD_SET(fds[i], &readfds);
        }

        int maxfd;
        if (fds[0] > fds[1]) {
           maxfd = fds[0];
        } else {
           maxfd = fds[1];
        }

        if((select(maxfd+1, &readfds, NULL, NULL, NULL)) < 0) {
           perror("select");
           exit(EXIT_FAILURE);
        }
        int destfd;
        for (int i = 0; i < 2; i++) {
             if (FD_ISSET(fds[i], &readfds)) {
                if (i == 0) {
                   destfd = 1;
                } else {
                   destfd = 0;
                }
                bytes_rec = recv(fds[i], buf, sizeof(buf), 0);
                if (bytes_rec < 0) {
                    perror("recv");
                    exit(EXIT_FAILURE);
                } else if (bytes_rec == 0) {
                           close(fds[i]);
                           close(fds[destfd]);
                           close(socketfd);
                           exit(0);
                }
                buf[bytes_rec] = '\0';
                //char *new_str = replace_bad_words(buf);
                bytes_sent = send(fds[destfd], buf, sizeof(buf), 0);
                if (bytes_sent < 0) {
                    perror("recv");
                    exit(EXIT_FAILURE);
                }
             }
         }

    }

    // TODO: close created sockets
     close(socketfd);
     close(fds[0]);
     close(fds[1]);

}


int main(int argc, char *argv[]) {
    if (argc < 2 || argc >3) {
        printf("Usage: chat_server server_port\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    server(port);
}
