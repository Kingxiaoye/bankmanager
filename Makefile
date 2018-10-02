all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c -lpthread
	chmod +x client server