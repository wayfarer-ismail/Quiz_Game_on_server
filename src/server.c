//
// Created by ismail on 08/04/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include "QuizDB.h"
#include "io.h"

#define BACKLOG 10

void startgame(int cfd);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server ip> <port number>.\n", argv[0]);
        exit(-1);
    }

    const char* SERVERIP = argv[1];
    struct sockaddr_in serverAddress;

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));

    serverAddress.sin_family = AF_INET;
    //validate and assign server ip
    if (inet_pton(AF_INET, SERVERIP, &serverAddress.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address\n");
        exit(1);
    }
    serverAddress.sin_port = htons(atoi(argv[2]));

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

    fprintf(stdout, "Listening on <%s:%s>\n", SERVERIP, argv[2]);

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
    char welcome[] = "Welcome to Unix Programming Quiz! "
                "\nThe quiz comprises five questions posed to you one after the other." 
                "\nYou have only one attempt to answer a question." 
                "\nYour final score will be sent to you after conclusion of the quiz." 
                "\nTo start the quiz, press Y and <enter>." 
                "\nTo quit the quiz, press q and <enter>.\r";

    writetofile(welcome, cfd);

    char buf[BUFSIZE];
    readfromfile(buf, cfd); //read user choice to start or quit
    printf("Received %s\n", buf);

    switch (buf[0]) {
        case 'Y':
            break;   //continue to quiz
        case 'q':
            _exit(0); //kill child process, main server still lives
        default:
            printf("Invalid input."); //undefined behavior
            return;
    }

    int score = 0;
    srand(time(NULL)); //seed random number generator
    for (int i=0; i<5; i++) {
        //generate a random number between 0 and 43
        int qid = rand() % NUM_QUESTIONS;

        writetofile(QuizQ[qid], cfd);
        readfromfile(buf, cfd);
        if (buf[strlen(buf)-1] == '\n') { //remove trailing newline
            buf[strlen(buf)-1] = '\0';
        }
        printf("Received %s\n", buf);

        if (strcmp(buf, QuizA[qid]) == 0) {
            score++;
            writetofile("Right Answer.", cfd);
        } else {
            char wrongAnsMsg[100];
            sprintf(wrongAnsMsg, "Wrong Answer. Right answer is %s", QuizA[qid]);
            writetofile(wrongAnsMsg, cfd);
        }
    }

    char finalScoreMsg[50];
    sprintf(finalScoreMsg, "Your quiz score is %d/5. Goodbye!", score);
    writetofile(finalScoreMsg, cfd);
}