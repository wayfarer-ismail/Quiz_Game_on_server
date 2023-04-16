#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include "io.h"

void readfromfile(char* buf, int cfd) {
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
}

void writetofile(char* buf, int cfd) {
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