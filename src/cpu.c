#include "cpu.h"

// Macros
#define concat_address(aa, bb) ((uint16_t)(aa) | (((uint16_t)(bb)) << 8))

////////////////////////////////////////////
/// Macros de modos de endereçamento
////////////////////////////////////////////

// Zero Page         - zero_page(aa, 0);
// Zero Page Indexed - zero_page(aa, index);
#define zero_page(aa, index) ((uint16_t)(((uint16_t)(aa) + (uint16_t)(index)) % 0x100))

// Absolute         - absolute(aa, bb, 0);
// Absolute Indexed - absolute(aa, bb, index);
#define absolute(aa, bb, index) ((uint16_t)( concat_address(aa, bb) + index))

// Indirect         - indirect(aa, bb, 0, 0);
// Indexed Indirect - indirect(aa, 0, index1, 0);
// Indirect Indexed - indirect(aa, 0, 0, index2);
#define indirect(aa, bb, index1, index2) (concat_address(\
cpu_read_memory(concat_address((aa), (bb)) + (index1)), \
cpu_read_memory(concat_address((aa), (bb)) + (index1) + 1)) + (index2)) 


////////////////////////////////////////////
/// Funções de mapeamento e acesso a memória
////////////////////////////////////////////

int8_t* mapper(uint16_t logical_address){
    int8_t* physical_address;

    // Memória do cartucho acessada pela CPU
    if(logical_address >= CPU_CART_MEM){
        // Expansion ROM
        if(logical_address < SRAM){
            physical_address = &cartridge.EX_ROM[logical_address - EX_ROM];
        }
        // SRAM
        else if((logical_address >= SRAM) && (logical_address < PRG_ROM_LB)){
            physical_address = &cartridge.SRAM[logical_address - SRAM];
        }
        // 32k PRG-ROM
        // PRG-ROM Lower Bank e Upper Bank (PRG_ROM_LB <= logical_address < 0x10000)
        else{
            physical_address = &cartridge.PRG_ROM[logical_address - PRG_ROM_LB];
        }
    }
    // Memória do cartucho acessada pela PPU
    else if(logical_address < 0x2000){
        if(cartridge.N_CHR_ROM_PAG != 0){
            physical_address = &cartridge.CHR_ROM[logical_address - 0x0000];
        }
        else{
            // Usa a VRAM da PPU
        }
    }

    return physical_address; 

}

int8_t* cpu_memory_mapper(uint16_t logical_address){
    int8_t *physical_address;

    // Memória RAM interna
    if(logical_address < IO_REG){
        physical_address = &cpu_ram[logical_address - RAM];
    }
    // I/O Registers
    else if((logical_address >= IO_REG) && (logical_address < 0x4020)){
        physical_address = &io_reg[logical_address - IO_REG];
    }
    // Memória ROM/RAM do cartucho (0x4020 <= logical_address < 0x10000)
    else{
        // Utiliza o mapeador configurado para o cartucho
        physical_address = mapper(logical_address);
    }

    return physical_address;

}

void cpu_write_memory(uint16_t logical_address, int8_t data){
    int8_t* physical_address;

    physical_address = cpu_memory_mapper(logical_address);
    *physical_address = data;
    
    return;

}

int8_t cpu_read_memory(uint16_t logical_address){
    int8_t* physical_address;
    int8_t data;

    physical_address = cpu_memory_mapper(logical_address);
    data = *physical_address;

    return data;

}

////////////////////////////////////////////
/// 
////////////////////////////////////////////

uint8_t cpu_fetch_and_execute(){

    switch(opcode){
        case 0x00:
            break;
        default:
            break;
    }

    return cpu_clock;
}