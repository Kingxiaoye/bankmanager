#include "user.h"
#include <stdio.h> 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*时间全局变量*/
char mytime[20] = "";

/*获取时间函数*/
char* get_time()
{
    struct timeval tv;
    char* p = mytime;
    gettimeofday(&tv,NULL);	strftime(mytime,sizeof(mytime),"%Y/%m/%d/%H/%M",localtime(&tv.tv_sec));
    //printf("Time:%s\n",mytime);
    return p;
}


void free_struct(xianshi *x)
{
    free(x);
    x = NULL;
}
void usermenu(xianshi *x)
{
    char command[COMMAND_LEN] = {0};
    printf("个人信息如下:\n");
    display(x);
    int xuanze;
    do
    {
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
        printf("请选择：");
        scanf("%d",&xuanze);
        fflush(stdin);
        switch(xuanze)
        {
            case 1: system("clear");
                userdeposit(x);
                break;
            case 2: system("clear");
                userwithdraw(x);
                break;
            case 3: system("clear");
                display(x);
                break;
            case 4: system("clear");
                userlogout(x);
                break;
            case 6: system("clear");
                change_password(x);
                break;
            case 0: system("clear");
                snprintf(command,sizeof(command),"时间:%s 用户:%s 退出登录！",get_time(),x->huming);
                write_running_log(command);
            /*不用exit(0),释放申请的内存*/
                return;
                break;
            default: system("clear");
                printf("您输入的有误！\n");
                getchar();
                break;
        }
    }while(1);
}
void userdeposit(xianshi *x)
{
    char command[COMMAND_LEN] = {0};
    float money=-1;
    int result=0;
    if(x->zhanghao!=0)
    {
        if(x->zhuangtai==1)
        {
            printf("您输入的账号已经销户！任意键回菜单.....\n");
            getchar();
            return;
        }
        else
        {
            display(x);
            printf("请输入您的存款金额：");
            money=getmoney2();
            x->jine+=money;
            result=save(x);
            if(result!=0)
            {
                //display(x);
                system("clear");
                printf("存款成功!任意键回菜单.....\n");
                snprintf(command,sizeof(command),"时间:%s 用户:%s 存款%f元成功！",get_time(),x->huming,money);
                write_running_log(command);
                getchar();
                return;
            }
        }
    }
    return;
}
void userwithdraw(xianshi *x)
{
    char password[6];
    char command[COMMAND_LEN] = {0};
    float money=-1;
    int result=0;
    int i = 0;
    if(x->zhanghao!=0)
    {
        if(x->zhuangtai==1)
        {
            printf("您输入的账号已经销户！任意键回菜单.....\n");
            getchar();
            return;
        }
        else
        {
            for(i=0;i<3;i++)
            {
                printf("请输入您要取款的密码：");
                scanf("%s",&password);
                fflush(stdin);
                if(0 == strcmp(x->mima,password))
                {
                    display(x);
                    printf("请输入您的取款金额：");
                    money=getmoney3(x->jine);
                    x->jine-=money;
                    result=save(x);
                    if(result!=0)
                    {
                        //display(x);
                        system("clear");
                        printf("取款成功！任意键回菜单.....\n");
                        snprintf(command,sizeof(command),"时间:%s 用户:%s 取款%f元成功！",get_time(),x->huming,money);
                        write_running_log(command);
                        getchar();
                        return;
                    }
                }
                else
                {
                    printf("您输入的密码不正确！\n");
                    continue;
                }
            }
            if (i==3)
            {
                printf("您输入错误已经超过3次。\n");
                getchar();
                return;
            }
        }
    }
    else
    {
        printf("您输入的账号不存在！\n");
        getchar();
        return;
    }
}

void userlogout(xianshi* x)
{
    int flag=0;
    int result=0;
    char command[COMMAND_LEN] = {0};
    if(x->jine > 0)
    {
        printf("您的帐户里还有余额请记得取出！\n");
    }
    printf("确定要将帐户%d销户吗？[1：销户|其它：不销户]",x->zhanghao);
    scanf("%d",&flag);
    fflush(stdin);
    if(flag==1)
    {
        x->zhuangtai=1;
        x->jine=0;
        result=save(x);
        if(result!=0)
        {
            //display(x);
            system("clear");
            printf("销户成功！任意键回菜单.....\n");
            snprintf(command,sizeof(command),"时间:%s 用户:%s 销户成功！",get_time(),x->huming);
            write_running_log(command);
            exit(0);
            getchar();
            return ;
        }
    }

}
void change_password(xianshi* x)
{
    int result = 0;
    char tmp[9];
    char password[9];
    char command[COMMAND_LEN] = {0};
    printf("请输入原密码：\n");
    scanf("%s",&password);
    fflush(stdin);
    if(0 == strcmp(x->mima,password))
    {
        printf("请输入新密码：\n");
        scanf("%s",&password);
        fflush(stdin);
        snprintf(tmp, sizeof(tmp),password);
        printf("请再次输入新密码：\n");
        scanf("%s",&password);
        fflush(stdin);
        if(0 == strcmp(tmp,password))
        {
            snprintf(x->mima, sizeof(x->mima),tmp);
            result=save(x);
            if(result!=0)
            {
                //display(x);
                system("clear");
                printf("修改密码成功！任意键回菜单.....\n");
                snprintf(command,sizeof(command),"时间:%s 用户:%s 修改密码成功！",get_time(),x->huming);
                write_running_log(command);
                getchar();
                return;
            }
            else
            {
                printf("修改密码失败！\n");
            }

        }
        else
        {
            printf("两次密码不一致！！！ \n");
        }
    }
    else
    {
        printf("密码错误！！！ \n");
    }

}
/*用户x1向x2转账
void user_transfer(xianshi* x)
{
    int id;
    printf("请输入要转账的账号：\n");
    scanf("%d",&id);
    fflush(stdin);

}
*/





/*显示*/
void display(xianshi *x) 
{
    char tem[9]="******";
    char *zt;
    if(x->zhuangtai==0)
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
    printf("|%8d|%8s|%8s|%10.1f|%8s|\n",x->zhanghao,x->huming,tem,x->jine,zt);
    printf("+----------------------------------------------+\n");
}
/*修改保存用户信息*/
int save(xianshi* ap)
{
    xianshi* nap=(xianshi*)malloc(sizeof(xianshi));
    int flag=0,count=0;
    FILE *fp;
    fp=fopen("bank","rb+");
    if(fp!=NULL)
    {
        while(!feof(fp))
        {
            count=fread(nap,sizeof(xianshi),1,fp);
            if(count>0)
            {
                if(ap->zhanghao==nap->zhanghao)
                {
                    fseek(fp,-((long)sizeof(xianshi)),1);
                    fwrite(ap,sizeof(xianshi),1,fp);
                    flag=1;
                    break;
                }
            }
        }
        fclose(fp);
    }
    free(nap);
    nap = NULL;
    return flag;
}
/*金额限制1*/
float getmoney1()
{
    float money=-1;
    while(1)
    {
        scanf("%f",&money);
        fflush(stdin);
        if(money<0)
        {
            printf("您输入的是无效金额！");
            continue;
        }
        else
        {
            break;
        }
    }
    return money;
}
/*金额限制2*/
float getmoney2()
{
    float money=-1;
    while(1)
    {
        scanf("%f",&money);
        fflush(stdin);
        if(money<=0)
        {
            printf("您输入的金额小于等于0！请从新输入......\n");
            continue;
        }
        else
        {
            break;
        }
    }
    return money;
}
/*金额限制3*/
float getmoney3(float i)
{
    float money=-1;
    while(1)
    {
        scanf("%f",&money);
        fflush(stdin);
        if(money<=0)
        {
            printf("您输入的金额小于等于0！请从新输入......\n");
            continue;
        }
        else
        {
            if(money<=i)
            {
                break;
            }
            else
            {
                printf("您的帐号余额已经不足您本次的取款！请从新输入......\n");
                continue;
            }
        }
    }
    return money;
}
/*执行开户*/
void adduser()
{
    char command[COMMAND_LEN] = {0};
    xianshi* xs=(xianshi*)malloc(sizeof(xianshi));
    int count=0;
    char name[9];
    char password[6];
    int   con;
    FILE *fp;
    int last=initnumber;
    /*ab+可追加读写操作*/
    fp=fopen("bank","ab+");
    if(fp!=NULL)
    {
        fseek(fp,-((long)sizeof(xianshi)),2);
        count=fread(xs,sizeof(xianshi),1,fp);
        if(count>0)
        {
            last=xs->zhanghao+1;
        }
        printf("请输入您的户名：");
        scanf("%s",name);
        fflush(stdin);
        printf("请输入您的密码：");
        scanf("%s",password);
        fflush(stdin);
        printf("请输入您要存入的金额:");
        xs->jine=getmoney1();
        con=0;
        xs->zhanghao=last;
        strcpy(xs->huming,name);
        strcpy(xs->mima,password);
        xs->zhuangtai=con;
        fseek(fp,0,2);
        fwrite(xs,sizeof(xianshi),1,fp);
        fclose(fp);
    }
    if(xs->zhanghao!=0)
    {
        display(xs);
        printf("开户成功！任意键回菜单.....\n");
        snprintf(command,sizeof(command),"时间:%s [管理员操作]用户:%s开户成功！账号:%d 存款金额:%f",\
        get_time(),xs->huming,xs->zhanghao,xs->jine);
        write_running_log(command);
        getchar();
        free_struct(xs);
    }

    return ;
}
/*通过账号获取用户信息*/
xianshi* getnumber(int number)
{
    xianshi *ap=(xianshi*)malloc(sizeof(xianshi));
    xianshi *tmp=(xianshi*)malloc(sizeof(xianshi));
    int flag=0,count=0;
    FILE *fp;
    fp=fopen("bank","rb");
    if(fp!=NULL)
    {
        while(!feof(fp))
        {
            count=fread(tmp,sizeof(xianshi),1,fp);
            if(count>0)
            {
                if(tmp->zhanghao==number)
                {
                    flag=1;
                    break;
                }
            }
        }
        fclose(fp);
        if(flag)
        {
            ap->zhanghao=tmp->zhanghao;
            strcpy(ap->huming,tmp->huming);
            strcpy(ap->mima,tmp->mima);
            ap->jine=tmp->jine;
            ap->zhuangtai=tmp->zhuangtai;
        }
        else
        {
            ap->zhanghao=0;
            strcpy(ap->huming,"");
            strcpy(ap->mima,"");
            ap->jine=0 ;
            ap->zhuangtai=0;
        }
    }
    free_struct(tmp);
    return ap;
}

/*退出*/
void tuichu()
{
    exit(0);
}
/*存款*/
void deposit()
{
    char command[COMMAND_LEN] = {0};
    int number=-1;
    xianshi *ap;
    float money=-1;
    int result=0;
    while(1)
    {
        printf("请输入您要存款的账号：");
        scanf("%d",&number);
        fflush(stdin);
        ap=getnumber(number);
        if(ap->zhanghao!=0)
        {
            if(ap->zhuangtai==1)
            {
                printf("您输入的账号已经销户！任意键回菜单.....\n");
                getchar();
                free_struct(ap);
                return;
            }
            else
            {
                display(ap);
                printf("请输入您的存款金额：");
                money=getmoney2();
                ap->jine+=money;
                result=save(ap);
                if(result!=0)
                {
                    display(ap);
                    printf("存款成功!任意键回菜单.....\n");
                    snprintf(command,sizeof(command),"时间:%s [管理员操作]用户:%s存款成功！账号:%d 存款金额:%f",\
        get_time(),ap->huming,ap->zhanghao,money);
                    write_running_log(command);
                    getchar();
                    break;
                }
            }

        }
        else
        {
            printf("您输入的账号不存在！\n");
            getchar();
            free_struct(ap);
            return;
        }
    }
    free_struct(ap);
}
/*取款*/
void withdraw()
{
    char command[COMMAND_LEN] = {0};
    int number=-1,i;
    char password[6];
    xianshi *ap;
    float money=-1;
    int result=0;
    while(1)
    {
        printf("请输入您要取款的账号：");
        scanf("%d",&number);
        fflush(stdin);
        ap=getnumber (number);
        if(ap->zhanghao!=0)
        {
            if(ap->zhuangtai==1)
            {
                printf("您输入的账号已经销户！任意键回菜单.....\n");
                getchar();
                break;
            }
            else
            {
                for(i=0;i<3;i++)
                {
                    printf("请输入您要取款的密码：");
                    scanf("%s",&password);
                    fflush(stdin);
                    if(strcmp(ap->mima,password)==0)
                    {
                        display(ap);
                        printf("请输入您的取款金额：");
                        money=getmoney3(ap->jine);
                        ap->jine-=money;
                        result=save(ap);
                        if(result!=0)
                        {
                            display(ap);
                            printf("取款成功！任意键回菜单.....\n");
                            snprintf(command,sizeof(command),"时间:%s [管理员操作]用户:%s取款成功！账号:%d 取款金额:%f",\
        get_time(),ap->huming,ap->zhanghao,money);
                            write_running_log(command);
                            getchar();
                            break;
                        }
                    }
                    else
                    {
                        printf("您输入的密码不正确！\n");
                        continue;
                    }
                }
                if (i==3)
                {
                    printf("您输入错误已经超过3次。\n");
                    getchar();
                    break;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            printf("您输入的账号不存在！\n");
            getchar();
            free_struct(ap);
            return;
        }
    }
    free_struct(ap);
}


/*销户*/
void closeAnAcount()
{
    int number=-1,flag=0,i;
    char password[6];
    char command[COMMAND_LEN] = {0};
    xianshi *ap;
    int result=0;
    while(1)
    {
        printf("请输入您要销户的账号：");
        scanf("%d",&number);
        fflush(stdin);
        ap=getnumber (number);
        if(ap->zhanghao!=0)
        {
            for(i=0;i<3;i++)
            {
                printf("请输入您要销户的密码：");
                scanf("%s",&password);
                fflush(stdin);
                if(strcmp(ap->mima,password)==0)
                {
                    display(ap);
                    if(ap->jine>0)
                    {
                        printf("您的帐户里还有余额请记得取出！\n");
                    }
                    printf("确定要将帐户%d销户吗？[1：销户|其它：不销户]",ap->zhanghao);
                    scanf("%d",&flag);
                    fflush(stdin);
                    if(flag==1)
                    {
                        ap->zhuangtai=1;
                        ap->jine=0;
                        result=save(ap);
                        if(result!=0)
                        {
                            display(ap);
                            printf("销户成功！任意键回菜单.....\n");
                            snprintf(command,sizeof(command),"时间:%s [管理员操作]用户:%s销户成功！销户账号:%d ",\
        get_time(),ap->huming,ap->zhanghao);
                            write_running_log(command);
                            getchar();
                            break;
                        }
                    }
                    else
                    {
                        printf("您已经成功取消销户！任意键回菜单.....\n");
                        getchar();
                        break;
                    }
                }
                else
                {
                    printf("您输入的密码不正确！\n");
                    continue;
                }
            }
            if (i==3)
            {
                printf("您输入错误已经超过3次。\n");
                getchar();
                break;
            }
            else
            {
                break;
            }
            free (ap);
        }
        else
        {
            printf("您输入的账号不存在！\n");
            getchar();
            free_struct(ap);
            return;
        }
    }
    free_struct(ap);
}
/*查询*/
void query()
{
    int number=-1,i;
    char password[6];
    xianshi *ap;
    while(1)
    {
        printf("请输入您要查询的账号：");
        scanf("%d",&number);
        fflush(stdin);
        ap=getnumber (number);
        if(ap->zhanghao!=0)
        {
            for(i=0;i<3;i++)
            {
                printf("请输入您要查询的密码：");
                scanf("%s",&password);
                fflush(stdin);
                if(strcmp(ap->mima,password)==0)
                {
                    display(ap);
                    printf("查询成功！任意键回菜单.....\n");
                    getchar();
                    break;
                }
                else
                {
                    printf("您输入的密码不正确！\n");
                    continue;
                }
            }
            if (i==3)
            {
                printf("您输入错误已经超过3次。\n");
                getchar();
                break;
            }
            else
            {
                break;
            }
        }
        else
        {
            printf("您输入的账号不存在！\n");
            getchar();
            free_struct(ap);
            return;
        }
    }
    free_struct(ap);
}
/*查询全部*/
void queryall()
{
    char name[9];
    char password[9];
    int flag=0,i,count=0;
    xianshi *tmp=(xianshi*)malloc(sizeof(xianshi));
    for(i=0;i<3;i++)
    {
        if (flag==0)
        {
            printf("请输入您的用户名：");
            scanf("%8s",&name);
            if (strcmp(name,"admin")!=0)
            {
                continue;
            }
            else
            {
                flag=1;
            }
        }
        printf("请输入您的密码：");
        scanf("%8s",&password);
        if (strcmp(password,"admin")!=0)
        {
            continue;
        }
        else
        {
            FILE *fp;
            fp=fopen("bank","rb");
            if(fp!=NULL)
            {
                while(!feof(fp))
                {
                    count=fread(tmp,sizeof(xianshi),1,fp);
                    if(count>0)
                    {
                        display(tmp);
                    }
                }
                getchar();
                free_struct(tmp);
                return;
            }
            fclose(fp);
        }
    }
    if (i==3)
    {
        printf("您输入错误已经超过3次。\n");
        tuichu();
    }
    free_struct(tmp);
}
/*系统菜单*/
void mainmenu()
{
    char command[COMMAND_LEN] = {0};
    int xuanze;
    do
    {
        xuanze=-1;
        printf("+-------------------------------+\n");
        printf("| 存款  请按 1     开户  请按 4 |\n");
        printf("| 取款  请按 2     销户  请按 5 |\n");
        printf("| 查询  请按 3     退出  请按 0 |\n");
        printf("| 查询全部         请按 6       |\n");
        printf("+-------------------------------+\n");
        printf("请选择：");
        scanf("%d",&xuanze);
        fflush(stdin);
        switch(xuanze)
        {
            case 1: system("clear");
                    deposit();
                    break;
            case 2: system("clear");
                    withdraw();
                    break;
            case 3: system("clear");
                    query();
                    break;
            case 4: system("clear");
                    adduser();
                    break;
            case 5: system("clear");
                    closeAnAcount();
                    break;
            case 6: system("clear");
                    queryall();
                    break;
            case 0: system("clear");
                    snprintf(command,sizeof(command),"时间:%s [管理员操作]管理员:admin 退出登录！",get_time());
                    write_running_log(command);
                    tuichu();
                    break;
            default:system("clear");
                    printf("您输入的有误！\n");
                    getchar();
                    break;
        }
    }while(1);
}
