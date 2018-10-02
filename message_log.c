#include "message_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>



void write_running_log(const char *cmd)
{
	char command[COMMAND_LEN] = {0};
	int check_line = 0;
	FILE *pp = NULL;
	char buf[BUF_LEN] = {0};
	snprintf(command,sizeof(command),
		"/usr/bin/echo  \"%s\"  >>%s",
		cmd,LOG_PATH);		
	if(system(command)>>8)
	{
		printf("write log error(cmd:%s)\n",command);
		return ;
	}	
	printf("%s\n",cmd);
	snprintf(command, sizeof(command), "/usr/bin/cat %s |wc -l",LOG_PATH);
	//获取行数
	pp = popen(command, "r");
	if(NULL == pp)
	{
		printf("get log line error(cmd:%s)\n",command);
		return ;
	}
	fgets(buf, sizeof(buf), pp);
	if(strlen(buf) > 0 && '\n' == buf[strlen(buf) - 1])
	{
		buf[strlen(buf) - 1] = '\0';
	}
	check_line = atoi(buf);
	pclose(pp);
	pp = NULL;
	//判断行数是否小于10000
	if(check_line <= SAVE_LINE)
	{			
		return ;
	}
	snprintf(command,sizeof(command),
		"/usr/bin/tail -n %d %s > %s ",SAVE_LINE,LOG_PATH,LOG_PATH_BACK);
	if(system(command)>>8)
	{
		printf("set log line error(cmd:%s)\n",command);
		return ;
	}
	//保存最新的日志
	snprintf(command,sizeof(command),
		"/usr/bin/mv %s  %s",LOG_PATH_BACK,LOG_PATH);
	
	if(system(command)>>8)
	{
		printf("change log error(cmd:%s)",command);
		return ;
	}
}

