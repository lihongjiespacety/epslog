/**
 *****************************************************************************
 * \brief       模块相关数据结构和接口描述.
 * \details     Copyright (c) 2019,spacety.
 *              All rights reserved.
 * \file        bsp_time.h
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

#ifndef FM25V05_H
#define FM25V05_H

#include <stdint.h>
///#include "bsp_typedef.h"

#define FM25V05_PROTECT_PIN GPIO_CH_FRAM_PROTECT  /**< GPIO写保护引脚 */
#define FM25V05_HOLD_PIN GPIO_CH_FRAM_HOLD  /**< GPIO HOLD引脚 */

#define FRAM_TIMEOUT 2000  /**< 超时时间  单位mS*/
#define SPI_READWRITE_TIMEOUT 10000 /**< 读写SPI超时时间 单位uS */

/* fm25v05 spi 接口属性定义 */
#define FM25V05_SPI_ID     SPI_ID_1
#define FM25V05_SPI_MODE   SPI_MODE_3
#define FM25V05_SPEED      SPI_SPEED_APB2CLK_DIV4
#define FM25V05_DATA_BIT_LEN  8

/*FM25V05实际SPI时钟频率*/
#define FM25V05_SPEED_Hz   30000000  /*与SPI_SPEED_APB2CLK_DIV4 对应*/

/* fm25v05 命令定义*/
#define FM25V05_CMD_WREN   0X06      /*写使能命令*/
#define FM25V05_CMD_WRDI   0X04      /*写禁止命令*/
#define FM25V05_CMD_RDSR   0X05      /*状态寄存器读取命令*/
#define FM25V05_CMD_WRSR   0X01      /*状态寄存器写入命令*/
#define FM25V05_CMD_READ   0X03      /*FRAM数据读取命令*/
#define FM25V05_CMD_FSTRD  0X0B      /*FRAM数据快速读取命令*/
#define FM25V05_CMD_WRITE  0X02      /*FRAM数据写入命令*/
#define FM25V05_CMD_SLEEP  0XB9      /*进入睡眠模式命令*/
#define FM25V05_CMD_RDID   0X9F      /*读取设备ID命令*/

/*FM25V05 最大地址范围 64KB*/
#define FM25V05_BOOTPARAM_ADDR   0          /**< boot参数地址 */
#define FM25V05_BOOTPARAM_SIZE  (1*512)     /**< boot参数大小 */

#define FM25V05_FAULTREC_ADDR   (FM25V05_BOOTPARAM_ADDR+FM25V05_BOOTPARAM_SIZE)   /**< 异常记录地址 */
#define FM25V05_FAULTREC_SIZE   (1*512)                                           /**< 异常记录大小 */

#define FM25V05_DEFPARAM_ADDR   (FM25V05_FAULTREC_ADDR+FM25V05_FAULTREC_SIZE)     /**< 默认参数地址 */
#define FM25V05_DEPARAM_SIZE    (10*1024)                                         /**< 默认参数大小 */

#define FM25V05_RUNPARAM_ADDR   (FM25V05_DEFPARAM_ADDR+FM25V05_DEPARAM_SIZE)      /**< 运行参数地址 */
#define FM25V05_RUNPARAM_SIZE   (10*1024)                                         /**< 运行参数大小 */

#define FM25V05_DS18B20PARAM_ADDR   (FM25V05_RUNPARAM_ADDR+FM25V05_RUNPARAM_SIZE)  /**< DS18B20参数地址 */
#define FM25V05_DS18B20PARAM_SIZE   (1*1024+512)                                       /**< DS18B20参数大小 */

#define FM25V05_LOGPARAM_ADDR   (FM25V05_DS18B20PARAM_ADDR+FM25V05_DS18B20PARAM_SIZE)   /**< 日志地址 */
//#define FM25V05_LOGPARAM_SIZE   剩余的为日志区


#define FM25V05_MAX_ADDR   (1024*64)

/*****************************************************************************
 *                                                                           *
 *                             外部接口函数声明                              *
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
int32_t fm25v05_init(void);

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
int32_t fm25v05_read(uint8_t* outbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex);

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
int32_t fm25v05_write(uint8_t* inbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex);

#endif /* FM25V05_H */
