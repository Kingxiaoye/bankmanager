/*文件头*/
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "user.h"
#include "message_log.h"
//#include "super_manager.h"




/*全局变量-用户信息*/
xianshi* userinfo = NULL;

/*自定义函数*/
int login();/*登录操作*/
int regester();/*注册操作*/
void welcome();/*欢迎界面*/



/*主函数*/
void main()
{
    userinfo=(xianshi*)malloc(sizeof(xianshi));
    welcome();
    //login();
    usermenu(userinfo);
    free(userinfo);
    userinfo = NULL;
}

/*登录函数，返回值为错误次数*/
int login()
{
    char name[9];
    char password[9];
    char chiose[9];
    char command[COMMAND_LEN] = {0};
    //xianshi* userinfo=(xianshi*)malloc(sizeof(xianshi));
    FILE *fp;
    int count , flag=0;

    printf("请输入您的用户名：");
    scanf("%8s",&name);
    printf("请输入您的密码：");
    scanf("%8s",&password);
    if (0==strcmp(name,"admin"))
    {
        if (0==strcmp(password,"admin"))
        {
            //超级管理员界面
            printf("欢迎进入超级管理员界面\n");
            snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员:%s 登录系统",\
        get_time(),name);
            write_running_log(command);
            mainmenu();
        }
        else
        {
            printf("password error!,please reput! \n");
            return 1;
        }
    }
    else
    {
        //判断用户是否注册过
        fp=fopen("bank","ab+");
        if(fp!=NULL)
        {
            while(!feof(fp))
            {
                count=fread(userinfo,sizeof(xianshi),1,fp);
                if(count>0)
                {
                    //printf("%s-%s\n",userinfo->huming,name);
                    if(0 == strcmp(userinfo->huming,name))
                    {
                        if(0 == strcmp(userinfo->mima,password))
                        {
                            //printf("%s-%s\n",userinfo->huming,name);
                            flag = 1;
                            break;
                        }
                        else
                        {
                            //printf("%s-%s\n",userinfo->mima,password);
                            printf("password error!,please reput!\n");
                            return 1;
                        }
                    }

                }
            }
            if(1 == flag)
            {
                snprintf(command,sizeof(command),"时间:%s 用户:%s 登录系统",\
        get_time(),name);
                write_running_log(command);
                //usermenu(userinfo);
            }
            else
            {
                printf("用户名不存在，请尝试注册！！！\n");
                fclose(fp);
                return 1;
            }
            fclose(fp);
            return 0;
        }
    }
}

/*注册函数*/
int regester()
{
    //xianshi* xs=(xianshi*)malloc(sizeof(xianshi));
    int count=0;
    int flag = 0;
    char name[9];
    char password[6];
    FILE *fp;
    int last=initnumber;

    printf("请输入您的用户名：");
    scanf("%8s",&name);
    printf("请输入您的密码：");
    scanf("%8s",&password);
    fp=fopen("bank","ab+");
    if(fp!=NULL)
    {
        while(!feof(fp))
        {
            count=fread(userinfo,sizeof(xianshi),1,fp);
            if(count>0)
            {
                //printf("%s-%s\n",userinfo->huming,name);
                if(0 == strcmp(userinfo->huming,name))
                {
                    flag = 1;
                    break;
                }
            }
        }
    }
    if(1 == flag)
    {
        printf("用户名被占用！！！\n");
        return 1;
    }
    else
    {
        fseek(fp,-((long)sizeof(xianshi)),2);
        count=fread(userinfo,sizeof(xianshi),1,fp);
        if(count>0)
        {
            last=userinfo->zhanghao+1;
        }
        userinfo->zhanghao=last;
        strcpy(userinfo->huming,name);
        strcpy(userinfo->mima,password);
        userinfo->zhuangtai=0;
        userinfo->jine = 0;
        fseek(fp,0,2);
        fwrite(userinfo,sizeof(xianshi),1,fp);
        fclose(fp);
    }
    return 0;
}

/*欢迎界面*/
void welcome()
{
    int chiose = -1;

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
        printf("|      3:退出                    |\n");
        printf("|                               |\n");
        printf("+-------------------------------+\n");
        printf("请选择：");
        scanf("%d",&chiose);
        fflush(stdin);
        switch(chiose)
        {
            case 1: system("clear");
                a = login();
                break;
            case 2: system("clear");
                b = regester();
                break;
            case 3: tuichu();
                break;
            default: system("clear");
                printf("您输入的有误！\n");
                getchar();
                break;
        }
        if(0 == a||0==b)
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





