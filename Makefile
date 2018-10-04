all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c -lpthread
	chmod +x client server
	git add .
	git commit -m "取钱功能三次密码错误重新登录实现基于客户端"
	git push -u origin master
	#git log   查看更新日志