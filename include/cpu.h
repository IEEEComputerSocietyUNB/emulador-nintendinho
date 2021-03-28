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

int8_t* mapper_0(uint16_t logical_address);

// Referência para o mapeador utilizado no cartucho

typedef int8_t* Mapper(uint16_t logical_address);

Mapper* mapper = mapper_0;

// Mapeia os endereços acessados pela CPU para a estrutura correta de memória

int8_t* cpu_memory_mapper(uint16_t logical_address);

// A CPU escreve na memória

void cpu_write_memory(uint16_t logical_address, int8_t data);

// A CPU lê da memória

int8_t cpu_read_memory(uint16_t logical_address);



#endif