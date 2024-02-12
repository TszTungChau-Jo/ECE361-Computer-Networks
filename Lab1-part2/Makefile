CC = gcc
CFLAGS = -Wall -Wextra

DELIVER_SRC = deliver/deliver.c
SERVER_SRC = server/server.c

DELIVER_OUT = deliver/deliver.exe
SERVER_OUT = server/server.exe

all: $(DELIVER_OUT) $(SERVER_OUT)

$(DELIVER_OUT): $(DELIVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_OUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(DELIVER_OUT) $(SERVER_OUT)
.PHONY: all clean
