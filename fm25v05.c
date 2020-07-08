/**
 *****************************************************************************
 * \brief       fm25v05 FRAM访问模块相关接口实现.
 * \details     Copyright (c) 2019,spacety.
 *              All rights reserved.
 * \file        fm25v05.c
 * \author      yangjuju1990@163.com
 * \version     1.0
 * \date        2019-09-11
 * \note        使用前参考注释.\n
 *              .\n
 * \since       yang 2019-09-11 1.0  新建
 * \par 修订记录
 * - 2019-09-11 初始版本
 * \par 资源说明
 * - RAM:
 * - ROM:
 *****************************************************************************
 */
#include "fm25v05.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>


uint8_t gbuffer[64*1024];
/*****************************************************************************
 *                                                                           *
 *                             外部接口函数实现                              *
 *                                                                           *
 ****************************************************************************/
/**
 *****************************************************************************
 * \fn          int32_t fm25v05_init(void)
 * \brief       fm25v05 初始化函数.
 * \note        此函数调用spi接口初始化函数，同时使能fm25v05写入，禁止写保护
 * \param[in]   无.
 * \param[out]  无.
 * \return      0 传输成功 其他失败.
 *****************************************************************************
 */
int32_t fm25v05_init(void)
{
    size_t size;
    FILE* file;
    if(0 == access("log.bin", F_OK))
    {
        /*文件存在不清除 打开  不能使用a+模式 a+模式fseek失效*/
        file = fopen("log.bin","rb+");  
    }
    else
    {
        /*文件不存在 */
        printf("open log.bin err\r\n");  
        return -1;
    }
    fseek(file,0,SEEK_END);
    size = ftell(file);
    if(size > 64*1024-FM25V05_LOGPARAM_ADDR)
    {
        size = 64*1024-FM25V05_LOGPARAM_ADDR;
    }
    int res1 = fseek(file,0, SEEK_SET);
    if (res1 < 0) {
        int err = -errno;
        printf("read lseek err\r\n");
        fclose(file);
        return err;
    }

    int res2 = fread(&gbuffer[0], 1,size,file);
    if ((res2 < 0) || (res2 != size)) {
        printf("read err to read %d,addr=%d,res=%d\r\n",(int)size,0,res2);
        int err = -errno;
        fclose(file);
        return err;
    }
    fclose(file);
    return 0;
}


/**
 *****************************************************************************
 * \fn          int32_t fm25v05_read(uint8_t *outbuff_pu8, uint16_t addr_u16, uint16_t len_u16,uint8_t needmutex)
 * \brief       fm25v05 FRAM数据读取函数.
 * \note        此函数从fram中按指定地址读取指定长度（字节）数据
 * \param[in]   addr_u16 读取起始地址.
 * \param[in]   len_u16 读取数据的长度
 * \param[in]   needmutex 0不需要互斥 1需要互斥多任务时调用.
 * \param[out]  outbuff_pu8 读取到数据输出缓冲区.
 * \return      0 传输成功 其他失败.
 *****************************************************************************
 */
int32_t fm25v05_read(uint8_t* outbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex)
{
    memcpy(outbuff_pu8,&gbuffer[addr_u16-FM25V05_LOGPARAM_ADDR],len_u16);
    return 0;
}

/**
 *****************************************************************************
 * \fn          int32_t fm25v05_write(uint8_t *inbuff_pu8, uint16_t addr_u16, uint16_t len_u16,uint8_t needmutex)
 * \brief       fm25v05 FRAM数据写入函数.
 * \note        此函数按指定地址，写入指定长度（字节）数据到fram中
 * \param[in]   addr_u16 写入起始地址.
 * \param[in]   len_u16 写入数据的长度
 * \param[in]   inbuff_pu8 需要写入的数据输出缓冲区.
 * \param[in]   needmutex 0不需要互斥 1需要互斥多任务时调用.
 * \return      0 传输成功 其他失败.
 *****************************************************************************
 */
int32_t fm25v05_write(uint8_t* inbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex)
{
    memcpy(&gbuffer[addr_u16-FM25V05_LOGPARAM_ADDR],inbuff_pu8,len_u16);
    return 0;
}
