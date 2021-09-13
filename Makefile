#	//~~ Makefile ~~//

CFLAGS = -Wall -g

all: server client subscriber

server: server.c struct.c list.c

client: client.c struct.c list.c

subscriber: subscriber.c struct.c list.c

.PHONY: clean

clean: 
	rm -rf server client subscriber
