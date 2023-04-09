CC = gcc
CFLAGS = -Wall

SERVER_SRC = src/server.c src/QuizDB.h
CLIENT_SRC = src/client.c
PRINTQUIZ_SRC = src/printquiz.c src/QuizDB.h

all: server client printquiz

server: $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

printquiz: $(PRINTQUIZ_SRC)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f server client printquiz
