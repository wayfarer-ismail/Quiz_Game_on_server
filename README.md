# TCP Quiz Application
This project is a Quiz Application that uses a TCP socket client and a TCP socket server. The client and server use internet domain stream sockets.

### Requirements
The project requires the following:

server.c source file\
client.c source file\
QuizDB.h header file


### How it works
The server program takes two arguments, an IPv4 address and a port number, then binds to the socket address created using the two input arguments. It waits for client connections, and after successful connection establishment with a client, it starts the quiz. The server randomly selects five questions from the quiz database. The quiz comprises five questions posed to the client one after the other. The client is allowed only one attempt to answer a question. At the end of the quiz, the server sends the final score to the client. 

The client program takes two arguments, an IPv4 address and a port number, that contain the details of the server’s listening endpoint. It connects to the server using the two input arguments. It sends Y or q to the server to start the quiz or quit. For each question sent by the server, the client must accept the answer from the standard input, and the answer is sent to the server. After the conclusion of the quiz, the client will close the connection to the server.

### How to run
To compile the project, run the make command in the src directory.
```bash
make
```
To start the server, run the following command:

```bash
./server <IPv4 address> <port number>
```
To start the client, run the following command:

```bash
./client <IPv4 address> <port number>
```
Press Y to start the quiz and q to quit. Answer each question within one attempt only. \
After the conclusion of the quiz, the server will send the client's final score.

run the following command to clean the project:
```bash
make clean
```