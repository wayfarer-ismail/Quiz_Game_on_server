//
// Created by ismail on 08/04/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVERIP "127.0.0.1"
#define BACKLOG 10
#define BUFSIZE 320

void readfromcl(char* buf, int cfd);
void writetocl(char* buf, int cfd);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port number>.\n",
                argv[0]);
        exit(-1);
    }

    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVERIP);
    serverAddress.sin_port = htons(atoi(argv[1]));

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        fprintf(stderr, "socket() error.\n");
        exit(-1);
    }

    /*
     * This socket option allows you to reuse the server endpoint
     * immediately after you have terminated it.
     */
    int optval = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,
                   &optval, sizeof(optval)) == -1)
        exit(-1);

    int rc = bind(lfd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));
    if (rc == -1) {
        fprintf(stderr, "bind() error.\n");
        exit(-1);
    }

    if (listen(lfd, BACKLOG) == -1)
        exit(-1);

    fprintf(stdout, "Listening on (%s, %s)\n", SERVERIP, argv[1]);

    for (;;) /* Handle clients iteratively */
    {
        fprintf(stdout, "<waiting for clients to connect>\n");
        fprintf(stdout, "<ctrl-C to terminate>\n");

        struct sockaddr_storage claddr;
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        int cfd = accept(lfd, (struct sockaddr *)&claddr, &addrlen);
        if (cfd == -1) {
            continue;   /* Print an error message */
        }

        {
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                            host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
                fprintf(stdout, "Connection from (%s, %s)\n", host, service);
            else
                fprintf(stderr, "Connection from (?UNKNOWN?)");
        }

        char welcome[] = "Welcome to Unix Programming Quiz! \nThe quiz comprises five questions posed to you one after the other." 
                        "You have only one attempt to answer a question." 
                        "Your final score will be sent to you after conclusion of the quiz." 
                        "To start the quiz, press Y and <enter>." 
                        "To quit the quiz, press q and <enter>.\r";

        // for(int i=0; i<strlen(welcome)/BUFSIZE; i++) {
        //     writetocl(welcome + i*BUFSIZE, cfd);
        // }
        writetocl(welcome, cfd);

        char buf[BUFSIZE];
        readfromcl(buf, cfd);

        writetocl(buf, cfd);

        if (close(cfd) == -1) /* Close connection */
        {
            fprintf(stderr, "close error.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (close(lfd) == -1) /* Close listening socket */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void readfromcl(char* buf, int cfd) {
    size_t totRead;
    char* bufr = buf;
    for (totRead = 0; totRead < BUFSIZE; ) {
        ssize_t numRead = read(cfd, bufr, BUFSIZE - totRead);
        if (numRead == 0)
            break;
        if (numRead == -1) {
            if (errno == EINTR)
                continue;
            else {
                fprintf(stderr, "Read error.\n");
            }
        }
        totRead += numRead;
        bufr += numRead;
    }
    printf("Received %s\n", buf);
}

void writetocl(char* buf, int cfd) {
    size_t totWritten;
    const char* bufw = buf;
    for (totWritten = 0; totWritten < BUFSIZE; ) {
        ssize_t numWritten = write(cfd, bufw, BUFSIZE - totWritten);
        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;
            else {
                fprintf(stderr, "Write error.\n");
                exit(EXIT_FAILURE);
            }
        }
        totWritten += numWritten;
        bufw += numWritten;
    }
}