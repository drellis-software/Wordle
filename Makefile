
all: library server client 

library:
	gcc -c socketUtilities.c
	ar rc libsocketUtilities.a socketUtilities.o
server: server.c 
	gcc server.c -o server socketUtilities.o -L. -lsocketUtilities 

client: client.c
	gcc client.c -o client socketUtilities.o -L. -lsocketUtilities 

