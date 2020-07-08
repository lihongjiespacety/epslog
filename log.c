#include "log.h"
#include "fm25v05.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
static uint8_t  s_log_ctrl_u8  = (uint8_t)0xFF;       /**<  日志使能标志LOG_TYPE_e对应的bit为1表示使能写对应日志,否则不使能写日志    */

/*结构体数据*/
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
 * \brief       查找当前最后有效值索引
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[in]   needmutex 0不需要互斥 1需要互斥多任务时调用.
 * \param[out]  存储最后有效数据对应的序号
 * \note        .
 * \retval      -2 - 失败
 * \retval      -1 - 未初始化
 * \retval      >=0 - 索引
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
    addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*序号区地址*/

    if(0 == fm25v05_read(addrbuff, addr, 2, needmutex))
    {
        preindex = ((uint16_t)addrbuff[1] << 8) | addrbuff[0];  /*小端模式*/
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
            nextindex = ((uint16_t)addrbuff[1] << 8) | addrbuff[0];  /*小端模式*/

            if(nextindex == (uint16_t)((preindex + (uint16_t)1)))
            {
                init = 1; /*有连续递增序号 说明有初始化*/
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
        return 0; /*未初始化  或者所有索引都是连续递增*/
    }
    else
    {
        return currentindex;
    }
}


/*******************************************************************************
 * \fn          int8_t log_init(void);
 * \brief       初始化日志模块
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
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
            /*未初始化*/
            addr = g_log_info_at[i].addr_u16 + g_log_info_at[i].itemsize_u16 * g_log_info_at[i].item_u16; /*序号区地址*/

            /*初始化序列*/
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
 * \brief       记录日志
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[in]   format 格式化字符串.
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
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
        return 1;  /*不允许写直接退出*/
    }

    va_list list;
    va_start(list, format);
    vsnprintf((char*)(&s_log_buff_au8[0]), LOG_MAXSIZE, format, list);
    va_end(list);
    index = log_findcurrentindex(type, &preindex, 1);

    if(index >= 0)
    {
        index = (index + 1) % g_log_info_at[type].item_u16;
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*指定索引对应的数据地址*/

        if(0 == fm25v05_write(s_log_buff_au8, addr, g_log_info_at[type].itemsize_u16, 1))
        {
            preindex += 1;
            buff[0] = preindex & 0xFF;
            buff[1] = (preindex >> 8) & 0xFF;
            addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*序号区地址*/
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
 * \brief       记录日志
 * \param[in]   type \ref LOG_TYPE_e日志类型
 * \param[in]   buff 待写入的内容.
 * \param[in]   needmutex 0不需要互斥 1需要互斥多任务时调用.
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
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
        return 1;  /*不允许写直接退出*/
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
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*指定索引对应的数据地址*/

        if(0 == fm25v05_write(buff, addr, wrlen, needmutex))
        {
            preindex += 1;
            addrbuff[0] = preindex & 0xFF;
            addrbuff[1] = (preindex >> 8) & 0xFF;
            addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * g_log_info_at[type].item_u16; /*序号区地址*/
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
        addr = g_log_info_at[type].addr_u16 + g_log_info_at[type].itemsize_u16 * index ; /*指定索引对应的数据地址*/

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
 * \brief       设置日志写使能控制
 * \param[in]   ctrl LOG_TYPE_e对应的bit为1表示使能写对应日志,否则不使能写日志
 * \note        .
 * \retval      1 - 失败
 * \retval      0 - 成功
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
 * \brief       获取日志写使能控制
 * \note        .
 * \return      uint8_t LOG_TYPE_e对应的bit为1表示使能写对应日志,否则不使能写日志
 *****************************************************************************
 */
uint8_t log_getctrl(void)
{
    return s_log_ctrl_u8;
}
