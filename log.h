/**
********************************************************************************
* \brief       ��־����ģ��
* \details     Copyright (c) 2019,�����о�Ժ
*              All rights reserved.
* \file        log.h
* \author
* \version     0.1
* \date        2019/08/27
* \note        ÿ����־��¼�Իس�����
* \since
* \par �޶���¼
* - 2019��8��27�� ��ʼ�汾
* \par ��Դ˵��
********************************************************************************
*/
#ifndef _LOG_H
#define _LOG_H
#include <stdint.h>
#include "fm25v05.h"

#define LOG_MAXSIZE 100   /**< һ����־����󳤶� */
#define LOG_TIMEOUT 2000  /**< LOG��ʱʱ��  ��λmS*/
/**  ÿһ��������־���������������:ǰ��n������,���������n����š����ݴ�СΪLOG_INFO_ITEMSIZE�ֽ�,���ռ��2�ֽ����LOG_INFO_ITEM-1.����¼65535����¼��
 *   INFO������־��ַ����LOG_INFO_ADDR=LOG_BASEADDR:
 *   ��ַ           LOG_BASEADDR
 *                  LOG_INFO_ADDR
 *
 *   ����           LOG_INFO(0)        |LOG_INFO(1)       |...                    |LOG_INFO(ITEM-1)      |  0  | 1   | ... | LOG_INFO_ITEM-1 |
 *   ��С(�ֽ�)     LOG_INFO_ITEMSIZE  LOG_INFO_ITEMSIZE  LOG_INFO_ITEMSIZE       LOG_INFO_ITEMSIZE        2    2      2        2
 *
 *   WARN������־����INFO��־���:LOG_WARN_ADDR=(LOG_INFO_ADDR+(LOG_WARN_ITEMSIZE+2)*LOG_WARN_ITEM)
 */

#define LOG_BASEADDR FM25V05_LOGPARAM_ADDR

#define LOG_INFO_ITEMSIZE  64                                                                 /**< INFO��־һ����С */
#define LOG_INFO_ITEM      100                                                                 /**< INFO��־���� */
#define LOG_INFO_ADDR      LOG_BASEADDR                                                       /**< INFO��־��ַ */

#define LOG_WARN_ITEMSIZE  64                                                                 /**< WARN��־һ����С */
#define LOG_WARN_ITEM      100                                                                /**< WARN��־���� */
#define LOG_WARN_ADDR      (LOG_INFO_ADDR+(LOG_WARN_ITEMSIZE+2)*LOG_WARN_ITEM)    /**< WARN��־��ַ */

#define LOG_ERR_ITEMSIZE   64    /**< ERR��־һ����С */
#define LOG_ERR_ITEM       100   /**< ERR��־���� */
#define LOG_ERR_ADDR      (LOG_WARN_ADDR+(LOG_ERR_ITEMSIZE+2)*LOG_ERR_ITEM)        /**< ERR��־��ַ */

#define LOG_FAULT_ITEMSIZE 80    /**< FAULT��־һ����С */
#define LOG_FAULT_ITEM     100   /**< FAULT��־���� */
#define LOG_FAULT_ADDR    (LOG_ERR_ADDR+(LOG_ERR_ITEMSIZE+2)*LOG_ERR_ITEM)    /**< FAULT��־��ַ */

#define LOG_ENDADDR (LOG_FAULT_ADDR+(LOG_FAULT_ITEMSIZE+2)*LOG_FAULT_ITEM)    /**< ��־������ַ */
#define LOG_SIZE (LOG_ENDADDR-LOG_BASEADDR)

#if LOG_ENDADDR > FM25V05_MAX_ADDR
    #error "LOG_ENDADDR ����С��FRAM_SIZE"
#endif

/**
 * \enum LOG_TYPE_e
 * ��־����.
 */
typedef enum
{
    LOG_INFO = 0,    /**< ��Ϣ��־ */
    LOG_WARN,      /**< �澯��־*/
    LOG_ERR,       /**< ������־*/
    LOG_FAULT,     /**< �쳣��־ (FAULT�ж�ʱ������)*/
    LOG_MAXNUM     /**< ��־������ */
} LOG_TYPE_e;

/**
 * \struct log_info_t
 * ��־��Ϣ�ṹ��.
 */
typedef struct
{
    LOG_TYPE_e type_e;         /**< ��־����*/
    uint32_t addr_u16;             /**< ��־��ʼ��ַ*/
    uint16_t itemsize_u16;         /**< һ����־�Ĵ�С*/
    uint16_t item_u16;             /**< ��־������ */
}log_info_t;


/**
 *****************************************************************************
 * \fn          int8_t log_init(void);
 * \brief       ��ʼ����־ģ��
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
int8_t log_init(void);


/**
 *****************************************************************************
 * \fn          int8_t log_writestring(LOG_TYPE_e type,const char *format, ...)
 * \brief       ��¼��־
 * \param[in]   type \ref LOG_TYPE_e��־����
 * \param[in]   format ��ʽ���ַ���.
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
int8_t log_writestring(LOG_TYPE_e type, const char* format, ...);

/**
 *****************************************************************************
 * \fn          int8_t log_writebuff(LOG_TYPE_e type,uint8_t* buff,uint8_t needmutex)
 * \brief       ��¼��־
 * \param[in]   type \ref LOG_TYPE_e��־����
 * \param[in]   buff ��д�������.
 * \param[in]   needmutex 0����Ҫ���� 1��Ҫ���������ʱ����.
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
int8_t log_writebuff(LOG_TYPE_e type, uint8_t* buff, uint16_t len, uint8_t needmutex);

/**
 *****************************************************************************
 * \fn          int8_t log_read(LOG_TYPE_e type,uint8_t* buff,uint16_t bufflen,uint16_t item)
 * \brief       ����־
 * \param[in]   type \ref LOG_TYPE_e��־����
 * \param[out]  buff �洢��������־����.
 * \param[in]   bufflen ������������
 * \param[in]   item ����ĵ�item����¼
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
int8_t log_read(LOG_TYPE_e type, uint8_t* buff, uint16_t bufflen, uint16_t item);

/**
 *****************************************************************************
 * \fn          void log_setctrl(uint8_t ctrl)
 * \brief       ������־дʹ�ܿ���
 * \param[in]   ctrl LOG_TYPE_e��Ӧ��bitΪ1��ʾʹ��д��Ӧ��־,����ʹ��д��־
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
void log_setctrl(uint8_t ctrl);

/**
 *****************************************************************************
 * \fn          iuint8_t log_getctrl(void)
 * \brief       ��ȡ��־дʹ�ܿ���
 * \note        .
 * \return      uint8_t LOG_TYPE_e��Ӧ��bitΪ1��ʾʹ��д��Ӧ��־,����ʹ��д��־
 *****************************************************************************
 */
uint8_t log_getctrl(void);

#endif
