all:
	gcc -Wall -o client  tcp_client.c
	gcc -Wall -o server tcp_server.c message_log.c -lpthread
	chmod +x client server
	git add .
	git commit -m "基本功能实现，待修改地方：1、管理员转账2、密码输错退出3、异常处理"
	git push -u origin master
	#git log   查看更新日志