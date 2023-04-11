//
// Created by ismail on 08/04/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "io.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP address of server> <port number>.\n", argv[0]);
        exit(-1);
    }

    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(atoi(argv[2]));

    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1) {
        fprintf(stderr, "socket() error.\n");
        exit(-1);
    }

    int rc = connect(cfd, (struct sockaddr *)&serverAddress,
                     sizeof(struct sockaddr));
    if (rc == -1) {
        fprintf(stderr, "connect() error, errno %d.\n", errno);
        exit(-1);
    }

    {
        char buf[BUFSIZE];
        readfromcl(buf, cfd);
        printf("Received %s\n", buf);

        
        printf("Enter your answer: ");
        fgets(buf, BUFSIZE, stdin);
        buf[BUFSIZE-1] = '\0';

        printf("Sending %s to localhost:%s\n", buf, argv[2]);


        writetocl(buf, cfd);

        readfromcl(buf, cfd);
        printf("Received %s\n", buf);
    }

    if (close(cfd) == -1) /* Close connection */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
