/**
 *****************************************************************************
 * \brief       fm25v05 FRAM����ģ����ؽӿ�ʵ��.
 * \details     Copyright (c) 2019,spacety.
 *              All rights reserved.
 * \file        fm25v05.c
 * \author      yangjuju1990@163.com
 * \version     1.0
 * \date        2019-09-11
 * \note        ʹ��ǰ�ο�ע��.\n
 *              .\n
 * \since       yang 2019-09-11 1.0  �½�
 * \par �޶���¼
 * - 2019-09-11 ��ʼ�汾
 * \par ��Դ˵��
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
 *                             �ⲿ�ӿں���ʵ��                              *
 *                                                                           *
 ****************************************************************************/
/**
 *****************************************************************************
 * \fn          int32_t fm25v05_init(void)
 * \brief       fm25v05 ��ʼ������.
 * \note        �˺�������spi�ӿڳ�ʼ��������ͬʱʹ��fm25v05д�룬��ֹд����
 * \param[in]   ��.
 * \param[out]  ��.
 * \return      0 ����ɹ� ����ʧ��.
 *****************************************************************************
 */
int32_t fm25v05_init(void)
{
    size_t size;
    FILE* file;
    if(0 == access("log.bin", F_OK))
    {
        /*�ļ����ڲ���� ��  ����ʹ��a+ģʽ a+ģʽfseekʧЧ*/
        file = fopen("log.bin","rb+");  
    }
    else
    {
        /*�ļ������� */
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
 * \brief       fm25v05 FRAM���ݶ�ȡ����.
 * \note        �˺�����fram�а�ָ����ַ��ȡָ�����ȣ��ֽڣ�����
 * \param[in]   addr_u16 ��ȡ��ʼ��ַ.
 * \param[in]   len_u16 ��ȡ���ݵĳ���
 * \param[in]   needmutex 0����Ҫ���� 1��Ҫ���������ʱ����.
 * \param[out]  outbuff_pu8 ��ȡ���������������.
 * \return      0 ����ɹ� ����ʧ��.
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
 * \brief       fm25v05 FRAM����д�뺯��.
 * \note        �˺�����ָ����ַ��д��ָ�����ȣ��ֽڣ����ݵ�fram��
 * \param[in]   addr_u16 д����ʼ��ַ.
 * \param[in]   len_u16 д�����ݵĳ���
 * \param[in]   inbuff_pu8 ��Ҫд����������������.
 * \param[in]   needmutex 0����Ҫ���� 1��Ҫ���������ʱ����.
 * \return      0 ����ɹ� ����ʧ��.
 *****************************************************************************
 */
int32_t fm25v05_write(uint8_t* inbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex)
{
    memcpy(&gbuffer[addr_u16-FM25V05_LOGPARAM_ADDR],inbuff_pu8,len_u16);
    return 0;
}
