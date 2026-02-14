#include <string.h>
#include "log.h"
#include "core/nes/bus.h"

#define BUSDEF static inline
#define BUS_DEV_MAX_NUM 10

static struct device
{
    char *name;
    u16 map_addr;
    u16 size;
    read_fn read;
    write_fn write;
} dev[BUS_DEV_MAX_NUM];
/* TODO 或许可以使用动态数组或者链表 */

/**
 * @brief  通过addr寻找到对应的dev_id
 * @param  addr 对应设备所映射的地址
 * @retval dev_id
 * @note 获取成功返回dev_id，获取失败返回-1。
 */
static dev_id bus_find_dev(u16 addr)
{
    dev_id id = RET_ERR;
    for (size_t i = 0; i < BUS_DEV_MAX_NUM; i++)
    {
        if(dev[i].name == NULL)
            continue;
        if(dev[i].map_addr <= addr && dev[i].map_addr + dev[i].size > addr)
        {
            id = i;
            break;
        }
    }
    return id;
}

/**
 * @brief  检测申请映射地址是否合法
 * @param  map_addr 映射地址
 * @param  size 映射大小
 * @retval \c RET_ERR: 非法, \c RET_OK: 合法
 * @note 
 */
static int bus_check_map(u16 map_addr, u16 size)
{
    if (bus_find_dev(map_addr) < 0 
        && bus_find_dev(map_addr + size / 2) < 0 
        && bus_find_dev(size + map_addr) < 0)
        return RET_OK;
    return RET_ERR;
}

/**
 * @brief  获取空闲设备id
 * @retval dev_id
 * @note 获取成功返回dev_id，获取失败返回-1。
 */
static dev_id bus_find_free_dev()
{
    dev_id id = RET_ERR;
    for (size_t i = 0; i < BUS_DEV_MAX_NUM; i++)
    {
        if(dev[i].name == NULL)
        {
            id = i;
            break;
        }
    }
    return id;
}

/**
 * @brief  向bus注册地址映射设备
 * @param  dev_name 设备名称
 * @param  map_addr 映射地址
 * @param  size 映射空间大小
 * @param  read 读取回调函数
 * @param  write 写入回调函数
 * @retval dev_id
 * @note 返回 \c RET_ERR 说明注册失败。
 */
dev_id bus_register(char * dev_name, u16 map_addr, u16 size, read_fn read, write_fn write)
{
    dev_id id = RET_ERR;

    if(bus_check_map(map_addr, size))
        goto no_free_dev;
    
    id = bus_find_free_dev();
    if(id < 0)
        goto no_free_dev;
    dev[id].name = dev_name;
    dev[id].map_addr = map_addr;
    dev[id].size = size;
    dev[id].read = read;
    dev[id].write = write;
no_free_dev:
    return id;
}

/**
 * @brief  卸载已注册的设备
 * @param  id \c bus_register 返回的 dev_id
 * @retval 无
 * @note 非法的 dev_id 将无任何效果。
 */
void bus_remove(dev_id id)
{
    if(id < 0 || id >= BUS_DEV_MAX_NUM)
        return;
    memset(dev + id, 0, sizeof(dev));
}

/**
 * @brief  读取总线对应地址数据
 * @param  addr 总线地址
 * @retval 读取的数据
 * @note 若读取非法地址进程将退出。
 */
u8 bus_read(u16 addr)
{
    dev_id id = bus_find_dev(addr);
    LOG_ASSERT(id != RET_ERR);
    return dev[id].read(addr - dev[id].map_addr);
}

/**
 * @brief  向总线地址写入数据
 * @param  addr 总线地址
 * @param  data 写入的数据
 * @retval 无
 * @note 若写入非法地址进程将退出。
 */
void bus_write(u16 addr, u8 data)
{
    dev_id id = bus_find_dev(addr);
    LOG_ASSERT(id != RET_ERR);
    dev[id].write(addr - dev[id].map_addr, data);
}