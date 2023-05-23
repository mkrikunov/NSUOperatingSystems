all: server client

server: server.c
	gcc -lsocket -laio server.c -o server

client: client.c
	gcc -lsocket client.c -o client
