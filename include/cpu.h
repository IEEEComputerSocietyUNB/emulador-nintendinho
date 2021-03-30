#ifndef _CPU_H_
#define _CPU_H_
#include <stdint.h>

#define N_REG       5
#define RAM_SIZE    2048
#define IO_REG_SIZE 28

// Endereço das regiões de memória acessadas pela CPU

enum CPU_MEM_MAP{
    RAM = 0x0000,           // Memória interna da CPU
    IO_REG = 0x02000,       // Registradores de comunicação outras partes do sistema
    CPU_CART_MEM = 0x4020   // Memória do cartucho acessada pela CPU
};

// Endereços de acesso à memória do cartucho acessada pela CPU

enum CPU_CART_MEM_MAP{
    EX_ROM = 0x4020,        // Expansão da memória ROM ou RAM do cartucho
    SRAM = 0x6000,          // Memória RAM comumente usada em saves
    PRG_ROM_LB = 0x8000,    // Banco inferior de memória de código do programa
    PRG_ROM_UB = 0xC000     // Banco superior de memória de código do programa
};

// Registradores da CPU

enum CPU_REGISTER{
    A,          // Accumulator
    X,          // Index Register X
    Y,          // Index Register Y
    S,          // Stack Pointer
    P           // Processor Status Register
};

int8_t cpu_reg[N_REG];

// Program Counter

uint16_t PC; 

// Processor Status Register (Flags)

enum P_FLAGS{
    C,          // Carry            (0=No Carry, 1=Carry)
    Z,          // Zero             (0=Nonzero, 1=Zero)
    I,          // IRQ Disable      (0=IRQ Enable, 1=IRQ Disable)
    D,          // Decimal Mode     (0=Normal, 1=BCD Mode for ADC/SBC opcodes)
    B,          // Break Flag       (0=IRQ/NMI, 1=RESET or BRK/PHP opcode)
    _,          // Not Used         (Always 1)
    V,          // Overflow         (0=No Overflow, 1=Overflow)
    N           // Negative/Sign    (0=Positive, 1=Negative)
};

// Memória interna da CPU

int8_t cpu_ram[RAM_SIZE];

// Registradores de entrada e saída

int8_t io_reg[IO_REG_SIZE];

enum IO_REGISTER{
    PPUCTRL0       = 0x2000,  // PPU Control Register 1 (W)
    PPUCTRL1,                 // PPU Control Register 2 (W)
    PPUSTAT,                  // PPU Status Register (R)
    SPRRAMADDR,               // SPR-RAM Address Register (W)
    SPRRAMDATA,               // SPR-RAM Data Register (RW)
    PPUBGSCROLLOFST,          // PPU Background Scrolling Offset (W2)
    VRAMADDR,                 // VRAM Address Register (W2)
    VRAMDATA,                 // VRAM Read/Write Data Register (RW)
    APUCH1VOL      = 0x4000,  // APU Channel 1 (Rectangle) Volume/Decay (W)
    APUCH1SWP,                // APU Channel 1 (Rectangle) Sweep (W)
    APUCH1FRQ,                // APU Channel 1 (Rectangle) Frequency (W)
    APUCH1LEN,                // APU Channel 1 (Rectangle) Length (W)
    APUCH2VOL,                // APU Channel 2 (Rectangle) Volume/Decay (W)
    APUCH2SWP,                // APU Channel 2 (Rectangle) Sweep (W)
    APUCH2FRQ,                // APU Channel 2 (Rectangle) Frequency (W)
    APUCH2LEN,                // APU Channel 2 (Rectangle) Length (W)
    APUCH3LCNT,               // APU Channel 3 (Triangle) Linear Counter (W)
    APUCH3,                   // APU Channel 3 (Triangle) N/A (-)
    APUCH3FRQ,                // APU Channel 3 (Triangle) Frequency (W)
    APUCH3LEN,                // APU Channel 3 (Triangle) Length (W)
    APUCH4VOL,                // APU Channel 4 (Noise) Volume/Decay (W)
    APUCH4,                   // APU Channel 4 (Noise) N/A (-)
    APUCH4FRQ,                // APU Channel 4 (Noise) Frequency (W)
    APUCH4LEN,                // APU Channel 4 (Noise) Length (W)
    APUCH5PLAYMODE,           // APU Channel 5 (DMC) Play mode and DMA frequency (W)
    APUCH5CNTLDREG,           // APU Channel 5 (DMC) Delta counter load register (W)
    APUCH5ADDRLDREG,          // APU Channel 5 (DMC) Address load register (W)
    APUCH5LENREG,             // APU Channel 5 (DMC) Length register (W)
    SPRRAMDMA,                // SPR-RAM DMA Register (W)
    DMC            = 0x4016,  // DMC/IRQ/length counter status/channel enable register (RW)
    IRQ            = 0x4016, 
    LENCNTSTAT     = 0x4016, 
    CHANNELENABLE  = 0x4016,
    JOYPAD1,                  // Joypad #1 (RW)
    JOYPAD2        = 0x4017,  // Joypad #2/APU SOFTCLK (RW)
    APUSOFTCLK     = 0x4017
};

// Estrutura de memória do cartucho

typedef struct{
    uint8_t N_PRG_ROM_PAG;
    uint8_t N_CHR_ROM_PAG;
    int8_t* PRG_ROM;
    int8_t* CHR_ROM;
    int8_t* SRAM;
    int8_t* EX_ROM;
}Cartridge;

Cartridge cartridge;

// Mapper 0

int8_t* mapper(uint16_t logical_address);

// Referência para o mapeador utilizado no cartucho

// typedef int8_t* Mapper(uint16_t logical_address);

// Mapper* mapper = mapper_0;

// Mapeia os endereços acessados pela CPU para a estrutura correta de memória

int8_t* cpu_memory_mapper(uint16_t logical_address);

// A CPU escreve na memória

void cpu_write_memory(uint16_t logical_address, int8_t data);

// A CPU lê da memória

int8_t cpu_read_memory(uint16_t logical_address);

// Endereçamento 

enum ADDRESSING_MODE{
    NONE,               //Undefined
    IMP,                //Implied
    ACC,                //Accumulator
    IMM,                //Immediate
    ZP,                 //Zero Page
    ZPX,                //Zero Page X
    ZPY,                //Zero Page Y
    REL,                //Relative
    ABS,                //Absolute
    ABX,                //Absolute X
    ABY,                //Absolute Y
    IND,                //Indirect
    IZX,                //Indexed Indirect
    IZY                 //Indirect Indexed
};

enum UPDATE_FLAGS{
    xxxxxxxx = 0b00000000,
    CZIDxxVN = 0b11110011,
    xZxxxxxN = 0b01000001,
    CZxxxxxN = 0b01000001,
    CZxxxxVN = 0b11000011,
    xZxxxxVN = 0b11000010,
    Cxxxxxxx = 0b10000000,
    xxIxxxxx = 0b00100000,
    xxxDxxxx = 0b00010000,
    xxxxBxxx = 0b00001000,
    xxxxxxVx = 0b00000010
};

static const enum ADDRESSING_MODE address_mode_lookup[256] =
{    // x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF
/*0x*/  NONE, IZX,  NONE, IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*1x*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX,
/*2x*/  ABS,  IZX,  NONE, IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*3x*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX,
/*4x*/  NONE, IZX,  NONE, IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*5x*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX,
/*6x*/  NONE, IZX,  NONE, IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  IND,  ABS,  ABS,  ABS,
/*7x*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX,
/*8x*/  IMM,  IZX,  IMM,  IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*9x*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPY,  ZPY,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABY,  ABY,
/*Ax*/  IMM,  IZX,  IMM,  IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*Bx*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPY,  ZPY,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABY,  ABY,
/*Cx*/  IMM,  IZX,  IMM,  IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*Dx*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX,
/*Ex*/  IMM,  IZX,  IMM,  IZX,  ZP,   ZP,   ZP,   ZP,   NONE, IMM,  NONE, IMM,  ABS,  ABS,  ABS,  ABS,
/*Fx*/  REL,  IZY,  NONE, IZY,  ZPX,  ZPX,  ZPX,  ZPX,  NONE, ABY,  NONE, ABY,  ABX,  ABX,  ABX,  ABX
};

static const enum UPDATE_FLAGS update_flags_lookup[256] =
{   // x0        x1        x2        x3        x4        x5        x6        x7        x8        x9        xA        xB        xC        xD        xE        xF
/*0x*/ xxxxBxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, CZxxxxxN, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*1x*/ xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx, Cxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*2x*/ xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xZxxxxVN, xZxxxxxN, CZxxxxxN, xxxxxxxx, CZIDxxVN, xZxxxxxN, CZxxxxxN, CZxxxxxN, xZxxxxVN, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*3x*/ xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxxN, xxxxxxxx, Cxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*4x*/ CZIDxxVN, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, CZxxxxxN, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*5x*/ xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxIxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, CZxxxxxN, xxxxxxxx,
/*6x*/ xxxxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxVN, CZxxxxxN, xxxxxxxx, xZxxxxxN, CZxxxxVN, CZxxxxxN, CZxxxxVN, xxxxxxxx, CZxxxxVN, CZxxxxxN, xxxxxxxx,
/*7x*/ xxxxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxVN, CZxxxxxN, xxxxxxxx, xxIxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxVN, CZxxxxxN, xxxxxxxx,
/*8x*/ xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, xxxxxxxx, xZxxxxxN, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx,                                                                                                        
/*9x*/ xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xZxxxxxN, xxxxxxxx, xZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx, xxxxxxxx,
/*Ax*/ xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN,
/*Bx*/ xxxxxxxx, xZxxxxxN, xxxxxxxx, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xxxxxxVx, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xZxxxxxN, xxxxxxxx,
/*Cx*/ CZxxxxxN, CZxxxxxN, xxxxxxxx, xxxxxxxx, CZxxxxxN, CZxxxxxN, xZxxxxxN, xxxxxxxx, xxxxxxxx, CZxxxxxN, xZxxxxxN, CZxxxxxN, CZxxxxxN, CZxxxxxN, xZxxxxxN, xxxxxxxx,
/*Dx*/ xxxxxxxx, CZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxxN, xZxxxxxN, xxxxxxxx, xxxDxxxx, CZxxxxxN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxxN, xZxxxxxN, xxxxxxxx,
/*Ex*/ CZxxxxxN, CZxxxxVN, xxxxxxxx, xxxxxxxx, CZxxxxxN, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxVN, xxxxxxxx, CZxxxxVN, CZxxxxxN, CZxxxxVN, xxxxxxxx, xxxxxxxx,
/*Fx*/ xxxxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx, xxxDxxxx, CZxxxxVN, xxxxxxxx, xZxxxxxN, xxxxxxxx, CZxxxxVN, xxxxxxxx, xxxxxxxx
};

static const int clock_cycle_lookup[256] =
{   // x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
/*0x*/ 7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, 
/*1x*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
/*2x*/ 6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 
/*3x*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/*4x*/ 6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, 
/*5x*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
/*6x*/ 6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, 
/*7x*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
/*8x*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 0, 4, 4, 4, 4, 
/*9x*/ 2, 6, 0, 0, 4, 4, 4, 4, 2, 5, 2, 0, 0, 5, 0, 0, 
/*Ax*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 0, 4, 4, 4, 4, 
/*Bx*/ 2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 0, 4, 4, 4, 4, 
/*Cx*/ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 
/*Dx*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 
/*Ex*/ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 
/*Fx*/ 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7 
};

static const int page_breaking_lookup[256] =
{   // x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF
/*0x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*1x*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 
/*2x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*3x*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 
/*4x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*5x*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 
/*6x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*7x*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 
/*8x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*9x*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*Ax*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*Bx*/ 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 
/*Cx*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*Dx*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 
/*Ex*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
/*Fx*/ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 
};

//CZIDB_VN


// Opcode

uint8_t cpu_clock;
uint8_t opcode;
uint16_t addr;
uint8_t operand;
int8_t LSB_operand;
int8_t MSB_operand;
uint8_t instruction_len;

uint16_t cpu_addressing(uint8_t opcode);

void cpu_update_flags(uint8_t opcode);

#endif