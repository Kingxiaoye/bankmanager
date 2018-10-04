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
//#include "message_log.h"


int main(int argc, char *argv[])
{
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

    /*结构体申请内存并初始化*/
    client_userinfo=(client_user*)malloc(sizeof(client_user));
    server_userinfo=(client_user*)malloc(sizeof(client_user));
    userinfo=(user_info*)malloc(sizeof(user_info));


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
    free_struct(client_userinfo);
    free_struct(server_userinfo);
    free(userinfo);
    userinfo = NULL;

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
    int* sockfd = arg;

    int len = 0;
    int count = 0;
    int flag = 0;

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
    	int ret = 0;
    	/*每次收到新消息，要把服务端结构体和用户信息结构体初始化*/
        client_userinfo_init(server_userinfo);
        userinfo_init(userinfo);
         /*client_userinfo->msg为消息类型*/
    	/*消息类型1：登陆请求*/
        if(USER_LOGIN==client_userinfo->msg)
        {
        	ret = login();
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
            else
            {
            	printf("用户%s登录成功！！！\n",server_userinfo->name);
            	server_userinfo->flag = 1;
            }
            send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_DESPOSIT==client_userinfo->msg)
        {
        	ret = desposit();
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
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
			}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_REGESTER == client_userinfo->msg)
        {
        	ret = regester();
        	if(NAME_OCCUPY == ret)
        	{
        		printf("该名字已注册过！！！\n");
        		server_userinfo->flag = NAME_OCCUPY;
        	}
        	else
        	{
        		s1_to_s2(userinfo,server_userinfo);
        		printf("用户%s注册成功！！！\n",server_userinfo->name);
        		server_userinfo->flag = 1;
        	}
        	send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_WITHDRAW == client_userinfo->msg)
        {
        	ret = userwithdraw();
        	if(SUCCESS == ret)
        	{
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
        	ret = user_quiry();
        	if(SUCCESS == ret)
			{
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
				printf("查询成功，查询信息已发送！！！\n");
			}
        	else
        	{
        		s1_to_s2(userinfo,server_userinfo);
        		//strncpy(server_userinfo->password,client_userinfo->password,sizeof(client_userinfo->password));
        		server_userinfo->flag = 0;
        		printf("查询失败%d--  %s\n",server_userinfo->flag,server_userinfo->password);
        	}
			send(*sockfd,server_userinfo, sizeof(client_user),0);
        }
        else if(USER_TRANSFER == client_userinfo->msg)
        {
        	ret = user_transfer();
        	if(SUCCESS == ret)
			{
				s1_to_s2(userinfo,server_userinfo);
				server_userinfo->flag = 1;
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
        	ret = user_change_password();
        	if(SUCCESS == ret)
			{
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
    }
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
    printf("save in\n");
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
                    printf("save success\n");
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

int login()
{
    int count = 0;
    int flag = 0;
	FILE* fp;
	if (0==strcmp(client_userinfo->name,"admin"))//管理员登录
	{
		if (0==strcmp(client_userinfo->password,"admin"))
		{
			printf("hello client you are admin \n");
			server_userinfo = client_userinfo;
			server_userinfo->flag = 1;
			return SUCCESS;
		}
		else
		{
			printf("password error!!!\n");
			server_userinfo = client_userinfo;
			return PASSWORD_ERROR;
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

				}
			}
			if(1 == flag)
			{
				printf("hello client you are user \n");
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

int desposit()
{
	int flag = 0;
	float money = -1;
	int ret = 0;
	ret = user_info_match();
	printf("desposit ret :%d\n",ret);
	if(ret == SUCCESS)
	{
		money = client_userinfo->money - userinfo->money;
		userinfo->money = client_userinfo->money;
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

int regester()
{
    int count=0;
    int ret = 0;
    FILE *fp;
    int last=initnumber;
    ret = user_info_match();
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

int userwithdraw()
{
    float get_money = -1;
    int ret = 0;
    ret = user_info_match();
    if(SUCCESS == ret)
    {
    	get_money = userinfo->money - client_userinfo->money;
    	userinfo->money = client_userinfo->money;
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
int user_info_match()
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
				printf("%s-%s\n",userinfo->name,userinfo->password);
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
					printf("用户名不存在，请尝试注册！！！\n");
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
int user_quiry()
{
	int ret = 0;
	ret = user_info_match();
	if(ret == SUCCESS)
	{
		return SUCCESS;
	}
	else
	{
		return FALSE;
	}
	//return 0;
}

int user_transfer()
{
	user_info* transfer_userinfo = NULL;
	FILE* fp;
	int count = -1;
	int ret = 0;
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
	ret = user_info_match();
	if(ret == SUCCESS)
	{
		printf("%s--%f\n",transfer_userinfo->name,transfer_userinfo->money);
		transfer_userinfo->money += client_userinfo->tmp_money;
		printf("%f\n",transfer_userinfo->money);
		if(0 == save(transfer_userinfo))
		{
			userinfo->money -= client_userinfo->tmp_money;
			if(0 == save(userinfo))
			{
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

int user_change_password()
{
	int ret = 0;
	ret = user_info_match();
	/*新密码也可以和原密码相同*/
	if(ret == PASSWORD_ERROR||ret == SUCCESS)
	{
		snprintf(userinfo->password,sizeof(userinfo->password),client_userinfo->password);
		if(0 == save(userinfo))
		{
			//printf("%s\n",userinfo->password);
			//printf("密码修改成功！！！\n");
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
