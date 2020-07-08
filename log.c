#include "log.h"
#include "fm25v05.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
static uint8_t  s_log_ctrl_u8  = (uint8_t)0xFF;       /**<  ��־ʹ�ܱ�־LOG_TYPE_e��Ӧ��bitΪ1��ʾʹ��д��Ӧ��־,����ʹ��д��־    */

/*�ṹ������*/
const log_info_t g_log_info_at[LOG_MAXNUM]=
{
    {LOG_INFO, LOG_INFO_ADDR, LOG_INFO_ITEMSIZE, LOG_INFO_ITEM},
    {LOG_WARN, LOG_WARN_ADDR, LOG_WARN_ITEMSIZE, LOG_WARN_ITEM},
    {LOG_ERR, LOG_ERR_ADDR, LOG_ERR_ITEMSIZE, LOG_ERR_ITEM},
    {LOG_FAULT, LOG_FAULT_ADDR, LOG_FAULT_ITEMSIZE, LOG_FAULT_ITEM},
};

/**
 *****************************************************************************
 * \fn          static int16_t log_findcurrentindex(LOG_TYPE_e type,uint16_t* index,uint8_t needmutex)
 * \brief       ���ҵ�ǰ�����Чֵ����
 * \param[in]   type \ref LOG_TYPE_e��־����
 * \param[in]   needmutex 0����Ҫ���� 1��Ҫ���������ʱ����.
 * \param[out]  �洢�����Ч���ݶ�Ӧ�����
 * \note        .
 * \retval      -2 - ʧ��
 * \retval      -1 - δ��ʼ��
 * \retval      >=0 - ����
 *****************************************************************************
 */
static int16_t log_findcurrentindex(LOG_TYPE_e type, uint16_t* index, uint8_t needmutex)
{
    uint16_t preindex=0;
    uint16_t nextindex=0;
    uint32_t i=0;
    uint32_t addr=0;
    uint32_t addroffset = 0;
    uint8_t addrbuff[2] = {0};
    uint8_t init = 0;
    uint8_t currentindex = 0;
    addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*�������ַ*/

    if(0 == fm25v05_read(addrbuff, addr, 2, needmutex))
    {
        preindex = ((uint16_t)addrbuff[1] << 8) | addrbuff[0];  /*С��ģʽ*/
    }
    else
    {
        return -2;
    }

    for(i = 0; i < g_log_info_at[type].item_u16; i++)
    {
        addroffset += 2;
        addroffset %= (g_log_info_at[type].item_u16 * 2);

        if(0 == fm25v05_read(addrbuff, addr + addroffset, 2, needmutex))
        {
            nextindex = ((uint16_t)addrbuff[1] << 8) | addrbuff[0];  /*С��ģʽ*/

            if(nextindex == (uint16_t)((preindex + (uint16_t)1)))
            {
                init = 1; /*������������� ˵���г�ʼ��*/
                *index = nextindex;
                preindex = nextindex;
            }
            else
            {
                *index = preindex;
                currentindex = i;
                break;
            }
        }
        else
        {
            return -2;
        }
    }

    if(init == 0)
    {
        return 0; /*δ��ʼ��  ������������������������*/
    }
    else
    {
        return currentindex;
    }
}


/*******************************************************************************
 * \fn          int8_t log_init(void);
 * \brief       ��ʼ����־ģ��
 * \note        .
 * \retval      1 - ʧ��
 * \retval      0 - �ɹ�
 *****************************************************************************
 */
int8_t log_init()
{
    uint8_t i;
    uint32_t addr;
    uint8_t buff[4] = {0, 0, 0, 0};
    uint16_t preindex;
    int8_t ret = 0;
    fm25v05_init();
    
    for(i = 0; i < LOG_MAXNUM; i++)
    {
        if(log_findcurrentindex((LOG_TYPE_e)i, &preindex, 0) <= 0)
        {
            /*δ��ʼ��*/
            addr = g_log_info_at[i].addr_u16 + g_log_info_at[i].itemsize_u16 * g_log_info_at[i].item_u16; /*�������ַ*/

            /*��ʼ������*/
            if(0 != fm25v05_write(buff, addr, 2, 0))
            {
                ret = 1;
            }
            else {}
        }
        else
        {
        }
    }

    ///s_log_ctrl_u8 = g_param_run_t.log_ctrl_u8;
    return ret;
}


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
int8_t log_writestring(LOG_TYPE_e type, const char* format, ...)
{
    static uint8_t s_log_buff_au8[LOG_MAXSIZE];
    uint32_t addr;
    int16_t index;
    uint16_t preindex;
    uint8_t buff[2] = {0, 0};

    if((s_log_ctrl_u8 & (uint8_t)((uint8_t)1 << type)) == 0)
    {
        return 1;  /*������дֱ���˳�*/
    }

    va_list list;
    va_start(list, format);
    vsnprintf((char*)(&s_log_buff_au8[0]), LOG_MAXSIZE, format, list);
    va_end(list);
    index = log_findcurrentindex(type, &preindex, 1);

    if(index >= 0)
    {
        index = (index + 1) % g_log_info_at[type].item_u16;
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*ָ��������Ӧ�����ݵ�ַ*/

        if(0 == fm25v05_write(s_log_buff_au8, addr, g_log_info_at[type].itemsize_u16, 1))
        {
            preindex += 1;
            buff[0] = preindex & 0xFF;
            buff[1] = (preindex >> 8) & 0xFF;
            addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*�������ַ*/
            addr += index * 2;

            if(0 == fm25v05_write(buff, addr, 2, 1))
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}


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
int8_t log_writebuff(LOG_TYPE_e type, uint8_t* buff, uint16_t len, uint8_t needmutex)
{
    uint16_t wrlen;
    int16_t index;
    uint16_t preindex;
    uint32_t addr;
    uint8_t addrbuff[2];
    
    if((s_log_ctrl_u8 & (uint8_t)((uint8_t)1 << type)) == 0)
    {
        return 1;  /*������дֱ���˳�*/
    }

    if(len >= g_log_info_at[type].itemsize_u16)
    {
        wrlen = g_log_info_at[type].itemsize_u16;
    }
    else
    {
        wrlen = len;
    }

    index = log_findcurrentindex(type, &preindex, needmutex);

    if(index >= 0)
    {
        index = (index + 1) % g_log_info_at[type].item_u16;
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*ָ��������Ӧ�����ݵ�ַ*/

        if(0 == fm25v05_write(buff, addr, wrlen, needmutex))
        {
            preindex += 1;
            addrbuff[0] = preindex & 0xFF;
            addrbuff[1] = (preindex >> 8) & 0xFF;
            addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*�������ַ*/
            addr += index * 2;

            if(needmutex)
            {
                if(0 == fm25v05_write(addrbuff, addr, 2, 1))
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                if(0 == fm25v05_write(addrbuff, addr, 2, 0))
                {
                    return 0;
                }
                else
                {
                    return 1;
                }
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

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
int8_t log_read(LOG_TYPE_e type, uint8_t* buff, uint16_t bufflen, uint16_t item)
{
    int16_t index;
    uint32_t addr;
    uint16_t readlen;
    uint16_t preindex;

    index = log_findcurrentindex(type, &preindex, 1);

    if(index >= 0)
    {
        if(item >= g_log_info_at[type].item_u16)
        {
            item = 0;
        }
        else {}

        index = ((index + g_log_info_at[type].item_u16) - item) % g_log_info_at[type].item_u16;
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*ָ��������Ӧ�����ݵ�ַ*/

        if(bufflen <= g_log_info_at[type].itemsize_u16)
        {
            readlen = bufflen;
        }
        else
        {
            readlen = g_log_info_at[type].itemsize_u16;
        }

        if(0 == fm25v05_read(buff, addr, readlen, 1))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

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
void log_setctrl(uint8_t ctrl)
{
    s_log_ctrl_u8 = ctrl;
    ///g_param_run_t.log_ctrl_u8 = s_log_ctrl_u8;
}

/**
 *****************************************************************************
 * \fn          iuint8_t log_getctrl(void)
 * \brief       ��ȡ��־дʹ�ܿ���
 * \note        .
 * \return      uint8_t LOG_TYPE_e��Ӧ��bitΪ1��ʾʹ��д��Ӧ��־,����ʹ��д��־
 *****************************************************************************
 */
uint8_t log_getctrl(void)
{
    return s_log_ctrl_u8;
}
