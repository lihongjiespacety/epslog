/**
********************************************************************************
* \brief       日志管理模块
* \details     Copyright (c) 2019,天仪研究院
*              All rights reserved.
* \file        log.h
* \author
* \version     0.1
* \date        2019/08/27
* \note        每条日志记录以回车结束
* \since
* \par 修订记录
* - 2019年8月27日 初始版本
* \par 资源说明
********************************************************************************
*/
#ifndef _LOG_H
#define _LOG_H
#include <stdint.h>
#include "fm25v05.h"

#define LOG_MAXSIZE 100   /**< 一条日志的最大长度 */
#define LOG_TIMEOUT 2000  /**< LOG超时时间  单位mS*/
/**  每一个类型日志类型由两部分组成:前面n份内容,后面紧接着n份序号。内容大小为LOG_INFO_ITEMSIZE字节,序号占用2字节最大到LOG_INFO_ITEM-1.最多纪录65535条记录。
 *   INFO类型日志地址如下LOG_INFO_ADDR=LOG_BASEADDR:
 *   地址           LOG_BASEADDR
 *                  LOG_INFO_ADDR
 *
 *   内容           LOG_INFO(0)        |LOG_INFO(1)       |...                    |LOG_INFO(ITEM-1)      |  0  | 1   | ... | LOG_INFO_ITEM-1 |
 *   大小(字节)     LOG_INFO_ITEMSIZE  LOG_INFO_ITEMSIZE  LOG_INFO_ITEMSIZE       LOG_INFO_ITEMSIZE        2    2      2        2
 *
 *   WARN类型日志接着INFO日志存放:LOG_WARN_ADDR=(LOG_INFO_ADDR+(LOG_WARN_ITEMSIZE+2)*LOG_WARN_ITEM)
 */

#define LOG_BASEADDR FM25V05_LOGPARAM_ADDR

#define LOG_INFO_ITEMSIZE  64                                                                 /**< INFO日志一条大小 */
#define LOG_INFO_ITEM      100                                                                 /**< INFO日志条数 */
#define LOG_INFO_ADDR      LOG_BASEADDR                                                       /**< INFO日志地址 */

#define LOG_WARN_ITEMSIZE  64                                                                 /**< WARN日志一条大小 */
#define LOG_WARN_ITEM      100                                                                /**< WARN日志条数 */
#define LOG_WARN_ADDR      (LOG_INFO_ADDR+(LOG_WARN_ITEMSIZE+2)*LOG_WARN_ITEM)    /**< WARN日志地址 */

#define LOG_ERR_ITEMSIZE   64    /**< ERR日志一条大小 */
#define LOG_ERR_ITEM       100   /**< ERR日志条数 */
#define LOG_ERR_ADDR      (LOG_WARN_ADDR+(LOG_ERR_ITEMSIZE+2)*LOG_ERR_ITEM)        /**< ERR日志地址 */

#define LOG_FAULT_ITEMSIZE 80    /**< FAULT日志一条大小 */
#define LOG_FAULT_ITEM     100   /**< FAULT日志条数 */
#define LOG_FAULT_ADDR    (LOG_ERR_ADDR+(LOG_ERR_ITEMSIZE+2)*LOG_ERR_ITEM)    /**< FAULT日志地址 */

#define LOG_ENDADDR (LOG_FAULT_ADDR+(LOG_FAULT_ITEMSIZE+2)*LOG_FAULT_ITEM)    /**< 日志结束地址 */
#define LOG_SIZE (LOG_ENDADDR-LOG_BASEADDR)

#if LOG_ENDADDR > FM25V05_MAX_ADDR
    #error "LOG_ENDADDR 必须小于FRAM_SIZE"
#endif

/**
 * \enum LOG_TYPE_e
 * 日志类型.
 */
typedef enum
{
    LOG_INFO = 0,    /**< 信息日志 */
    LOG_WARN,      /**< 告警日志*/
    LOG_ERR,       /**< 错误日志*/
    LOG_FAULT,     /**< 异常日志 (FAULT中断时上下文)*/
    LOG_MAXNUM     /**< 日志类型数 */
} LOG_TYPE_e;

/**
 * \struct log_info_t
 * 日志信息结构体.
 */
typedef struct
{
    LOG_TYPE_e type_e;         /**< 日志类型*/
    uint32_t addr_u16;             /**< 日志起始地址*/
    uint16_t itemsize_u16;         /**< 一条日志的大小*/
    uint16_t item_u16;             /**< 日志总条数 */
}log_info_t;


/**
 *****************************************************************************
 * \fn          int8_t log_init(void);
 * \brief       初始化日志模块
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
 *****************************************************************************
 */
int8_t log_init(void);


/**
 *****************************************************************************
 * \fn          int8_t log_writestring(LOG_TYPE_e type,const char *format, ...)
 * \brief       记录日志
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[in]   format 格式化字符串.
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
 *****************************************************************************
 */
int8_t log_writestring(LOG_TYPE_e type, const char* format, ...);

/**
 *****************************************************************************
 * \fn          int8_t log_writebuff(LOG_TYPE_e type,uint8_t* buff,uint8_t needmutex)
 * \brief       记录日志
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[in]   buff 待写入的内容.
 * \param[in]   needmutex 0不需要互斥 1需要互斥多任务时调用.
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
 *****************************************************************************
 */
int8_t log_writebuff(LOG_TYPE_e type, uint8_t* buff, uint16_t len, uint8_t needmutex);

/**
 *****************************************************************************
 * \fn          int8_t log_read(LOG_TYPE_e type,uint8_t* buff,uint16_t bufflen,uint16_t item)
 * \brief       读日志
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[out]  buff 存储读到的日志内容.
 * \param[in]   bufflen 读缓冲区长度
 * \param[in]   item 最近的第item条记录
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
 *****************************************************************************
 */
int8_t log_read(LOG_TYPE_e type, uint8_t* buff, uint16_t bufflen, uint16_t item);

/**
 *****************************************************************************
 * \fn          void log_setctrl(uint8_t ctrl)
 * \brief       设置日志写使能控制
 * \param[in]   ctrl LOG_TYPE_e对应的bit为1表示使能写对应日志,否则不使能写日志
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
 *****************************************************************************
 */
void log_setctrl(uint8_t ctrl);

/**
 *****************************************************************************
 * \fn          iuint8_t log_getctrl(void)
 * \brief       获取日志写使能控制
 * \note        .
 * \return      uint8_t LOG_TYPE_e对应的bit为1表示使能写对应日志,否则不使能写日志
 *****************************************************************************
 */
uint8_t log_getctrl(void);

#endif
