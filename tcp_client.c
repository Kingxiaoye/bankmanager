#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function
#include "tcp_client.h"


int main(int argc, char *argv[])
{

    /*客户端*/
    int* client_sockfd;
    struct sockaddr_in remote_addr; //服务器端网络地址结构体    

    client_sockfd = malloc(sizeof(int));
    memset(&remote_addr,0,sizeof(remote_addr)); //数据初始化--清零    
    remote_addr.sin_family=AF_INET; //设置为IP通信    
    remote_addr.sin_addr.s_addr=inet_addr("192.168.31.106");//服务器IP地址    
    remote_addr.sin_port=htons(8001); //服务器端口号        
    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((*client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
    {
        perror("socket");
        return 1;
    }
    /*将套接字绑定到服务器的网络地址上*/
    if(connect(*client_sockfd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)
    {
        perror("connect");
        return 1;
    }
    printf("connected to server\n");

    client_userinfo=(client_user*)malloc(sizeof(client_user));
    server_userinfo=(client_user*)malloc(sizeof(client_user));
    client_userinfo_init(client_userinfo);
    client_userinfo_init(server_userinfo);

    welcome(client_sockfd);


    //if(!strcmp(buf,"quit"))
      //  break;
    //len=send(client_sockfd,client_userinfo,sizeof(client_user),0);
    //len=recv(client_sockfd,client_userinfo,sizeof(client_user),0);




    close(*client_sockfd);//关闭套接字 
    return 0;
}
void free_struct(client_user *x)
{
    free(x);
    x = NULL;
}

void client_userinfo_init(client_user* x)
{
    strncpy(x->password,"null",4);
    strncpy(x->name,"null",4);
    x->msg = 0;
    x->id = 0;
    x->money  = 0;
    x->state = 0;
    x->flag = 0;
    x->transfer_id = 0;
    strncpy(x->transfer_name,"null",sizeof(x->transfer_name)-1);
    x->tmp_money = 0;
}

/*管理员系统菜单*/
void mainmenu(void* arg)
{
	int* client_sockfd = arg;
    int xuanze;
    do
    {
        xuanze=-1;
        printf("+-------------------------------+\n");
        printf("| 存款  请按 1     开户  请按 4 |\n");
        printf("| 取款  请按 2     销户  请按 5 |\n");
        printf("| 查询  请按 3     退出  请按 0 |\n");
        printf("| 修改密码         请按 6      |\n");
        printf("| 模糊查询         请按 7      |\n");
        printf("+-------------------------------+\n");
        printf("请选择：\n");
        scanf("%d",&xuanze);
        fflush(stdin);
        switch(xuanze)
        {
            case 1: system("clear");
            	admin_deposit(client_sockfd,server_userinfo);
				break;
            case 2: system("clear");
				admin_withdraw(client_sockfd,server_userinfo);
				break;
            case 3: system("clear");
				admin_query(client_sockfd,server_userinfo);
				break;
            case 4: system("clear");
				admin_adduser(client_sockfd,server_userinfo);
				break;
            case 5: system("clear");
            	admin_close_account(client_sockfd,server_userinfo);
				break;
            case 6: system("clear");
				admin_change_password(client_sockfd,server_userinfo);
				break;
            case 7: system("clear");
				admin_fuzzy_query(client_sockfd,server_userinfo);
				break;
            case 0: system("clear");
            	free_struct(client_userinfo);
				free_struct(server_userinfo);
				close(*client_sockfd);//关闭套接字 
				exit(0);
				break;
            default:system("clear");
                    printf("您输入的有误！\n");
                    getchar();
                    break;
        }
    }while(1);
}

void admin_deposit(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int id = 0;
	char password[9];
	printf("请输入存款账户id:\n");
	scanf("%d",&id);
	printf("请输入存款账户密码:\n");
	scanf("%8s",password);
	x->id = id;
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*通过账户id向服务器发送消息，获取用户信息*/
	x->msg = USERINFO_BY_ID;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		userdeposit(client_sockfd,server_userinfo);
	}
	else
	{
		printf("账户id有误！！！\n");
	}
	return;
}

void admin_withdraw(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int id = 0;
	char password[9];
	printf("请输入取款账户id:\n");
	scanf("%d",&id);
	printf("请输入取款账户密码:\n");
	scanf("%8s",password);
	x->id = id;
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*通过账户id向服务器发送消息，获取用户信息*/
	x->msg = USERINFO_BY_ID;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		userwithdraw(client_sockfd,server_userinfo);
	}
	else
	{
		printf("账户id有误！！！\n");
	}
	return;
}
void admin_query(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int id = 0;
	char password[9];
	printf("请输入查询账户id:\n");
	scanf("%d",&id);
	printf("输入查询账户密码:\n");
	scanf("%8s",password);
	x->id = id;
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*通过账户id向服务器发送消息，获取用户信息*/
	x->msg = USERINFO_BY_ID;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		user_quiry(client_sockfd,server_userinfo);
	}
	else
	{
		printf("账户id有误！！！\n");
	}
	return;
}
void admin_adduser(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	char name[9];
	char password[9];
	//float money = -1;
	printf("请输入账户名称:\n");
	scanf("%8s",name);
	printf("请输入账户密码:\n");
	scanf("%8s",password);
	//printf("请输入金额:\n");
	//money = getmoney();
	snprintf(x->name,sizeof(x->name),"%s",name);
	snprintf(x->password,sizeof(x->password),"%s",password);
	//x->money = money;
	x->msg = USER_REGESTER;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		printf("恭喜用户%s注册成功！！！\n",server_userinfo->name);
	}
	else
	{
		printf("注册失败，用户名已存在！！！\n");
	}

	return;
}
void admin_close_account(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int id = 0;
	char password[9];
	printf("请输入销户id:\n");
	scanf("%d",&id);
	printf("请输入密码:\n");
	scanf("%8s",password);
	x->id = id;
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*通过账户id向服务器发送消息，获取用户信息*/
	x->msg = USERINFO_BY_ID;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		close_acount(client_sockfd,server_userinfo);
	}
	else
	{
		printf("账户id有误！！！\n");
	}
	return;
}
void admin_change_password(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int id = 0;
	char password[9];
	printf("请输入账户id:\n");
	scanf("%d",&id);
	printf("请输入密码:\n");
	scanf("%8s",password);
	x->id = id;
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*通过账户id向服务器发送消息，获取用户信息*/
	x->msg = USERINFO_BY_ID;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		change_password(client_sockfd,server_userinfo);
	}
	else
	{
		printf("账户id有误！！！\n");
	}
	return;
}
void admin_fuzzy_query(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int i = 0;
	char name[9];
	char password[9];
	char str[9];
	//float money = -1;
	printf("请输入账户名称:\n");
	scanf("%8s",name);
	printf("请输入账户密码:\n");
	scanf("%8s",password);
	snprintf(x->name,sizeof(x->name),"%s",name);
	snprintf(x->password,sizeof(x->password),"%s",password);
	/*再次验证管理员身份*/
	x->msg = USER_LOGIN;
	if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(ADMIN_FLAG == server_userinfo->flag)
	{
		printf("请输入你要模糊查询的内容:\n");
		scanf("%8s",str);
		/*用结构体的transfer_name保存用来模糊查询的字符串*/
		snprintf(x->transfer_name,sizeof(x->transfer_name),"%s",str);
		x->msg = FUZZY_QUIRY;
		if((len=send(*client_sockfd,x,sizeof(client_user),0)<0))
		{
			printf("send error\n");
		}
		while((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)>0))
		{
			if(1 == server_userinfo->flag)
			{
				i++;
				client_display(server_userinfo);
			}
			else if(FUZZY_QUIRY_END == server_userinfo->flag)
			{
				if(i == 0)
				{
					printf("未找到相关内容！！！\n");
				}
				break;
			}
			else
			{
				printf("未找到相关内容！！！\n");
			}
		}

	}
	else
	{
		printf("非管理员操作！退出登录！！！\n");
		welcome(client_sockfd);
	}

	return;
}

/*登录函数*/
int login(void* arg)
{
    char name[9];
    char password[9];
    int len = 0;
    int ret = 0;
    int* client_sockfd = arg;
    printf("请输入您的用户名：");
    scanf("%8s",name);
    printf("请输入您的密码：");
    scanf("%8s",password);
    strcpy(client_userinfo->name,name);
    strcpy(client_userinfo->password,password);
    client_userinfo->msg = USER_LOGIN;
    if((len=send(*client_sockfd,client_userinfo,sizeof(client_user),0)<0))
    {
        printf("send error\n");
        printf("name:%s",client_userinfo->name);
    }
    if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
    {
        printf("recv error");
    }
    if(ADMIN_FLAG == server_userinfo->flag)
    {
    	//printf("%d\n",server_userinfo->flag);
    	if (0==strcmp(server_userinfo->name,"admin"))
		{
			if (0==strcmp(server_userinfo->password,"admin"))
			{
				//超级管理员界面
				printf("欢迎进入超级管理员界面！！！\n");
				mainmenu(client_sockfd);
				return SUCCESS;
			}
			else
			{
				printf("password error!,please reput! \n");
				return FALSE;
			}
		}
    }
    else if(1 == server_userinfo->flag)
    {
    	while(1)
		{
			printf("按回车键进入主菜单！！！\n");
			getchar();
			system("clear");
			ret = client_usermenu(client_sockfd,server_userinfo);
			if(1 == ret)
			{
				break;
			}
		}
		printf("%d\n",server_userinfo->msg);
		printf("123\n");
		printf("%s-%s\n",server_userinfo->name,server_userinfo->password);
		return 0;
    }
    else if(NO_USER_INFO == server_userinfo->flag)
    {
    	system("clear");
    	printf("用户不存在，请尝试注册！！！\n");
    	return FALSE;
    }
    else if(ACCOUNT_STATE_INVALID == server_userinfo->flag)
    {
    	system("clear");
    	printf("用户已销户，请尝试重新注册！！！\n");
    	return FALSE;
    }
    else
    {
    	system("clear");
		printf("密码错误！！！\n");
		return FALSE;
    }
    return 0;

}

/*注册函数*/
int regester(void* arg)
{
    int len;
    char name[9];
    int ret = 0;
    char password[6];
    int* client_sockfd = arg;
    printf("请输入您的用户名：");
    scanf("%8s",name);
    printf("请输入您的密码：");
    scanf("%8s",password);
    strcpy(client_userinfo->name,name);
    strcpy(client_userinfo->password,password);
    client_userinfo->msg = USER_REGESTER;
    if((len=send(*client_sockfd,client_userinfo,sizeof(client_user),0)<0))
	{
		printf("send error\n");
	}
	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv error");
	}
	if(1 == server_userinfo->flag)
	{
		printf("恭喜用户%s注册成功！！！\n",server_userinfo->name);
		while(1)
		{
			system("clear");
			ret = client_usermenu(client_sockfd,server_userinfo);
			if(1 == ret)
			{
				break;
			}
		}
		return SUCCESS;
	}
	else
	{
		printf("注册失败，用户名已存在！！！\n");
		return FALSE;
	}



}

/*欢迎界面*/
void welcome(void* arg)
{
    int msg = -1;
    int* client_sockfd = arg;
    do
    {
        int a = -1;
        int b = -1;
        //int chiose = -1;
        printf("+-------------------------------+\n");
        printf("|                               |\n");
        printf("|   欢迎使用XX银行业务管理平台     |\n");
        printf("|      1:登录                    |\n");
        printf("|      2:注册                    |\n");
        printf("|      0:退出                    |\n");
        printf("|                               |\n");
        printf("+-------------------------------+\n");
        printf("请选择：");
        scanf("%d",&msg);
        fflush(stdin);
        //client_userinfo->msg = msg;
        switch(msg)
        {
            case 1: system("clear");
                a = login(client_sockfd);
                break;
            case 2: system("clear");
            	b = regester(client_sockfd);
            	break;
            case 0: system("clear");
            	free_struct(client_userinfo);
            	free_struct(server_userinfo);
            	close(*client_sockfd);//关闭套接字 
				exit(0);
            	break;
            default: system("clear");
                printf("您输入的有误！\n");
                getchar();
                break;
        }
        if(0 == a||0 == b)
        {

            break;
        }
        else
        {
            //printf("%d %d\n",a,b);
            printf("操作错误，请重新登录/注册！！！\n");
        }

    }while(1);

}

void user_quiry(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	x->msg = USER_QUIRY;
	if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
	{
		printf("send quiry msg error\n");
	}

	if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
	{
		printf("recv quiry msg error");
	}
	if(1 == server_userinfo->flag)
	{
		printf("你的信息如下：\n");
		client_display(server_userinfo);
	}
}

void client_display(client_user* x)
{
    char tem[9]="******";
    char *zt;
    if(x->state==0)
    {
        zt="正常";
    }
    else
    {
        zt="销户";
        return;//不打印
    }
    printf("\n");
    printf("+----------------------------------------------+\n");
    printf("|    帐号|    户名|    密码|      余额|    状态|\n");
    printf("|%8d|%8s|%8s|%10.1f|%8s|\n",x->id,x->name,tem,x->money,zt);
    printf("+----------------------------------------------+\n");
}

int client_usermenu(void* arg,client_user *x)
{
    int* client_sockfd = arg;
    printf("个人信息如下:\n");
    client_display(x);
    int xuanze;
    xuanze=-1;
    printf("+-------------------------------+\n");
    printf("| 存款      请按 1  |\n");
    printf("| 取款      请按 2  |\n");
    printf("| 查询      请按 3  |\n");
    printf("| 销户      请按 4  |\n");
    printf("| 转账      请按 5  |\n");
    printf("| 修改密码   请按 6  |\n");
    printf("| 退出  请按 0  |\n");
    printf("+-------------------------------+\n");
    printf("请选择：\n");
    scanf("%d",&xuanze);
    fflush(stdin);
    switch(xuanze)
    {
        case 1: system("clear");
            userdeposit(client_sockfd,x);
            break;
        case 2: system("clear");
        	userwithdraw(client_sockfd,x);
			break;
        case 3: system("clear");
        	user_quiry(client_sockfd,x);
        	break;
        case 4: system("clear");
        	close_acount(client_sockfd,x);
        	break;
        case 5: system("clear");
        	usertransfer(client_sockfd,x);
        	break;
        case 6: system("clear");
        	change_password(client_sockfd,x);
        	break;
        case 0: system("clear");
        	close(*client_sockfd);//关闭套接字 
        	exit(0);
        	break;
        default: system("clear");
            printf("您输入的有误！请重新输入：\n");
            break;
    }
    return 0;
}

void userdeposit(void* arg,client_user *x)
{
    int* client_sockfd = arg;
    float money=-1;
    int len = 0;
    if(x->id!=0)
    {
        if(x->state==1)
        {
            printf("您输入的账号已经销户！任意键回菜单.....\n");
            return;
        }
        else
        {
        	/*存钱前，先去服务端判断用户信息是否正确*/
        	x->msg = USER_QUIRY;
			if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
			{
				printf("send USER_QUIRY msg error\n");
			}

			if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
			{
				printf("recv USER_QUIRY msg error");
			}
			if(1 == server_userinfo->flag)
			{
				printf("请输入您的存款金额：");
				money=getmoney();
				x->tmp_money = money;
				x->msg = USER_DESPOSIT;//存钱
				printf("%f--%d\n",x->money,x->msg);
				if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
				{
					printf("send deposit msg error\n");
				}

				if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
				{
					printf("recv deposit msg error");
				}
				if(1 == server_userinfo->flag)
				{
					system("clear");
					printf("存款成功!\n");
					getchar();
					return;
				}
				else
				{
					printf("存款失败！\n");
				}

			}
            //client_display(x);

        }
    }
    else
    {
    	printf("账户id异常！\n");
    }
    return;
}

float getmoney()
{
    float money=-1;
    while(1)
    {
        scanf("%f",&money);
        fflush(stdin);
        if(money<0)
        {
            printf("您输入的是无效金额！请重新输入！！！\n");
            continue;
        }
        else
        {
            break;
        }
    }
    return money;
}

void userwithdraw(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int i = 0;
    char password[6];
    float money=-1;
    int len = 0;
    if(x->id != 0)
    {
        if(x->state == 1)
        {
            printf("您输入的账号已经销户！任意键回菜单.....\n");
            getchar();
            return;
        }
        else
        {
        	client_display(x);
        	for(i = 0;i<3;i++)
        	{
            	printf("请输入您的取款密码：");
            	scanf("%s",password);
            	snprintf(x->password,sizeof(x->password),"%s",password);
            	x->msg = USER_QUIRY;
            	if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
				{
					printf("send USER_QUIRY msg error\n");
				}

				if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
				{
					printf("recv USER_QUIRY msg error");
				}
				if(1 == server_userinfo->flag)
				{
					printf("请输入您的取款金额：");
					money=getmoney();
					x->tmp_money = money;
					x->msg = USER_WITHDRAW;//取钱
					if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
					{
						printf("send userwithdraw msg error\n");
					}
					if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
					{
						printf("recv userwithdraw msg error");
					}
					if(1 == server_userinfo->flag)
					{
						system("clear");
						printf("%s\n",server_userinfo->name);
						printf("取款成功!\n");
						//getchar();
						return;
					}
				}
                else
            	{
            		system("clear");
            		printf("密码错误！！！\n");
            		//welcome(client_sockfd);
            		continue;
            	}
        	}
        	if(3 == i)
        	{
        		printf("你输入错误已经超过3次！请重新登录！！！\n");
        		welcome(client_sockfd);
        	}
        }
    }
    else
    {
        printf("您输入的账号不存在！\n");
        return;
    }
}

void usertransfer(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	char password[9] = {0};
	int transfer_id = -1;
	float tmp_money = 0;
	int len = 0;
	printf("转账前请输入密码：\n");
	scanf("%8s",password);
	if(0 == strncmp(password,x->password,sizeof(password)))
	{
		printf("请输入转账账户：\n");
		scanf("%d",&transfer_id);
		x->transfer_id = transfer_id;
		x->msg = USER_TRANSFER;
		printf("请输入转账金额：\n");
		scanf("%f",&tmp_money);
		x->tmp_money = tmp_money;
		if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
		{
			printf("send transfer msg error\n");
		}

		if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
		{
			printf("recv transfer msg error");
		}
		if(1 == server_userinfo->flag)
		{
			system("clear");
			printf("用户%s向账户%d转账%f成功\n",server_userinfo->name,server_userinfo->transfer_id,server_userinfo->tmp_money);
			return;
		}
		else
		{
			printf("转账失败！！！");
			return;
		}

	}
	else
	{
		printf("密码错误！！！\n");
	}
	return;
}

void change_password(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int i = 0;
	char tmp[9];
	char password[9];
	/*第一次发消息确认是本人操作*/
	for(i = 0;i<3;i++)
	{
		printf("请输入原密码：\n");
		scanf("%8s",password);
		fflush(stdin);
		snprintf(x->password, sizeof(password),"%s",password);
		x->msg = USER_QUIRY;
		if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
		{
			printf("send check userinfo msg error\n");
		}

		if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
		{
			printf("recv check userinfo msg error");
		}
		printf("123\n");
		printf("%d---%s\n",server_userinfo->flag,server_userinfo->name);
		if(1 == server_userinfo->flag)
		{
			while(1)
			{
				/*第二次发送消息，修改密码*/
				printf("请输入新密码：\n");
				scanf("%8s",password);
				fflush(stdin);
				snprintf(tmp, sizeof(tmp),"%s",password);
				/*本地验证二次新密码*/
				printf("请再次输入新密码：\n");
				scanf("%s",password);
				fflush(stdin);
				if(0 == strncmp(tmp,password,sizeof(tmp)))
				{
					snprintf(x->password, sizeof(x->password),"%s",tmp);
					printf("%s\n",x->password);
					x->msg = USER_CHANGE_PASSWORD;
					break;
				}
				else
				{
					system("clear");
					printf("两次密码不一致，请重新输入：\n");
				}
			}
			/*第二次发送消息，修改密码*/
			if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
			{
				printf("send change password msg error\n");
			}

			if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
			{
				printf("recv change password  msg error");
			}
			if(1 == server_userinfo->flag)
			{
				system("clear");
				printf("用户%s密码修改成功 ！！！\n",server_userinfo->name);
				return;
			}
			else
			{
				system("clear");
				printf("用户%s密码修改失败 ！！！\n",server_userinfo->name);
				return;
			}
		}
		else if(0 == server_userinfo->flag)
		{
			system("clear");
			printf("原密码输入错误！！！\n");
			//welcome(client_sockfd);
			continue;
		}
	}
	if(3 == i)
	{
		printf("你输入错误已经超过3次！请重新登录！！！\n");
		welcome(client_sockfd);
	}
	return;
}

void close_acount(void* arg,client_user *x)
{
	int* client_sockfd = arg;
	int len = 0;
	int i = 0;
	char tmp[9];
	char password[9];
	/*第一次发消息确认是本人操作*/
	for(i = 0;i<3;i++)
	{
		printf("销户前，请输入原密码：\n");
		scanf("%8s",password);
		fflush(stdin);
		snprintf(x->password, sizeof(password),"%s",password);
		x->msg = USER_QUIRY;
		if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
		{
			printf("send check userinfo msg error\n");
		}

		if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
		{
			printf("recv check userinfo msg error");
		}
		if(1 == server_userinfo->flag)
		{
			if(server_userinfo->money>0)
			{
				printf("账户还有余额！是否继续销户！yes/no！");
				scanf("%8s",tmp);
				fflush(stdin);
				if(0 == strncmp(tmp,"yes",sizeof(tmp)))
				{
					//x->state = 1;
					x->msg = USER_CLOSE_ACCOUNT;
					/*第二次发送消息，修改密码*/
					if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
					{
						printf("send USER_CLOSE_ACCOUNT  msg error\n");
					}

					if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
					{
						printf("recv USER_CLOSE_ACCOUNT  msg error");
					}
					if(1 == server_userinfo->flag)
					{
						system("clear");
						printf("用户%s销户成功 ！！！\n",server_userinfo->name);
						return;
					}
					else
					{
						system("clear");
						printf("用户%s销户失败 ！！！\n",server_userinfo->name);
						return;
					}
				}
				else
				{
					printf("账户还有余额！请先取钱！");
					return ;
				}
			}
			else
			{
				//x->state = 1;
				x->msg = USER_CLOSE_ACCOUNT;
				/*第二次发送消息，修改密码*/
				if((len=send(*client_sockfd,x,sizeof(client_user),0))<0)
				{
					printf("send USER_CLOSE_ACCOUNT msg error\n");
				}

				if((len=recv(*client_sockfd,server_userinfo,sizeof(client_user),0)<0))
				{
					printf("recv USER_CLOSE_ACCOUNT  msg error");
				}
				if(1 == server_userinfo->flag)
				{
					system("clear");
					printf("用户%s销户成功 ！系统转入登录界面！！！\n",server_userinfo->name);
					welcome(client_sockfd);
					return;
				}
				else
				{
					system("clear");
					printf("用户%s销户失败 ！！！\n",server_userinfo->name);
					return;
				}
			}

		}
		else if(0 == server_userinfo->flag)
		{
			system("clear");
			printf("原密码输入错误！！！\n");
			//welcome(client_sockfd);
			continue;
		}
	}
	if(3 == i)
	{
		printf("你输入错误已经超过3次！请重新登录！！！\n");
		welcome(client_sockfd);
	}
	return;

}
