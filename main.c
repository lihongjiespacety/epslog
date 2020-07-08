#include "log.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int8_t printfbuff(unsigned char * buff, unsigned int length)
{  
    unsigned int i;
    unsigned int j;
    unsigned char* p;
    int dat;
    unsigned int lines = length / 16;          /* 打印行数          */
    unsigned int lastline = length % 16;       /* 最后一行数据个数  */


    p = buff;
    for(j=0;j<lines;j++)
    {
        printf("%#10lx:",(unsigned long)(intptr_t)p);
        for (i = 0; i< 16; i++)
        {
            dat = p[i]; 
            printf("%02x ", dat);
        }
        printf("| ");
        for (i = 0; i< 16; i++)
        {
            dat = p[i]; 
            if(isprint((int)dat))
            {
                printf("%c ", dat);
            }
            else
            {
                printf(".");    
            }
        }
        p += 16;
        printf(" \r\n");
    }
    printf("%#10lx:",(unsigned long)(intptr_t)p);
    for (i = 0; i< lastline; i++)
    {
        dat = p[i]; 
        printf("%02x ", dat);
    }
    printf("| ");
    for (i = 0; i< lastline; i++)
    {
        dat = p[i]; 
        if(isprint(dat))
        {
            printf("%c ", dat);
        }
        else
        {
            printf(".");    
        }
    }
    printf(" \r\n");
    return 0;	
}

/**
*****************************************************************************
* \fn          void ShowLogFun(void* param,void * cmdbuf);
* \brief       获取日志.
* \note        .
* \param[in]   cmdbuf 命令行字符串,
*  ie "runtime"
*****************************************************************************
*/
void ShowLogFun(void* param,void* cmdbuf)
{
    char type[32] = {0};
    int item;
    int len = sscanf((char const*)cmdbuf, "%*s %31s %d", (char*)&type, &item);
    uint8_t buff[LOG_MAXSIZE];
    memset(buff,0,LOG_MAXSIZE);
    if(len == 2)
    {
        LOG_TYPE_e cmdtype = LOG_INFO;

        if(memcmp(type, "info", strlen("info")) == 0)
        {
            cmdtype = LOG_INFO;
        }
        else if(memcmp(type, "warn", strlen("warn")) == 0)
        {
            cmdtype = LOG_WARN;
        }
        else if(memcmp(type, "err", strlen("err")) == 0)
        {
            cmdtype = LOG_ERR;
        }
        else
        {
            cmdtype = LOG_FAULT;
        }
        if(0==log_read(cmdtype,buff,sizeof(buff),item))
        {
             printfbuff(buff,sizeof(buff));
        }
        else
        {
            printf("err\r\n");
        }

    }
    else
    {
    }
}

int main(void)
{
    log_init();
    char cmd[1024];

    while(1)
    {
        printf("log info|warn|err|fault \r\n");
        gets(cmd);
        ShowLogFun(0,cmd);
    }
}