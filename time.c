
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char mytime[20] = "";

char* get_time()
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}
char* get_time2()
{
    struct timeval tv;
    char* p = mytime;
    gettimeofday(&tv,NULL);	strftime(mytime,sizeof(mytime),"%Y/%m/%d/%H/%M",localtime(&tv.tv_sec));
    printf("Time:%s\n",mytime);
    return p;
}
int main()
{

    printf("\007 the current data/time is %s\n",get_time2());
    //get_time2();
    return 0;
}