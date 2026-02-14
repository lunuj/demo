#include <string.h>
#include "core/nes/cpu.h"
#include "core/nes/bus.h"
#include "log.h"

/* 指令 地址 操作数*/
#pragma region "寄存器"
enum cpu_reg_map
{
    CPU_REG_REGA = 0,
    CPU_REG_REGX = 1,
    CPU_REG_REGY = 2,
    CPU_REG_REGP = 3,
    CPU_REG_REGPC_L = 4,
    CPU_REG_REGPC_H = 5,
    CPU_REG_REGSP_L = 6,
    CPU_REG_REGSP_H = 7,
};

struct cpu_reg
{
    u8 a;
    u8 x;
    u8 y;
    u8 p;
    u16 pc;
    u8 sp;
};

static struct cpu_reg __cpu;

#define __a __cpu.a
#define __p __cpu.p
#define __x __cpu.x
#define __y __cpu.y
#define __pc __cpu.pc
#define __sp __cpu.sp

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

#define SET_FLAG(f, FLAG_V) \
    do { if (FLAG_V) __cpu.p |= (f); else __cpu.p &= ~(f); } while (0)

#define GET_FLAG(f) ((__cpu.p & (f)) != 0)

void stack_push(u8 data)
{
    __sp = data;
    __sp++;
}

u8 stack_pop()
{
    __sp --;
    return __sp;
}

u16 get_int_prt_addr()
{
    return bus_read(0xFFFE) | (bus_read(0xFFFF) << 8);
}

static u8 cpu_read(u16 addr)
{
    u8 data = 0;
    switch (addr)
    {
    case CPU_REG_REGA:
        data = __cpu.a;
        break;
    case CPU_REG_REGX:
        data = __cpu.x;
        break;
    case CPU_REG_REGY:
        data = __cpu.y;
        break;
    case CPU_REG_REGP:
        data = __cpu.p;
        break;
    case CPU_REG_REGPC_L:
        data = __cpu.pc & 0xFF;
        break;
    case CPU_REG_REGPC_H:
        data = __cpu.pc >> 8;
        break;
    case CPU_REG_REGSP_L:
        data = __cpu.sp & 0xFF;
        break;
    case CPU_REG_REGSP_H:
        data = __cpu.sp >> 8;
        break;
    default:
        LOG_L(LOG_FATAL, "Try to access cpu reg %#x!", addr);
        break;
    }
    return data;
}

static void cpu_write(u16 addr, u8 data)
{
    switch (addr)
    {
    case CPU_REG_REGA:
        __cpu.a = data;
        break;
    case CPU_REG_REGX:
        __cpu.x = data;
        break;
    case CPU_REG_REGY:
        __cpu.y = data;
        break;
    case CPU_REG_REGP:
        __cpu.p = data;
        break;
    case CPU_REG_REGPC_L:
        __cpu.pc = data + (__cpu.pc & 0xFF);
        break;
    case CPU_REG_REGPC_H:
        __cpu.pc = (data << 8) + (__cpu.pc >>8);
        break;
    case CPU_REG_REGSP_L:
        __cpu.sp = data + (__cpu.pc & 0xFF);
        break;
    case CPU_REG_REGSP_H:
        __cpu.sp = (data << 8) + (__cpu.pc >>8);
        break;
    default:
        LOG_L(LOG_FATAL, "Try to access cpu reg %#x!", addr);
        break;
    }
}
#pragma endregion

#pragma region "寻址模式"
/**
 * 12 种寻址方式
 * IMP, Implied
 * IMM, Immediate
 * ZP0, Zero page
 * ZPX, Zero page. X
 * ZPY, Zero page, Y
 * REL, Relative
 * ABS, Absolute 
 * ABX, Absolute. X
 * ABY, Absolute. Y
 * IND, Indirect
 * IZX, Indexed indirect zero page, x
 * IZY, Indexed indirect zero page, x
 */
u16 IMP()
{
    return CPU_MAP_BASE;
}
u16 IMM()
{
    return __pc++;
}
u16 ZP0()
{
    return bus_read(__pc++);
}
u16 ZPX()
{
    return (bus_read(__pc++) + __x) & 0xFF;
}
u16 ZPY()
{
    return (bus_read(__pc++) + __y) & 0xFF;
}
u16 REL()
{
    u16 addr = bus_read(__pc++);
    return addr | ((addr>> 7) * 0xFF00);
}
u16 ABS()
{
    __pc += 2;
    return bus_read(__pc - 2) | (bus_read(__pc -1) << 8);
}
u16 ABX()
{
    __pc += 2;
    return (bus_read(__pc - 2) | (bus_read(__pc -1) << 8)) + __x;
}
u16 ABY()
{
    __pc += 2;
    return (bus_read(__pc - 2) | (bus_read(__pc -1) << 8)) + __y;
}
u16 IND()
{
    u16 tmp;
    __pc += 2;
    tmp = bus_read(__pc - 2) | (bus_read(__pc -1) << 8);
    return bus_read(tmp) | (bus_read(tmp) << 8);
}
u16 IZX()
{
    u16 tmp = (bus_read(__pc++) + __x) & 0xFF;
    return bus_read(tmp) | (bus_read(tmp) << 8);
}
u16 IZY()
{
    u16 tmp = bus_read(__pc++) & 0xFF;
    return bus_read(tmp) | (bus_read(tmp) << 8) + __y;
}
#pragma endregion

#pragma region "指令操作"
#   pragma region "Access"
void __LDR(u8 *reg, u16 addr)
{
    *reg = bus_read(addr);
    SET_FLAG(FLAG_Z, *reg == 0);
    SET_FLAG(FLAG_Z, *reg >> 7);
}
void __STR(u8 *reg, u16 addr) { bus_write(addr, *reg); }
void LDA(u16 addr) { UNUSED(addr); __LDR(&__a, addr); }
void LDX(u16 addr) { UNUSED(addr); __LDR(&__x, addr); }
void LDY(u16 addr) { UNUSED(addr); __LDR(&__y, addr); }
void STA(u16 addr) { UNUSED(addr); __STR(&__a, addr); }
void STX(u16 addr) { UNUSED(addr); __STR(&__x, addr); }
void STY(u16 addr) { UNUSED(addr); __STR(&__y, addr); }
#   pragma endregion

#   pragma region "Transfer"
void __TRR(u8 *reg, u8 data)
{
    *reg = data;
    SET_FLAG(FLAG_Z, *reg == 0);
    SET_FLAG(FLAG_N, *reg >> 7);
}
void TAX(u16 addr) { UNUSED(addr); __TRR(&__x, __a); }
void TXA(u16 addr) { UNUSED(addr); __TRR(&__a, __x); }
void TAY(u16 addr) { UNUSED(addr); __TRR(&__y, __a); }
void TYA(u16 addr) { UNUSED(addr); __TRR(&__a, __y); }
#   pragma endregion

#   pragma region "Arithmetic"
void ADC(u16 addr)
{
    size_t tmp = __a + bus_read(addr) + GET_FLAG(FLAG_C);
    SET_FLAG(FLAG_C, tmp < __a || tmp < bus_read(addr));
    SET_FLAG(FLAG_V, (tmp ^ __a) >> 7 && (tmp ^ bus_read(addr)) >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    SET_FLAG(FLAG_N, tmp >> 7);
    __a = tmp;
}
void SBC(u16 addr)
{
    u8 tmp = __a + ~bus_read(addr) + GET_FLAG(FLAG_C);
    SET_FLAG(FLAG_C, tmp < __a || tmp < bus_read(addr));
    SET_FLAG(FLAG_V, (tmp ^ __a) >> 7 && (tmp ^ bus_read(addr)) >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    SET_FLAG(FLAG_N, tmp >> 7);
    __a = tmp;
}
void INC(u16 addr)
{
    size_t tmp = bus_read(addr) + 1;
    bus_write(addr, tmp);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    SET_FLAG(FLAG_N, tmp >> 7);
}
void DEC(u16 addr)
{
    size_t tmp = bus_read(addr) - 1;
    bus_write(addr, tmp);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    SET_FLAG(FLAG_N, tmp >> 7);

}
void __INR(u8 *reg)
{
    (*reg)++;
    SET_FLAG(FLAG_Z, *reg == 0x0);
    SET_FLAG(FLAG_N, *reg >> 7);
}
void __DER(u8 *reg)
{
    (*reg)--;
    SET_FLAG(FLAG_Z, *reg == 0x0);
    SET_FLAG(FLAG_N, *reg >> 7);
}
void INX(u16 addr) { UNUSED(addr); __INR(&__x); }
void INY(u16 addr) { UNUSED(addr); __INR(&__y); }
void DEX(u16 addr) { UNUSED(addr); __DER(&__x); }
void DEY(u16 addr) { UNUSED(addr); __DER(&__y); }
#   pragma endregion

#   pragma region "Shift"
void ASL(u16 addr)
{
    size_t tmp = bus_read(addr);
    SET_FLAG(FLAG_C, tmp >> 7);
    tmp <<= 1;
    SET_FLAG(FLAG_N, tmp >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    bus_write(addr, tmp);
}
void LSR(u16 addr)
{
    size_t tmp = bus_read(addr);
    SET_FLAG(FLAG_C, tmp & 0x01);
    tmp >>= 1;
    SET_FLAG(FLAG_N, tmp >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    bus_write(addr, tmp);
}
void ROL(u16 addr)
{
    size_t tmp = bus_read(addr);
    tmp <<= 1;
    tmp |= GET_FLAG(FLAG_C);
    SET_FLAG(FLAG_C, tmp >> 8);
    SET_FLAG(FLAG_N, tmp >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    bus_write(addr, tmp);
}
void ROR(u16 addr)
{
    size_t tmp = bus_read(addr);
    tmp |= GET_FLAG(FLAG_C) << 8;
    SET_FLAG(FLAG_C, tmp & 0x01);
    tmp >>= 1;
    SET_FLAG(FLAG_N, tmp >> 7);
    SET_FLAG(FLAG_Z, tmp == 0x0);
    bus_write(addr, tmp);
}
#   pragma endregion

#   pragma region "Bitwise"
void AND(u16 addr)
{
    size_t tmp = bus_read(addr);
    __a |= tmp;
    SET_FLAG(FLAG_N, __a >> 7);
    SET_FLAG(FLAG_Z, __a == 0x0);
}
void ORA(u16 addr)
{
    size_t tmp = bus_read(addr);
    __a |= tmp;
    SET_FLAG(FLAG_N, __a >> 7);
    SET_FLAG(FLAG_Z, __a == 0x0);
}
void EOR(u16 addr)
{
    size_t tmp = bus_read(addr);
    __a ^= tmp;
    SET_FLAG(FLAG_N, __a >> 7);
    SET_FLAG(FLAG_Z, __a == 0x0);
}
void BIT(u16 addr)
{
    size_t tmp = bus_read(addr);
    SET_FLAG(FLAG_N, tmp >> 7);
    SET_FLAG(FLAG_V, tmp >> 6);
    SET_FLAG(FLAG_Z, (__a & tmp) == 0x0);
}
#   pragma endregion

#   pragma region "Compare"
void __CMR(u8 data, u16 addr)
{
    size_t tmp = bus_read(addr);
    SET_FLAG(FLAG_N, data >= tmp);
    SET_FLAG(FLAG_Z, data == tmp);
    SET_FLAG(FLAG_Z, (data- tmp) >> 7);
}
void CMP(u16 addr)
{
    __CMR(__a, addr);
}
void CPX(u16 addr)
{
    __CMR(__x, addr);
}
void CPY(u16 addr)
{
    __CMR(__y, addr);
}
#   pragma endregion

#   pragma region "Branch"
void BCC(u16 addr)
{
    if (GET_FLAG(FLAG_C) == 0x0)
        __pc += bus_read(addr);
}
void BCS(u16 addr)
{
    if (GET_FLAG(FLAG_C) == 0x1)
        __pc += bus_read(addr);
}
void BEQ(u16 addr)
{
    if (GET_FLAG(FLAG_Z) == 0x0)
        __pc += bus_read(addr);
}
void BNE(u16 addr)
{
    if (GET_FLAG(FLAG_Z) == 0x1)
        __pc += bus_read(addr);
}
void BPL(u16 addr)
{
    if (GET_FLAG(FLAG_N) == 0x0)
        __pc += bus_read(addr);
}
void BMI(u16 addr)
{
    if (GET_FLAG(FLAG_N) == 0x1)
        __pc += bus_read(addr);
}
void BVC(u16 addr)
{
    if (GET_FLAG(FLAG_V) == 0x0)
        __pc += bus_read(addr);
}
void BVS(u16 addr)
{
    if (GET_FLAG(FLAG_V) == 0x1)
        __pc += bus_read(addr);
}
#   pragma endregion

#   pragma region "Jump"
void JMP(u16 addr)
{
    __pc = addr;
}
void JSR(u16 addr)
{
    --__pc;
    stack_push((__pc & 0xFF00) >> 8);
    stack_push((__pc & 0xFF) >> 8);
    __pc = addr;
}
void RTS(u16 addr)
{
    UNUSED(addr);
    __pc = stack_pop();
    __pc |= (stack_pop() << 8);
    ++__pc;
}
void BRK(u16 addr)
{
    UNUSED(addr);
    ++__pc;
    SET_FLAG(FLAG_I, 1);
    stack_push((__pc & 0xFF00) >> 8);
    stack_push((__pc & 0xFF) >> 8);
    SET_FLAG(FLAG_B, 1);
    stack_push(__p);
    SET_FLAG(FLAG_B, 0);
    __pc = get_int_prt_addr();
}
void RTI(u16 addr)
{
    UNUSED(addr);
    __p = stack_pop();
    SET_FLAG(FLAG_B, 0);
    SET_FLAG(FLAG_U, 0);
    __pc = stack_pop();
    __pc |= (stack_pop() << 8);
}
#   pragma endregion

#   pragma region "Stack"
void PHA(u16 addr)
{
    UNUSED(addr);
    stack_push(__a);
}
void PLA(u16 addr)
{
    UNUSED(addr);
    __a = stack_pop();
    SET_FLAG(FLAG_B, 1);
    SET_FLAG(FLAG_U, 1);
}
void PHP(u16 addr)
{
    UNUSED(addr);
    u8 tmp = __p;
    SET_FLAG(FLAG_B, 1);
    SET_FLAG(FLAG_U, 1);
    __p = stack_pop();
    __p = tmp;
}
void PLP(u16 addr)
{
    UNUSED(addr);
    __p = stack_pop();
    SET_FLAG(FLAG_U, 1);
}
void TXS(u16 addr)
{
    UNUSED(addr);
    __sp = __x;
}
void TSX(u16 addr)
{
    UNUSED(addr);
    __x = __sp;
    SET_FLAG(FLAG_Z, 1);
    SET_FLAG(FLAG_N, 1);
}
#pragma endregion

#pragma region "Flags"
void CLC(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_C, 0); }
void SEC(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_C, 1); }
void CLI(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_I, 0); }
void SEI(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_I, 0); }
void CLD(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_D, 0); }
void SED(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_D, 0); }
void CLV(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_V, 0); }
void SEV(u16 addr) { UNUSED(addr); SET_FLAG(FLAG_V, 0); }
#pragma endregion

#pragma region "Others"
void NOP(u16 addr)
{
    UNUSED(addr);
}
void XXX(u16 addr)
{
    UNUSED(addr);
}
#pragma endregion

#pragma region "Operation"

#define OP(instruct, address, cycle) \
{ \
    .instruction_func = instruct, \
    .addressing_func = address, \
    .cycles = cycle - 1\
}

struct operation __operations[4 * 8 * 8] = {
    OP( BRK, IMM, 7 ), OP( ORA, IZX, 6 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 3 ), OP( ORA, ZP0, 3 ), OP( ASL, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( PHP, IMP, 3 ), OP( ORA, IMM, 2 ), OP( ASL, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( NOP, IMP, 4 ), OP( ORA, ABS, 4 ), OP( ASL, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BPL, REL, 2 ), OP( ORA, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( ORA, ZPX, 4 ), OP( ASL, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( CLC, IMP, 2 ), OP( ORA, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( ORA, ABX, 4 ), OP( ASL, ABX, 7 ), OP( XXX, IMP, 7 ),
    OP( JSR, ABS, 6 ), OP( AND, IZX, 6 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( BIT, ZP0, 3 ), OP( AND, ZP0, 3 ), OP( ROL, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( PLP, IMP, 4 ), OP( AND, IMM, 2 ), OP( ROL, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( BIT, ABS, 4 ), OP( AND, ABS, 4 ), OP( ROL, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BMI, REL, 2 ), OP( AND, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( AND, ZPX, 4 ), OP( ROL, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( SEC, IMP, 2 ), OP( AND, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( AND, ABX, 4 ), OP( ROL, ABX, 7 ), OP( XXX, IMP, 7 ),
    OP( RTI, IMP, 6 ), OP( EOR, IZX, 6 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 3 ), OP( EOR, ZP0, 3 ), OP( LSR, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( PHA, IMP, 3 ), OP( EOR, IMM, 2 ), OP( LSR, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( JMP, ABS, 3 ), OP( EOR, ABS, 4 ), OP( LSR, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BVC, REL, 2 ), OP( EOR, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( EOR, ZPX, 4 ), OP( LSR, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( CLI, IMP, 2 ), OP( EOR, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( EOR, ABX, 4 ), OP( LSR, ABX, 7 ), OP( XXX, IMP, 7 ),
    OP( RTS, IMP, 6 ), OP( ADC, IZX, 6 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 3 ), OP( ADC, ZP0, 3 ), OP( ROR, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( PLA, IMP, 4 ), OP( ADC, IMM, 2 ), OP( ROR, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( JMP, IND, 5 ), OP( ADC, ABS, 4 ), OP( ROR, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BVS, REL, 2 ), OP( ADC, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( ADC, ZPX, 4 ), OP( ROR, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( SEI, IMP, 2 ), OP( ADC, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( ADC, ABX, 4 ), OP( ROR, ABX, 7 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 2 ), OP( STA, IZX, 6 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 6 ),
    OP( STY, ZP0, 3 ), OP( STA, ZP0, 3 ), OP( STX, ZP0, 3 ), OP( XXX, IMP, 3 ),
    OP( DEY, IMP, 2 ), OP( NOP, IMP, 2 ), OP( TXA, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( STY, ABS, 4 ), OP( STA, ABS, 4 ), OP( STX, ABS, 4 ), OP( XXX, IMP, 4 ),
    OP( BCC, REL, 2 ), OP( STA, IZY, 6 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 6 ),
    OP( STY, ZPX, 4 ), OP( STA, ZPX, 4 ), OP( STX, ZPY, 4 ), OP( XXX, IMP, 4 ),
    OP( TYA, IMP, 2 ), OP( STA, ABY, 5 ), OP( TXS, IMP, 2 ), OP( XXX, IMP, 5 ),
    OP( NOP, IMP, 5 ), OP( STA, ABX, 5 ), OP( XXX, IMP, 5 ), OP( XXX, IMP, 5 ),
    OP( LDY, IMM, 2 ), OP( LDA, IZX, 6 ), OP( LDX, IMM, 2 ), OP( XXX, IMP, 6 ),
    OP( LDY, ZP0, 3 ), OP( LDA, ZP0, 3 ), OP( LDX, ZP0, 3 ), OP( XXX, IMP, 3 ),
    OP( TAY, IMP, 2 ), OP( LDA, IMM, 2 ), OP( TAX, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( LDY, ABS, 4 ), OP( LDA, ABS, 4 ), OP( LDX, ABS, 4 ), OP( XXX, IMP, 4 ),
    OP( BCS, REL, 2 ), OP( LDA, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 5 ),
    OP( LDY, ZPX, 4 ), OP( LDA, ZPX, 4 ), OP( LDX, ZPY, 4 ), OP( XXX, IMP, 4 ),
    OP( CLV, IMP, 2 ), OP( LDA, ABY, 4 ), OP( TSX, IMP, 2 ), OP( XXX, IMP, 4 ),
    OP( LDY, ABX, 4 ), OP( LDA, ABX, 4 ), OP( LDX, ABY, 4 ), OP( XXX, IMP, 4 ),
    OP( CPY, IMM, 2 ), OP( CMP, IZX, 6 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( CPY, ZP0, 3 ), OP( CMP, ZP0, 3 ), OP( DEC, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( INY, IMP, 2 ), OP( CMP, IMM, 2 ), OP( DEX, IMP, 2 ), OP( XXX, IMP, 2 ),
    OP( CPY, ABS, 4 ), OP( CMP, ABS, 4 ), OP( DEC, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BNE, REL, 2 ), OP( CMP, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( CMP, ZPX, 4 ), OP( DEC, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( CLD, IMP, 2 ), OP( CMP, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( CMP, ABX, 4 ), OP( DEC, ABX, 7 ), OP( XXX, IMP, 7 ),
    OP( CPX, IMM, 2 ), OP( SBC, IZX, 6 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( CPX, ZP0, 3 ), OP( SBC, ZP0, 3 ), OP( INC, ZP0, 5 ), OP( XXX, IMP, 5 ),
    OP( INX, IMP, 2 ), OP( SBC, IMM, 2 ), OP( NOP, IMP, 2 ), OP( SBC, IMP, 2 ),
    OP( CPX, ABS, 4 ), OP( SBC, ABS, 4 ), OP( INC, ABS, 6 ), OP( XXX, IMP, 6 ),
    OP( BEQ, REL, 2 ), OP( SBC, IZY, 5 ), OP( XXX, IMP, 2 ), OP( XXX, IMP, 8 ),
    OP( NOP, IMP, 4 ), OP( SBC, ZPX, 4 ), OP( INC, ZPX, 6 ), OP( XXX, IMP, 6 ),
    OP( SED, IMP, 2 ), OP( SBC, ABY, 4 ), OP( NOP, IMP, 2 ), OP( XXX, IMP, 7 ),
    OP( NOP, IMP, 4 ), OP( SBC, ABX, 4 ), OP( INC, ABX, 7 ), OP( XXX, IMP, 7 )
};

struct operation * get_operation()
{
    return __operations + bus_read(__pc++);
}

// https://www.cnblogs.com/1bite/p/18521817
#pragma endregion

#pragma region "CPU"
static char cpu_name[] = "NES_CPU_6502";

/**
 * @brief  CPU初始化
 * @retval 返回 \c RET_ERR 表示失败，其他表示成功
 * @note 
 */
dev_id cpu_init()
{
    static dev_id cpu_id = RET_ERR;
    if(cpu_id != RET_ERR)
        bus_remove(cpu_id);
    cpu_id = bus_register(cpu_name, CPU_MAP_BASE, CPU_MAP_SIZE, &cpu_read, &cpu_write);
    memset(&__cpu, 0, sizeof(__cpu));
    return cpu_id;
}

/**
 * @brief  CPU执行一个周期
 * @retval 无
 * @note 指令需要多个周期才能执行完成
 */
void cpu_clock()
{
    static u8 cycle = 0;
    if(!cycle--)
    {
        struct operation *op = get_operation();
        u16 addr = op->addressing_func();
        op->instruction_func(addr);
        cycle = op->cycles;
    }
}
#pragma endregion