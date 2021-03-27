#include "cpu.h"

int8_t* mapper_0(uint16_t logical_address){
    int8_t* physical_address;
    // CPU ROM Address
    if(logical_address >= 0x4020){
        // SRAM
        if (logical_address < 0x6000){
            physical_address = &cartr.SRAM[logical_address - 0x4020];
        }
        // Expansion ROM
        else if(logical_address >= 0x6000 && logical_address < 0x8000){
            physical_address = &cartr.EXP_ROM[logical_address - 0x6000];
        }
        // PRG-ROM Lower Bank
        else if(logical_address >= 0x8000 && logical_address < 0xC000){
            physical_address = &cartr.PRG_ROM[logical_address - 0x8000];
        }
        // PRG-ROM Upper Bank
        else{
            physical_address = &cartr.PRG_ROM[logical_address - 0xC000];
        }
    }
    // PPU ROM Address
    else if(logical_address >= 0x0000 && logical_address < 0x2000){
        if(cartr.N_CHR_ROM_PAG != 0){
            physical_address = &cartr.CHR_ROM[logical_address - 0x0000];
        }
        else{
            // Usa a VRAM da PPU
        }
    }

    return physical_address; 
}

int8_t* cpu_memory_mapper(uint16_t logical_address){
    int8_t *physical_address;
    // Memória interna RAM
    if(logical_address < 0x2000){
        physical_address = &RAM[logical_address];
    }
    // I/O Registers
    else if(logical_address >= 0x2000 && logical_address < 0x4020){
        physical_address = &RAM[logical_address - 0x2000];
    }
    // Memória ROM do cartucho
    else if(logical_address >= 0x4020){
        // Utiliza o mapeador correto para o cartucho
        physical_address = (*mapper)(logical_address);
    }
    return physical_address;
}