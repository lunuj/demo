#pragma once
#include <stdint.h>
#include "useful.h"
#include "core/nes/bus.h"

struct operation {
    void (*instruction_func)(u16);
    u16 (*addressing_func)(void);
    u64 instruction_code : 8;
    u64 addressing_code : 8;
    u64 cycles : 8;
    u64 __padding : 40;
};
struct operation * get_operation();

dev_id cpu_init();
void cpu_clock();
