#pragma once


#ifdef __cplusplus
extern "C" {
#endif


#define SAVE_LINE 1000
#define COMMAND_LEN 512
#define BUF_LEN 1024
    /*用户名字长度和密码长度*/
#define USER_LEN 9

/*开户账号*/
#define initnumber 100000001




/*结构体*/
typedef struct xingxi
{
    int zhanghao;
    char huming[9];
    char mima[9];
    float jine;
    int zhuangtai;
}xianshi;

char* get_time();
void free_struct(xianshi *x);

/*普通用户操作函数*/
void usermenu(xianshi *x);/*用户操作界面*/
void userdeposit(xianshi *x);/*用户存钱*/
void userwithdraw(xianshi *x);/*用户取钱*/
void userlogout(xianshi* x);/*用户选择销户*/
void change_password(xianshi* x);/*用户修改密码*/

/*管理员操作函数*/
void display(xianshi *x);/*显示*/
int save(xianshi* ap);/*保存*/
float getmoney1();/*金额限制1*/
float getmoney2();/*金额限制2*/
float getmoney3(float i);/*金额限制3*/
//xianshi* docreatexianshi();/*执行开户*/
void adduser();/*执行开户*/
xianshi* getnumber(int number);/*获取账号*/
void tuichu();/*退出*/
void deposit();/*存款*/
void withdraw();/*取款*/
void openAnxianshi();/*开户*/
void closeAnAcount();/*销户*/
void query();/*查询*/
void queryall();/*查询全部*/
void mainmenu();/*系统菜单，管理员操作界面*/

#ifdef __cplusplus
}
#endif

