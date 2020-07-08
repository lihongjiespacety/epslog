/**
 *****************************************************************************
 * \brief       ģ��������ݽṹ�ͽӿ�����.
 * \details     Copyright (c) 2019,spacety.
 *              All rights reserved.
 * \file        bsp_time.h
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

#ifndef FM25V05_H
#define FM25V05_H

#include <stdint.h>
///#include "bsp_typedef.h"

#define FM25V05_PROTECT_PIN GPIO_CH_FRAM_PROTECT  /**< GPIOд�������� */
#define FM25V05_HOLD_PIN GPIO_CH_FRAM_HOLD  /**< GPIO HOLD���� */

#define FRAM_TIMEOUT 2000  /**< ��ʱʱ��  ��λmS*/
#define SPI_READWRITE_TIMEOUT 10000 /**< ��дSPI��ʱʱ�� ��λuS */

/* fm25v05 spi �ӿ����Զ��� */
#define FM25V05_SPI_ID     SPI_ID_1
#define FM25V05_SPI_MODE   SPI_MODE_3
#define FM25V05_SPEED      SPI_SPEED_APB2CLK_DIV4
#define FM25V05_DATA_BIT_LEN  8

/*FM25V05ʵ��SPIʱ��Ƶ��*/
#define FM25V05_SPEED_Hz   30000000  /*��SPI_SPEED_APB2CLK_DIV4 ��Ӧ*/

/* fm25v05 �����*/
#define FM25V05_CMD_WREN   0X06      /*дʹ������*/
#define FM25V05_CMD_WRDI   0X04      /*д��ֹ����*/
#define FM25V05_CMD_RDSR   0X05      /*״̬�Ĵ�����ȡ����*/
#define FM25V05_CMD_WRSR   0X01      /*״̬�Ĵ���д������*/
#define FM25V05_CMD_READ   0X03      /*FRAM���ݶ�ȡ����*/
#define FM25V05_CMD_FSTRD  0X0B      /*FRAM���ݿ��ٶ�ȡ����*/
#define FM25V05_CMD_WRITE  0X02      /*FRAM����д������*/
#define FM25V05_CMD_SLEEP  0XB9      /*����˯��ģʽ����*/
#define FM25V05_CMD_RDID   0X9F      /*��ȡ�豸ID����*/

/*FM25V05 ����ַ��Χ 64KB*/
#define FM25V05_BOOTPARAM_ADDR   0          /**< boot������ַ */
#define FM25V05_BOOTPARAM_SIZE  (1*512)     /**< boot������С */

#define FM25V05_FAULTREC_ADDR   (FM25V05_BOOTPARAM_ADDR+FM25V05_BOOTPARAM_SIZE)   /**< �쳣��¼��ַ */
#define FM25V05_FAULTREC_SIZE   (1*512)                                           /**< �쳣��¼��С */

#define FM25V05_DEFPARAM_ADDR   (FM25V05_FAULTREC_ADDR+FM25V05_FAULTREC_SIZE)     /**< Ĭ�ϲ�����ַ */
#define FM25V05_DEPARAM_SIZE    (10*1024)                                         /**< Ĭ�ϲ�����С */

#define FM25V05_RUNPARAM_ADDR   (FM25V05_DEFPARAM_ADDR+FM25V05_DEPARAM_SIZE)      /**< ���в�����ַ */
#define FM25V05_RUNPARAM_SIZE   (10*1024)                                         /**< ���в�����С */

#define FM25V05_DS18B20PARAM_ADDR   (FM25V05_RUNPARAM_ADDR+FM25V05_RUNPARAM_SIZE)  /**< DS18B20������ַ */
#define FM25V05_DS18B20PARAM_SIZE   (1*1024+512)                                       /**< DS18B20������С */

#define FM25V05_LOGPARAM_ADDR   (FM25V05_DS18B20PARAM_ADDR+FM25V05_DS18B20PARAM_SIZE)   /**< ��־��ַ */
//#define FM25V05_LOGPARAM_SIZE   ʣ���Ϊ��־��


#define FM25V05_MAX_ADDR   (1024*64)

/*****************************************************************************
 *                                                                           *
 *                             �ⲿ�ӿں�������                              *
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
int32_t fm25v05_init(void);

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
int32_t fm25v05_read(uint8_t* outbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex);

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
int32_t fm25v05_write(uint8_t* inbuff_pu8, uint16_t addr_u16, uint16_t len_u16, uint8_t needmutex);

#endif /* FM25V05_H */
