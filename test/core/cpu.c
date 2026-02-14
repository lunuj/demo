#define LOG_IMPLEMENTATION
#include "log.h"
#include "core/nes/cpu.h"
#include "core/nes/bus.h"
#include "core/nes/ram.h"

u8 rom[] = {
    0xa2, 0x00,         // LDX #$00
    0xa0, 0x00,         // LDY #$00
    0xe8,               // INX
    0x98,               // TYA
    0x49, 0x1f,         // EOR #$1f
    0xa8,               // TAY
    0x8a,               // TXA
    0x6d, 0x00, 0x00,   // ADC $FFFF
    0xaa,               // TAX
    0x88,               // DEY
    0xea,               // NOB
    0xea,               // NOB
    0x4c, 0x00, 0x00    // JMP $0000
};

int main()
{
    dev_id ret = 0;
    ret = cpu_init();
    LOG_ASSERT(ret != RET_ERR);
    ret = ram_init();
    LOG_ASSERT(ret != RET_ERR);

    for (size_t i = 0; i < sizeof(rom); i++)
    {
        bus_write(i, rom[i]);
    }
    
    u8 a;
    u8 x;
    u8 y;
    u8 p;
    u16 pc;
    u8 sp;
    while(1)
    {
        cpu_clock();
        a = bus_read(CPU_MAP_BASE);
        x = bus_read(CPU_MAP_BASE + 1);
        y = bus_read(CPU_MAP_BASE + 2);
        p = bus_read(CPU_MAP_BASE + 3);
        pc = bus_read(CPU_MAP_BASE+ 4) + (bus_read(CPU_MAP_BASE+ 5)>>8);
        sp = bus_read(CPU_MAP_BASE + 6);
        LOG("%#x %#x %#x %#x %#x %#x", a, x, y, p, pc, sp);
    }
}