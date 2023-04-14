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
#include "QuizDB.h"
#include "io.h"

#define SERVERIP "127.0.0.1"
#define BACKLOG 10

void startgame(int cfd);

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

        int pid = fork();
        if (pid == -1) {
            fprintf(stderr, "fork() error.\n");
            exit(-1);
        } else if (pid == 0) { //child process
            startgame(cfd);    //service client and move on to next client
        }

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

void startgame(int cfd) {
    char welcome[] = "Welcome to Unix Programming Quiz! \nThe quiz comprises five questions posed to you one after the other." 
                "\nYou have only one attempt to answer a question." 
                "\nYour final score will be sent to you after conclusion of the quiz." 
                "\nTo start the quiz, press Y and <enter>." 
                "\nTo quit the quiz, press q and <enter>.\r";

    writetocl(welcome, cfd);

    char buf[BUFSIZE];
    readfromcl(buf, cfd); //read user choice to start or quit
    printf("Received %s\n", buf);

    switch (buf[0]) {
        case 'Y':
            break;
        case 'q':
            exit(0);
        default:
            printf("Invalid input.");
            return;
    }

    int score = 0;
    for (int i=0; i<5; i++) {
        //generate a random number between 1 and 43
        int qid = rand() % NUM_QUESTIONS;

        writetocl(QuizQ[qid], cfd);
        readfromcl(buf, cfd);
        if (buf[strlen(buf)-1] == '\n') { //remove trailing newline
            buf[strlen(buf)-1] = '\0';
        }
        printf("Received %s\n", buf);

        if (strcmp(buf, QuizA[qid]) == 0) {
            score++;
            writetocl("Right Answer.", cfd);
        } else {
            char wrongAnsMsg[100];
            sprintf(wrongAnsMsg, "Wrong Answer. Right answer is %s", QuizA[qid]);
            writetocl(wrongAnsMsg, cfd);
        }
    }

    char finalScoreMsg[50];
    sprintf(finalScoreMsg, "Your quiz score is %d/5. Goodbye!", score);
    writetocl(finalScoreMsg, cfd);
}