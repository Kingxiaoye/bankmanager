#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*gcc执行的时候需要加上 -lpthread 才能真正调用pthread库函数
 * 只有头文件不行
 * */
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function
#include "tcp_server.h"
//#include "user.h"
#include "message_log.h"
#include <time.h>


pthread_mutex_t mutex;

/*时间全局变量*/
char mytime[20] = "";
int main(int argc, char *argv[])
{
	/*初始化互斥锁*/
	pthread_mutex_init(&mutex,NULL);

    int server_sockfd;//服务器端套接字    
    int *client_sockfd;//客户端套接字    
    pthread_t thread = -1;
    struct sockaddr_in my_addr;//服务器网络地址结构体  
    struct sockaddr_in remote_addr; //客户端网络地址结构体    
    socklen_t sin_size;
    //char buf[BUFSIZ];//数据传送的缓冲区  
    memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零    
    my_addr.sin_family=AF_INET; //设置为IP通信    
    my_addr.sin_addr.s_addr=inet_addr("192.168.31.106");//服务器IP地址--允许连接到所有本地地址上  
    my_addr.sin_port=htons(8001); //服务器端口号        
    /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return 1;
    }
    /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
    {
        perror("bind");
        return 1;
    }
    /*监听连接请求--监听队列长度为5*/
    listen(server_sockfd,5);
    sin_size=sizeof(struct sockaddr_in);

    while(1)
    {
        client_sockfd = malloc(sizeof(int));
        /*等待客户端连接请求到达*/
        if((*client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
        {
            perror("accept");
            return 1;
        }
        printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
        //为连接创建线程
        if(0 != pthread_create(&thread, NULL, message_handle, (void *)client_sockfd))
        {
            perror("pthread_create error");
            continue;
        }
        printf("pthread_create success\n");
    }


    close(server_sockfd);
    return 0;
}

void client_userinfo_init(client_user* x)
{
    strncpy(x->password,"null", 4);
    strncpy(x->name,"null", 4);
    x->msg = 0;
    x->id = 0;
    x->money  = 0;
    x->state = 0;
    x->flag = 0;
    x->transfer_id = 0;
	strncpy(x->transfer_name,"null",4);
	x->tmp_money = 0;
	x->admin_operate = 0;
}
/*创建一个管理员，存在一个则返回*/
void create_admin(user_info* userinfo)
{
	FILE* fp = NULL;
	int count = 0;
	int last=1000;
	userinfo_init(userinfo);
	fp = fopen("bank_admin","ab+");
	fseek(fp,0,0);
	count=fread(userinfo,sizeof(user_info),1,fp);
	if(count>0)
	{
		return;
	}
	userinfo->id=last;
	//strcpy(userinfo->name,client_userinfo->name);
	//strcpy(userinfo->password,client_userinfo->password);
	snprintf(userinfo->name,sizeof(userinfo->name),"admin");
	snprintf(userinfo->password,sizeof(userinfo->password),"admin");
	userinfo->state=0;
	userinfo->money = 0;
	fseek(fp,0,2);
	fwrite(userinfo,sizeof(user_info),1,fp);
	fclose(fp);

	fp = fopen("bank_admin","ab+");
	while(!feof(fp))
	{
		count=fread(userinfo,sizeof(user_info),1,fp);
		if(count>0)
		{
			printf("%s-%s\n",userinfo->name,userinfo->password);
		}
	}
	fclose(fp);
	return ;

}

/*获取时间函数*/
char* get_time()
{
    struct timeval tv;
    char* p = mytime;
    gettimeofday(&tv,NULL);	strftime(mytime,sizeof(mytime),"%Y/%m/%d/%H/%M",localtime(&tv.tv_sec));
    //printf("Time:%s\n",mytime);
    return p;
}
void userinfo_init(user_info* x)
{
    strncpy(x->name,"null",4);
    strncpy(x->password,"null", 4);
    x->money = 0;
    x->id = 0;
    x->state = 0;
}
/*小结构转为大结构体*/
void s1_to_s2(user_info* x1,client_user* x2)
{
    client_userinfo_init(x2);
    x2->id = x1->id;
    x2->money = x1->money;
    strncpy(x2->password,x1->password, sizeof(x2->password));
    strncpy(x2->name,x1->name, sizeof(x2->name));
    x2->state = x1->state;
}

void s2_to_s1(client_user* x2,user_info* x1)
{
    x1->id = x2->id;

}

void* message_handle(void *arg)
{

	client_user* client_userinfo = NULL;
	client_user* server_userinfo = NULL;
	user_info* userinfo = NULL;

	/*结构体申请内存并初始化*/
	client_userinfo=(client_user*)malloc(sizeof(client_user));
	server_userinfo=(client_user*)malloc(sizeof(client_user));
	userinfo=(user_info*)malloc(sizeof(user_info));

	create_admin(userinfo);
    int* sockfd = arg;
    char command[COMMAND_LEN] = {0};
    char admin_name[9] = "admin";
    int len = 0;
    /*初始化结构体*/
    client_userinfo_init(client_userinfo);

    printf("IN message_handle\n");
/*
    if(0 >= send(*sockfd,"Welcome to my server\n",21,0))//发送欢迎信息
    {
        perror("Send welcome error");
    }
*/
    /*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/


    while((len=recv(*sockfd,client_userinfo, sizeof(client_user),0))>0)
    {
    	pthread_mutex_lock(&mutex);
    	int ret = 0;
    	/*每次收到新消息，要把服务端结构体和用户信息结构体初始化*/
        client_userinfo_init(server_userinfo);
        userinfo_init(userinfo);
         /*client_userinfo->msg为消息类型*/
    	/*消息类型1：登陆请求*/
        if(USER_LOGIN==client_userinfo->msg)
        {
        	printf("服务端收到USER_LOGIN消息！！！\n");
        	ret = login(userinfo,client_userinfo,server_userinfo);
            if(PASSWORD_ERROR == ret)
            {
            	printf("登录失败,密码错误！！！\n");
            	server_userinfo->flag = PASSWORD_ERROR;
            }
            else if(NO_USER_INFO == ret)
            {
            	printf("登录失败,用户不存在！！！\n");
            	server_userinfo->flag = NO_USER_INFO;
            }
            else if(ACCOUNT_STATE_INVALID == ret)
            {
            	printf("登录失败,用户已销户！！！\n");
            	server_userinfo->flag = ACCOUNT_STATE_INVALID;
            }
            else
            {
            	if(server_userinfo->flag == ADMIN_FLAG)
            	{
            		server_userinfo->flag = ADMIN_FLAG;
            		if(1 != client_userinfo->admin_operate)
            		{
						snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s登录系统",\
						get_time(),server_userinfo->name);
						write_running_log(command,server_userinfo->name);
            		}
            	}
            	else
            	{
            		snprintf(command,sizeof(command),"时间:%s 用户%s登录系统",\
					get_time(),server_userinfo->name);
					write_running_log(command,server_userinfo->name);
					server_userinfo->flag = 1;
            	}
            }
            send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_DESPOSIT==client_userinfo->msg)
        {
        	printf("服务端收到USER_DESPOSIT消息！！！\n");
        	ret = desposit(userinfo,client_userinfo,server_userinfo);
        	if(PASSWORD_ERROR == ret)
        	{
        		printf("存钱失败，密码错误！！！\n");
        		server_userinfo->flag = PASSWORD_ERROR;
        	}
        	else if(NO_USER_INFO == ret)
        	{
        		printf("存钱失败，用户不存在！！！\n");
        		server_userinfo->flag = NO_USER_INFO;
        	}
        	else
			{
        		if(1 == client_userinfo->admin_operate)
        		{
            		snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s存钱%f成功",\
    				get_time(),client_userinfo->name,client_userinfo->tmp_money);
    				write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s存钱%f成功",\
    				get_time(),admin_name,client_userinfo->name,client_userinfo->tmp_money);
    				write_running_log(command,admin_name);
        		}
        		else
        		{
					snprintf(command,sizeof(command),"时间:%s 用户%s存钱%f成功",\
					get_time(),client_userinfo->name,client_userinfo->tmp_money);
					write_running_log(command,client_userinfo->name);
        		}
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
			}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_REGESTER == client_userinfo->msg)
        {
        	printf("服务端收到USER_REGESTER消息！！！\n");
        	ret = regester(userinfo,client_userinfo,server_userinfo);
        	if(NAME_OCCUPY == ret)
        	{
        		printf("该名字已注册过！！！\n");
        		server_userinfo->flag = NAME_OCCUPY;
        	}
        	else
        	{
        		if(1 == client_userinfo->admin_operate)
				{
					snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s注册成功",\
					get_time(),client_userinfo->name);
					write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s注册成功",\
    				get_time(),admin_name,client_userinfo->name);
    				write_running_log(command,admin_name);
				}
				else
				{
					snprintf(command,sizeof(command),"时间:%s 用户%s注册成功",\
					get_time(),client_userinfo->name);
					write_running_log(command,client_userinfo->name);
				}
        		s1_to_s2(userinfo,server_userinfo);
        		//printf("用户%s注册成功！！！\n",server_userinfo->name);
        		server_userinfo->flag = 1;
        	}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_WITHDRAW == client_userinfo->msg)
        {
        	printf("服务端收到USER_WITHDRAW消息！！！\n");
        	ret = userwithdraw(userinfo,client_userinfo,server_userinfo);
        	if(SUCCESS == ret)
        	{
        		if(1 == client_userinfo->admin_operate)
        		{
            		snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s取钱%f成功",\
    				get_time(),client_userinfo->name,client_userinfo->tmp_money);
    				write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s取钱%f成功",\
    				get_time(),admin_name,client_userinfo->name,client_userinfo->tmp_money);
    				write_running_log(command,admin_name);
        		}
        		else
        		{
					snprintf(command,sizeof(command),"时间:%s 用户%s取钱%f成功",\
					get_time(),client_userinfo->name,client_userinfo->tmp_money);
					write_running_log(command,client_userinfo->name);
        		}
        		s1_to_s2(userinfo,server_userinfo);
        		server_userinfo->flag = 1;
        	}
        	else
        	{
        		server_userinfo = client_userinfo;
        		server_userinfo->flag = 0;
        	}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_QUIRY == client_userinfo->msg)
        {
        	printf("服务端收到USER_QUIRY消息！！！\n");
        	ret = user_quiry(userinfo,client_userinfo,server_userinfo);
        	if(SUCCESS == ret)
			{
        		/*
        		if(1 == client_userinfo->admin_operate)
        		{
            		snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s查询信息成功",\
    				get_time(),client_userinfo->name);
    				write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s查询信息成功",\
    				get_time(),admin_name,client_userinfo->name);
    				write_running_log(command,admin_name);
        		}
        		else
        		{
					snprintf(command,sizeof(command),"时间:%s 用户%s查询信息成功",\
					get_time(),client_userinfo->name);
					write_running_log(command,client_userinfo->name);
        		}
        		*/
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
				printf("查询成功，查询信息已发送！！！\n");
			}
        	else if(NO_USER_INFO == ret)
        	{
        		//s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = NO_USER_INFO;
        	}
        	else
        	{
        		s1_to_s2(userinfo,server_userinfo);
        		//strncpy(server_userinfo->password,client_userinfo->password,sizeof(client_userinfo->password));
        		server_userinfo->flag = 0;
        		printf("查询失败%d--  %s\n",server_userinfo->flag,server_userinfo->password);
        	}
        	/*管理员操作时，会先发查询消息判断用户本人，这时应保证管理员操作位为1*/
			if(1 == client_userinfo->admin_operate)
			{
				server_userinfo->admin_operate = 1;
			}
			send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_TRANSFER == client_userinfo->msg)
        {
        	printf("服务端收到USER_TRANSFER消息！！！\n");
        	ret = user_transfer(userinfo,client_userinfo,server_userinfo);
        	if(SUCCESS == ret)
			{

				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->tmp_money = client_userinfo->tmp_money;
				server_userinfo->transfer_id = client_userinfo->transfer_id;
				server_userinfo->flag = 1;

				snprintf(command,sizeof(command),"时间:%s 用户%s向账户:%d转账%f成功",\
				get_time(),client_userinfo->name,client_userinfo->transfer_id,client_userinfo->tmp_money);
				write_running_log(command,client_userinfo->name);
				printf("转账成功！！！\n");
			}
        	else if(UPDATE_USERINFO_ERROR == ret)
        	{
        		server_userinfo->flag = UPDATE_USERINFO_ERROR;
        	}
        	else if(NO_USER_INFO == ret)
        	{
        		server_userinfo->flag = NO_USER_INFO;
        	}
			send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_CHANGE_PASSWORD == client_userinfo->msg)
		{
        	printf("服务端收到USER_CHANGE_PASSWORD消息！！！\n");
        	ret = user_change_password(userinfo,client_userinfo,server_userinfo);
        	if(SUCCESS == ret)
			{
        		if(1 == client_userinfo->admin_operate)
        		{
            		snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s修改密码成功",\
    				get_time(),client_userinfo->name);
    				write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s修改密码成功",\
    				get_time(),admin_name,client_userinfo->name);
    				write_running_log(command,admin_name);
        		}
        		else
        		{
					snprintf(command,sizeof(command),"时间:%s 用户%s修改密码成功",\
					get_time(),client_userinfo->name);
					write_running_log(command,client_userinfo->name);
        		}
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
				printf("密码修改成功！！！\n");
			}
        	else
        	{
        		printf("密码修改失败！！！\n");
        	}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
		}
        else if(USER_CLOSE_ACCOUNT == client_userinfo->msg)
        {
        	printf("服务端收到USER_CLOSE_ACCOUNT消息！！！\n");
        	ret = user_close_account(userinfo,client_userinfo,server_userinfo);
			if(SUCCESS == ret)
			{
        		if(1 == client_userinfo->admin_operate)
        		{
            		snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s销户成功",\
    				get_time(),client_userinfo->name);
    				write_running_log(command,client_userinfo->name);

            		snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s帮用户%s销户成功",\
    				get_time(),admin_name,client_userinfo->name);
    				write_running_log(command,admin_name);
        		}
        		else
        		{
					snprintf(command,sizeof(command),"时间:%s 用户%s销户成功",\
					get_time(),client_userinfo->name);
					write_running_log(command,client_userinfo->name);
        		}
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
				printf("用户销户成功！！！\n");
			}
			else
			{
				printf("用户销户失败！！！\n");
			}
			send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USERINFO_BY_ID == client_userinfo->msg)
        {
        	printf("收到USERINFO_BY_ID信息！！！\n");
        	ret = get_userinfo_by_id(userinfo,client_userinfo,server_userinfo);
        	if(SUCCESS == ret)
			{
				s1_to_s2(userinfo,server_userinfo);

				snprintf(command,sizeof(command),"时间:%s [管理员操作]用户%s通过账户id:%d向管理员%s获取了信息",\
				get_time(),server_userinfo->name,client_userinfo->id,admin_name);
				write_running_log(command,server_userinfo->name);

				snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s通过账户id:%d获取用户%s信息成功",\
				get_time(),admin_name,client_userinfo->id,server_userinfo->name);
				write_running_log(command,admin_name);


				server_userinfo->flag = 1;
				//printf("通过id获取用户信息成功！！！\n");
			}
			else
			{
				printf("通过id获取用户信息失败！！！\n");
			}
			send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(FUZZY_QUIRY == client_userinfo->msg)
        {
        	printf("收到FUZZY_QUIRY信息！！！\n");
        	fuzzy_quiry(sockfd,userinfo,client_userinfo,server_userinfo);

			snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s对字符串%s使用了模糊查询功能",\
			get_time(),client_userinfo->name,client_userinfo->transfer_name);
			write_running_log(command,admin_name);
        }
        else if(USER_EXIT == client_userinfo->msg)
        {
        	printf("收到身份异常退出USER_EXIT信息！！！\n");
        	snprintf(command,sizeof(command),"时间:%s %s身份验证异常！退出系统！！！",\
    		get_time(),client_userinfo->name);
    		write_running_log(command,client_userinfo->name);
        }
        else if(USER_QUIRY_LOG == client_userinfo->msg)
        {
        	printf("收到USER_QUIRY_LOG信息！！！\n");
        	FILE* fp;
        	char file_name[512];
        	char buf[1024];
        	snprintf(file_name,sizeof(file_name),"%s/%s",LOG_PATH,client_userinfo->name);
        	fp = fopen(file_name,"rb+");
        	while(!feof(fp))
        	{
        		fgets(buf,sizeof(buf),fp);
        		//printf("%s\n",buf);
        		send(*sockfd,buf, sizeof(buf),0);

        	}
        	fclose(fp);
        	snprintf(buf,sizeof(buf),"%s","log end");
        	send(*sockfd,buf, sizeof(buf),0);



        }
        pthread_mutex_unlock(&mutex);
    }
    printf("%s\n",client_userinfo->name);
    if(0 == strncmp(client_userinfo->name,"admin",sizeof(client_userinfo->name)))
    {
    	snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s退出系统",\
		get_time(),client_userinfo->name);
		write_running_log(command,client_userinfo->name);
    }
    else
    {
    	if(1 == client_userinfo->admin_operate)
    	{
    		/*管理员操作时，会导致结构体变量为用户信息，但管理员标志位为1,
    		 * 这里写名字时，应写管理员的*/

        	snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员%s退出系统",\
    		get_time(),admin_name);
    		write_running_log(command,admin_name);
    	}
    	else
    	{
			snprintf(command,sizeof(command),"时间:%s 用户%s退出系统",\
			get_time(),client_userinfo->name);
			write_running_log(command,client_userinfo->name);
    	}
    }

    free_struct(client_userinfo);
    free_struct(server_userinfo);
    free(userinfo);
    userinfo = NULL;
    close(*sockfd);
    return NULL;
}


void free_struct(client_user *x)
{
    free(x);
    x = NULL;
}

int save(user_info* ap)
{
    user_info* nap=(user_info*)malloc(sizeof(user_info));
    int flag=-1,count=0;
    FILE *fp;
    //printf("save in\n");
    fp=fopen("bank","rb+");
    if(fp!=NULL)
    {
        while(!feof(fp))
        {
            count=fread(nap,sizeof(user_info),1,fp);
            if(count>0)
            {
                if(ap->id==nap->id)
                {
                    fseek(fp,-((long)sizeof(user_info)),1);
                    fwrite(ap,sizeof(user_info),1,fp);
                    //printf("save success\n");
                    flag=0;
                    break;
                }
            }
        }

    }
    free(nap);
    nap = NULL;
    fclose(fp);
    return flag;
}

int login(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
    int count = 0;
    int flag = 0;
	FILE* fp;
	if (0==strcmp(client_userinfo->name,"admin"))//管理员登录
	{
		fp=fopen("bank_admin","ab+");
		if(fp!=NULL)
		{
			while(!feof(fp))
			{
				count=fread(userinfo,sizeof(user_info),1,fp);
				if(count>0)
				{
					if (0==strcmp(userinfo->password,client_userinfo->password))
					{
						//printf("hello client you are admin \n");
						s1_to_s2(userinfo,server_userinfo);
						server_userinfo->flag = ADMIN_FLAG;
						fclose(fp);
						return SUCCESS;
					}
					else
					{
						printf("password error!!!\n");
						s1_to_s2(userinfo,server_userinfo);
						fclose(fp);
						return PASSWORD_ERROR;
					}
				}
			}

		}

	}
	//用户登录
	else
	{
		fp=fopen("bank","ab+");
		if(fp!=NULL)
		{
			while(!feof(fp))
			{
				count=fread(userinfo,sizeof(user_info),1,fp);
				if(count>0)
				{
					if(0 == strcmp(userinfo->name,client_userinfo->name))
					{
						/*先判断用户是否销户，在判断密码是否匹配*/
						if(0 == userinfo->state)
						{
							if(0 == strcmp(userinfo->password,client_userinfo->password))
							{
								flag = 1;
								break;
							}
							else
							{
								//printf("%s-%s\n",userinfo->mima,password);
								printf("password error!,please reput!\n");
								return PASSWORD_ERROR ;
							}
						}
						else
						{
							printf("user had closed account!!!\n");
							return ACCOUNT_STATE_INVALID;
						}

					}

				}
			}
			if(1 == flag)
			{
				//printf("hello client you are user \n");
				s1_to_s2(userinfo,server_userinfo);
				fclose(fp);
				return SUCCESS;
			}
			else
			{
				printf("用户名不存在，请尝试注册！！！\n");
				fclose(fp);
				return NO_USER_INFO;
			}

		}
	}
	return SUCCESS;
}

int desposit(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int flag = 0;
	float money = -1;
	int ret = 0;
	ret = user_info_match(userinfo,client_userinfo,server_userinfo);
	printf("desposit ret :%d\n",ret);
	if(ret == SUCCESS)
	{
		money = client_userinfo->tmp_money;
		userinfo->money += money;
		if(0 == save(userinfo))
		{
			flag = 1;
		}
	}

	if(1 == flag)
	{
		printf("用户%s存钱%f成功！！！\n",userinfo->name,money);
		return SUCCESS;
	}
	return ret;
}

int regester(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
    int count=0;
    int ret = 0;
    FILE *fp;
    int last=initnumber;
    ret = user_info_match(userinfo,client_userinfo,server_userinfo);
    if(SUCCESS == ret)
    {
        printf("用户名被占用！！！\n");
        return NAME_OCCUPY;
    }
    else
    {
    	fp = fopen("bank","ab+");
        fseek(fp,-((long)sizeof(user_info)),2);
        count=fread(userinfo,sizeof(user_info),1,fp);
        if(count>0)
        {
            last=userinfo->id+1;
        }
        userinfo->id=last;
        strcpy(userinfo->name,client_userinfo->name);
        strcpy(userinfo->password,client_userinfo->password);
        userinfo->state=0;
        userinfo->money = 0;
        fseek(fp,0,2);
        fwrite(userinfo,sizeof(user_info),1,fp);

        fclose(fp);
        return SUCCESS;
    }
    return 0;
}

int userwithdraw(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
    float get_money = -1;
    int ret = 0;
    ret = user_info_match(userinfo,client_userinfo,server_userinfo);
    if(SUCCESS == ret)
    {
    	get_money = client_userinfo->tmp_money;
    	userinfo->money -= get_money;
    	if(0 == save(userinfo))
    	{
    		printf("用户%s取钱%f成功\n",userinfo->name,get_money);
    		return SUCCESS;
    	}
    }
    else
    {
    	return ret;
    }


    return 0;

}
/*在本地数据库匹配客户端发送来的用户信息，匹配成功则返回SUCCESS，并将
 * 用户信息存到全局变量userinfo中
 * */
int user_info_match(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int count = 0;
	int flag = 0;
	FILE *fp;
	if(client_userinfo->id!=0)
	{
		if(client_userinfo->state!=1)
		{
			fp=fopen("bank","rb+");
			if(fp!=NULL)
			{
				while(!feof(fp))
				{
					count=fread(userinfo,sizeof(user_info),1,fp);
					if(count>0)
					{
						if(0 == strncmp(userinfo->name,client_userinfo->name,sizeof(userinfo->name)))
						{
							flag = 1;
							break;
						}
					}
				}
				if(1 == flag)
				{
					if(0 == strncmp(userinfo->password,client_userinfo->password,sizeof(userinfo->password)))
					{
						fclose(fp);
						return SUCCESS;
					}
					else
					{
						printf("password error!,please reput!\n");
						fclose(fp);
						return PASSWORD_ERROR ;
					}
				}
				else
				{
					//printf("用户名不存在，请尝试注册！！！\n");
					fclose(fp);
					return NO_USER_INFO ;
				}
			}
			else
			{
				return OPEN_FILE_ERROR;
			}
		}
		else
		{
			printf("您的账号已经销户!!!\n");
			return ACCOUNT_STATE_INVALID;
		}
	}
	else
	{
		if(USER_REGESTER != client_userinfo->msg)
		{
			printf("您的账号ID错误!!!\n");
			return ACCOUNT_ID_ERROR;
		}
		else
		{
			printf("新账号注册中!!!\n");
			return USER_REGESTER;
		}
	}

	return SUCCESS;
}

int user_info_match_by_id(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int count = 0;
	int flag = 0;
	FILE *fp;
	//printf("%d\n",client_userinfo->id);
	if(client_userinfo->id!=0)
	{
		if(client_userinfo->state!=1)
		{
			fp=fopen("bank","rb+");
			if(fp!=NULL)
			{
				while(!feof(fp))
				{
					count=fread(userinfo,sizeof(user_info),1,fp);
					if(count>0)
					{
						if(userinfo->id == client_userinfo->id)
						{
							flag = 1;
							break;
						}
					}
				}
				//printf("%s-%s\n",userinfo->name,userinfo->password);
				if(1 == flag)
				{
					if(0 == strncmp(userinfo->password,client_userinfo->password,sizeof(userinfo->password)))
					{
						fclose(fp);
						return SUCCESS;
					}
					else
					{
						printf("password error!,please reput!\n");
						fclose(fp);
						return PASSWORD_ERROR ;
					}
				}
				else
				{
					printf("账户不存在，请尝试注册！！！\n");
					fclose(fp);
					return NO_USER_INFO ;
				}
			}
			else
			{
				return OPEN_FILE_ERROR;
			}
		}
		else
		{
			printf("您的账号已经销户!!!\n");
			return ACCOUNT_STATE_INVALID;
		}
	}
	else
	{
		if(USER_REGESTER != client_userinfo->msg)
		{
			printf("您的账号ID错误!!!\n");
			return ACCOUNT_ID_ERROR;
		}
		else
		{
			printf("新账号注册中!!!\n");
			return USER_REGESTER;
		}
	}

	return SUCCESS;
}
int user_quiry(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int ret = 0;
	ret = user_info_match(userinfo,client_userinfo,server_userinfo);
	if(ret == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		return ret;
	}
	//return 0;
}

int user_transfer(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	user_info* transfer_userinfo = NULL;
	FILE* fp;
	int count = -1;
	int ret = 0;
	char command[COMMAND_LEN] = {0};
	transfer_userinfo=(user_info*)malloc(sizeof(user_info));
	userinfo_init(transfer_userinfo);
	//strncpy(transfer_userinfo->name,client_userinfo->transfer_name,sizeof(transfer_userinfo->name));
	fp=fopen("bank","ab+");
	if(fp!=NULL)
	{
		while(!feof(fp))
		{
			count=fread(transfer_userinfo,sizeof(user_info),1,fp);
			if(count>0)
			{
				if(transfer_userinfo->id == client_userinfo->transfer_id)
				{
					printf("===\n");
					break;
				}
			}
		}
	}
	fclose(fp);
	ret = user_info_match(userinfo,client_userinfo,server_userinfo);
	if(ret == SUCCESS)
	{
		//printf("%s--%f\n",transfer_userinfo->name,transfer_userinfo->money);
		transfer_userinfo->money += client_userinfo->tmp_money;
		//printf("%f\n",transfer_userinfo->money);
		if(0 == save(transfer_userinfo))
		{
			userinfo->money -= client_userinfo->tmp_money;
			if(0 == save(userinfo))
			{
				snprintf(command,sizeof(command),"时间:%s 用户%s收到账户:%d转账%f成功",\
				get_time(),transfer_userinfo->name,client_userinfo->id,client_userinfo->tmp_money);
				write_running_log(command,transfer_userinfo->name);

				free(transfer_userinfo);
				transfer_userinfo = NULL;
				return SUCCESS;
			}
		}
		else
		{
			free(transfer_userinfo);
			transfer_userinfo = NULL;
			printf("更新转账数据失败！！！\n");
			return UPDATE_USERINFO_ERROR;
		}
	}
	else
	{
		free(transfer_userinfo);
		transfer_userinfo = NULL;
		printf("转账账户不存在！！！\n");
		return NO_USER_INFO;
	}
	free(transfer_userinfo);
	transfer_userinfo = NULL;
	return 0;
}

int user_change_password(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int ret = 0;
	ret = user_info_match(userinfo,client_userinfo,server_userinfo);
	/*新密码也可以和原密码相同*/
	if(ret == PASSWORD_ERROR||ret == SUCCESS)
	{
		snprintf(userinfo->password,sizeof(userinfo->password),"%s",client_userinfo->password);
		if(0 == save(userinfo))
		{
			return SUCCESS;
		}
		else
		{

			return UPDATE_USERINFO_ERROR;
		}

	}
	else
	{
		return FALSE;
	}
	return 0;
}
int user_close_account(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int ret = 0;
	ret = user_info_match(userinfo,client_userinfo,server_userinfo);
	/*新密码也可以和原密码相同*/
	if(ret == SUCCESS)
	{
		userinfo->state = 1;
		if(0 == save(userinfo))
		{
			return SUCCESS;
		}
		else
		{

			return UPDATE_USERINFO_ERROR;
		}
	}
	else
	{
		return FALSE;
	}
	return 0;
}

int get_userinfo_by_id(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int ret = 0;
	ret = user_info_match_by_id(userinfo,client_userinfo,server_userinfo);
	if(ret == SUCCESS)
	{
		return SUCCESS;
	}
	return ret;
}
int fuzzy_quiry(void *arg,user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo)
{
	int* sockfd = arg;
	char str[9];
	int count = -1;
	FILE* fp = NULL;
	snprintf(str,sizeof(str),"%s",client_userinfo->transfer_name);
	fp = fopen("bank","rb+");
	if(fp!=NULL)
	{
		while(!feof(fp))
		{
			count=fread(userinfo,sizeof(user_info),1,fp);
			if(count>0)
			{
				if(strstr(userinfo->name,str))
				{
					s1_to_s2(userinfo,server_userinfo);
					server_userinfo->flag = 1;
					send(*sockfd,server_userinfo, sizeof(client_user),0);
				}

			}
		}
		/*结束设置标志位发送结构体，告诉客户端结束搜索*/
		s1_to_s2(userinfo,server_userinfo);
		server_userinfo->flag = FUZZY_QUIRY_END;
		send(*sockfd,server_userinfo, sizeof(client_user),0);
	}
	return 0;
}
