all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c -lpthread
	chmod +x client server
	git add .
	git commit -m "改密码功能实现"
	git push -u origin master
	#git log   查看更新日志