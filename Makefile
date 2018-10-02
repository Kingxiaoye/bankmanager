all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c -lpthread
	chmod +x client server
	git add .
	git commit -m "kk"
	git push -u origin master