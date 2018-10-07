/*
 * tcp_server.h
 *
 *  Created on: 2018年10月1日
 *      Author: wangye11
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SUCCESS 0
/*定义错误类型*/
#define FALSE 1
#define PASSWORD_ERROR 2
#define NO_USER_INFO 3
#define NAME_OCCUPY 4
#define ACCOUNT_STATE_INVALID 5
#define ACCOUNT_ID_ERROR 6
#define OPEN_FILE_ERROR 7
#define UPDATE_USERINFO_ERROR 8
/*管理员登录标志位*/
#define ADMIN_FLAG 110
/*模糊查询标志位*/
#define FUZZY_QUIRY_END 120

#define initnumber 100000001 //初始账户号，后续账户加1操作
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
#define USERINFO_BY_ID 1010  //获取用户信息通过账户id
#define FUZZY_QUIRY 1011 //模糊查询
#define USER_QUIRY_LOG 1012 //日志查询



#define LINK(PATH,FILENAME) PATH/##FILENAME
/*结构体*/
typedef struct user_info
{
    int id;
    char name[9];
    char password[9];
    float money;
    int state;
}user_info;

typedef struct client_user
{
    int msg;
    int id;
    char name[9];
    char password[9];
    float money;
    int state;
    int flag;
    int transfer_id;//转账账户id
    char transfer_name[9];//转账账户名字
    float tmp_money;//存取钱金额，转账金额
    int admin_operate;//管理员操作标志位 0用户操作，1管理员操作
}client_user,server_user;

/*全局变量-用户信息*/



/*自定义函数*/
int login(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);/*登录操作*/
int regester(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);/*注册操作*/
int desposit(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);/*存钱操作*/
int userwithdraw(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);/*取钱操作*/
void welcome(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);/*欢迎界面*/
void* message_handle(void *arg);
void free_struct(client_user *x);
char* get_time();
void client_userinfo_init(client_user* x);
void userinfo_init(user_info* x);
void s1_to_s2(user_info* x1,client_user* x2);
void s2_to_s1(client_user* x2,user_info* x1);
int user_info_match(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int user_info_match_by_id(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int save(user_info* ap);
int user_quiry(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int user_transfer(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int user_change_password(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int user_close_account(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
int get_userinfo_by_id(user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);
void create_admin(user_info* userinfo);
int fuzzy_quiry(void *arg,user_info* userinfo,client_user* client_userinfo,client_user* server_userinfo);




#endif /* TCP_SERVER_H_ */
