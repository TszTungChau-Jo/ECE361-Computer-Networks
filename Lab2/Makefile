CC = gcc
CFLAGS = -Wall -Wextra

DELIVER_SRC = s.c
SERVER_SRC = c.c

DELIVER_OUT = client
SERVER_OUT = server

all: $(DELIVER_OUT) $(SERVER_OUT)

$(DELIVER_OUT): $(DELIVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_OUT): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(DELIVER_OUT) $(SERVER_OUT)
.PHONY: all clean
