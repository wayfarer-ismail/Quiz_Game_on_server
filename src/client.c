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

#define BUFSIZE 32

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
        int c;
        srand(time(NULL));
        for (c = 0; c < BUFSIZE-1; c++)
        {
            buf[c] = 'A' + (random() % 26);
        }
        buf[BUFSIZE-1] = '\0';

        printf(
                "Sending %s to localhost:%s\n",
                buf, argv[2]);

        size_t totWritten;
        const char* bufw = buf;
        for (totWritten = 0; totWritten < BUFSIZE; ) {
            ssize_t numWritten = write(cfd, bufw, BUFSIZE - totWritten);
            if (numWritten <= 0) {
                if (numWritten == -1 && errno == EINTR)
                    continue;
                else {
                    fprintf(stderr, "Write error. Errno %d.\n", errno);
                }
            }
            totWritten += numWritten;
            bufw += numWritten;
        }

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
                    fprintf(stderr, "Read error, Errno %d.\n", errno);
                }
            }
            totRead += numRead;
            bufr += numRead;
        }
        printf("Received %s\n", buf);
    }

    if (close(cfd) == -1) /* Close connection */
    {
        fprintf(stderr, "close error.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
