all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c -lpthread
	chmod +x client server
	git add .
	git commit -m "模糊查询实现前"
	git push -u origin master
	#git log   查看更新日志