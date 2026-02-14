#include <string.h>
#include "core/nes/ram.h"
#include "core/nes/bus.h"

#define RAM_BUFSIZE 0x800

static char ram_name[] = "RAM 0x800";
static u8 ram[RAM_BUFSIZE];

/**
 * @brief  读取RAM指定地址数据
 * @param  addr
 * @retval 指定地址的数据
 * @note 
 */
static u8 ram_read(u16 addr)
{
    return ram[addr & (RAM_BUFSIZE - 1)];
}

/**
 * @brief  向指定地址写入数据
 * @param  addr 要写入的地址
 * @param  data 要写入的数据
 * @retval 无
 * @note 
 */
static void ram_write(u16 addr, u8 data)
{
    ram[addr & (RAM_BUFSIZE - 1)] = data;
}

/**
 * @brief  RAM初始化
 * @retval 无
 * @note 
 */
dev_id ram_init()
{
    static dev_id ram_id = RET_ERR;
    if(ram_id != RET_ERR)
        bus_remove(ram_id);
    ram_id = bus_register(ram_name, RAM_MAP_BASE, RAM_MAP_SIZE, ram_read, ram_write);
    memset(ram, 0, sizeof(ram));
    return ram_id;
}