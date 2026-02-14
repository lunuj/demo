#pragma once
#include "useful.h"

#define PPU_MAP_BASE 0x2000
#define PPU_MAP_SIZE 0x2000

#define APU_MAP_BASE 0x4000
#define APU_MAP_SIZE 0x0018

#define CPU_MAP_BASE 0x4F00
#define CPU_MAP_SIZE 0x0100

#define SRAM_MAP_BASE 0x5000
#define SRAM_MAP_SIZE 0x3000

#define CART_MAP_BASE 0x8000
#define CART_MAP_SIZE 0x8000

typedef int dev_id;
typedef u8 (*read_fn)(u16 addr);
typedef void (*write_fn)(u16 addr, u8 data);

dev_id bus_register(char *dev_name, u16 addr, u16 len,  read_fn, write_fn);
void bus_remove(dev_id dev);

u8 bus_read(u16 addr);
void bus_write(u16 addr, u8 data);