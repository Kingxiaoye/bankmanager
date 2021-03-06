/*

 */

#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define initnumber 100000001 //初始账户号，后续账户加1操作
#define SUCCESS 0
#define FALSE 1
#define PASSWORD_ERROR 2
#define NO_USER_INFO 3
#define ACCOUNT_STATE_INVALID 5
/*消息id定义*/
#define USER_LOGIN 1000   //登录
#define USER_REGESTER 1002 //注册
#define USER_EXIT 1003  //退出
#define USER_DESPOSIT 1004  //存钱
#define USER_WITHDRAW 1005  //取钱
#define USER_QUIRY 1006  //查询
#define USER_CLOSE_ACCOUNT 1007   //销户
#define USER_CHANGE_PASSWORD 1008 //修改密码
#define USER_TRANSFER 1009 //用户转账
#define USERINFO_BY_ID 1010//获取用户信息通过账户id
#define FUZZY_QUIRY 1011 //模糊查询
#define USER_QUIRY_LOG 1012 //日志查询

/*管理员登录标志位*/
#define ADMIN_FLAG 110
/*模糊查询标志位*/
#define FUZZY_QUIRY_END 120



typedef struct client_user
{
    int msg;//消息类型
    int id;//账户
    char name[9];//账户名称
    char password[9];//账户密码
    float money;//账户金额
    int state;//账户状态 0正常 1销户
    int flag;//判断服务端操作是否有效，0有效，1无效
    int transfer_id;//转账账户id
    char transfer_name[9];//转账账户名字
    float tmp_money;//存取钱金额，转账金额
    int admin_operate;//管理员操作标志位 0用户操作，1管理员操作
}client_user;

/*全局变量-用户信息*/
client_user* client_userinfo = NULL;
client_user* server_userinfo = NULL;

/*自定义函数*/
void free_struct(client_user *x);
int login(void* arg);/*登录操作*/
//int regester();/*注册操作*/
void welcome(void* arg);/*欢迎界面*/
void client_userinfo_init(client_user* x);
int client_usermenu(void* arg,client_user *x);
void client_display(client_user* x);
void userdeposit(void* arg,client_user *x);
void userwithdraw(void* arg,client_user *x);
float getmoney();
void user_quiry(void* arg,client_user *x);
void usertransfer(void* arg,client_user *x);
void change_password(void* arg,client_user *x);
void close_acount(void* arg,client_user *x);
void mainmenu(void* arg);
void admin_deposit(void* arg,client_user *x);
void admin_withdraw(void* arg,client_user *x);
void admin_query(void* arg,client_user *x);
void admin_adduser(void* arg,client_user *x);
void admin_close_account(void* arg,client_user *x);
void admin_change_password(void* arg,client_user *x);
void admin_fuzzy_query(void* arg,client_user *x);
void user_quiry_log(void* arg,client_user *x);
void admin_transfer(void* arg,client_user *x);

#endif /* TCP_CLIENT_H_ */
