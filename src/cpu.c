#include "cpu.h"

enum BIT_POS{
    BIT0,
    BIT1,
    BIT2,
    BIT3,
    BIT4,
    BIT5,
    BIT6,
    BIT7,
    BIT8,
};

// Macros úteis
#define concat_address(aa, bb) ((uint16_t)(aa) | (((uint16_t)(bb)) << 8))

#define bit_mask(bit_index) ((0x0001) << (bit_index))

#define nibble_mask(nib_index) ((0x000F) << ((nib_index) * (4)))

#define byte_mask(byt_index) ((0x00FF) << ((byt_index) * (8)))

#define bit_test(word, bit_index) ((word) & ((0x01) << (bit_index)))

#define bit_set(word, bit_index) ((word) | ((0x01) << (bit_index)))

#define bit_clear(word, bit_index) ((word) & ((~(0x01)) << (bit_index)))

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
// Verifica se a indexação ultrapassa o limíte da página

uint8_t cpu_check_page_breaks(uint8_t lsb_addr, uint8_t msb_addr, int8_t index){
    if(((concat_address(lsb_addr, msb_addr) + index) & 0xFF00) != (concat_address(lsb_addr, msb_addr) & 0xFF00)) 
        return 1;
    else
        return 0;
}

// Endereçamento de memória
// Incrementa o número de ciclos de clock de algumas instruções caso
// a indexação ultrapasse o limíte da página

int16_t cpu_addressing(uint8_t opcode, uint8_t cpu_clock_cycle){
    uint8_t addressing_mode = address_mode_lookup[opcode];
    uint8_t addr, msb_addr, lsb_addr;
    
    // Verifica se é uma instrução que tem um ciclo a mais de clock
    // quando a indexação ultrapassa o limite da página
    uint8_t check_page_breaks = page_breaking_lookup[opcode];

    switch (addressing_mode){
        case IMM:
            addr = cpu.pc++;
            return addr;
        case ZP:
            addr = cpu.pc++;
            return zero_page(addr, 0);
        case ZPX:
            addr = cpu.pc++;
            return zero_page(addr, cpu.reg[X]);
        case ZPY:
            addr = cpu.pc++;
            return zero_page(addr, cpu.reg[Y]);
        case ABS:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            return absolute(lsb_addr, msb_addr, 00);
        case ABX:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            if(check_page_breaks)
                cpu_clock_cycle += cpu_check_page_breaks(lsb_addr, msb_addr, cpu.reg[X]);
            return absolute(lsb_addr, msb_addr, cpu.reg[X]);
        case ABY:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            if(check_page_breaks)
                cpu_clock_cycle += cpu_check_page_breaks(lsb_addr, msb_addr, cpu.reg[Y]);            
            return absolute(lsb_addr, msb_addr, cpu.reg[Y]);
        case IND:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            return indirect(lsb_addr, msb_addr, 0, 0);
        case IZX:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            return indirect(lsb_addr, 0, cpu.reg[X], 0);
        case IZY:
            lsb_addr = cpu.pc++;
            msb_addr = cpu.pc++;
            if(check_page_breaks)
                cpu_clock_cycle += cpu_check_page_breaks(lsb_addr, msb_addr, cpu.reg[Y]);            
            return indirect(lsb_addr, 0, 0, cpu.reg[Y]);
        default:
            return 0;
    }
}

////////////////////////////////////////////
/// Funções para interpretação e execução de instruções
////////////////////////////////////////////

// Atualização de flags

void cpu_update_flags(uint8_t opcode, int8_t first_operand, int8_t second_operand, int16_t result){
    uint8_t update_flags = update_flags_lookup[opcode];

    if(bit_test(update_flags, N)){
        // Limpa a flag N
        bit_clear(cpu.reg[P], N);
        switch(opcode){
            // BIT - Bit Test
            case 0x24: case 0x2C:
                if(bit_test(second_operand, BIT7)) 
                    bit_set(cpu.reg[P], N);
                break;                        
            // SHR - Shift Right Logical    
            case 0x4A: case 0x46: case 0x56:  case 0x4E:  case 0x5E: 
                break;
            default:
                if(bit_test(result, BIT7)) 
                    bit_set(cpu.reg[P], N);
                break;        
        }
    }
    if(bit_test(update_flags, V)){
        // Limpa a flag V
        bit_clear(cpu.reg[P], V);
        switch(opcode){
            // CLV - Clear Overflow Flag
            case 0xB8:
                break;
            // BIT - Bit Test
            case 0x24: case 0x2C:
                if(bit_test(second_operand, BIT6))
                    bit_set(cpu.reg[P], V);
                break;
            default:
                if(((~(first_operand ^ second_operand)) & (first_operand ^ result)) & bit_mask(BIT7))
                    bit_set(cpu.reg[P], V);
                break;
        }
    }
    if(bit_test(update_flags, B)){
        // Limpa a flag B
        bit_clear(cpu.reg[P], B);        
        switch(opcode){
            // BRK - Force break
            case 0x00:
                bit_set(cpu.reg[P], B);
                break;
            default:
                break;
        }
    }
    if(bit_test(update_flags, D)){
        // Limpa a flag D
        bit_clear(cpu.reg[P], D);  
        switch(opcode){
            // CLD - Clear decimal mode
            case 0xD8:
                break;
            // STD - Set decimal mode    
            case 0xF8:
                bit_set(cpu.reg[P], D);     
                break;
            default:
                break;
        }
    }
    if(bit_test(update_flags, I)){
        // Limpa a flag I
        bit_clear(cpu.reg[P], I);
        switch(opcode){
            // CLI - Clear interrupt disable bit
            case 0x58:
                break;
            // STI - Set interrupt disable bit
            case 0x78:
                bit_set(cpu.reg[P], I);
                break;    
            default:
                break;
        }
    }
    if(bit_test(update_flags, Z)){
        // Limpa a flag Z
        bit_clear(cpu.reg[P], Z);        
        switch(opcode){
            // BIT - Bit Test
            case 0x24: case 0x2C:
                if((first_operand & second_operand) == 0x00)
                    bit_set(cpu.reg[P], Z);
                break;
            default:
                if(result == 0)
                    bit_set(cpu.reg[P], Z);
                break;
        }
    }
    if(bit_test(update_flags, C)){
        // Limpa a flag C
        bit_clear(cpu.reg[P], C);        
        switch(opcode){
            case 0x18:
            // STC - Set carry flag
                bit_set(cpu.reg[P], C);    
                break;
            // CLC - Clear carry flag    
            case 0x38:
                break;
            // ASL - Shift Left Logical/Arithmetic
            case 0x0A: case 0x06: case 0x16: case 0x0E: case 0x1E: 
            // ROL - Rotate Left through Carry
            case 0x2A: case 0x26: case 0x36: case 0x2E: case 0x3E:
                if(bit_test(result, BIT7))
                    bit_set(cpu.reg[P], C);
                break;
            // LSR - Shift Right Logical    
            case 0x4A: case 0x46: case 0x56: case 0x4E: case 0x5E:
            // ROR - Rotate Right through Carry
            case 0x6A: case 0x66: case 0x76: case 0x6E: case 0x7E:    
                if(bit_test(result, BIT0))
                    bit_set(cpu.reg[P], C);
                break;
            // SBC - Subtract memory from accumulator with borrow
            case 0xE9: case 0xE5: case 0xF5: case 0xED: case 0xFD: case 0xF9: case 0xE1: case 0xF1:
            // CMP - Compare
            case 0xC9: case 0xC5: case 0xD5: case 0xCD: case 0xDD: case 0xD9: case 0xC1: case 0xD1:
            case 0xE0: case 0xE4: case 0xEC: case 0xC0: case 0xC4: case 0xCC:
                if( first_operand >= second_operand)
                    bit_set(cpu.reg[P], C);
                break;
            default:
                // ADC - Add memory to accumulator with carry
                if(bit_test(result, BIT8))
                    bit_set(cpu.reg[P], C);
                break;
        }
    }
}



uint8_t cpu_fetch_decode_and_execute(){
    int8_t opcode, imm, data;
    uint16_t addr;
    uint8_t cpu_clock_cycles;

    opcode = cpu_read_memory(cpu.pc++);
    cpu_clock_cycles = clock_cycle_lookup[opcode];

    switch(opcode){
        /////////////////////////////////////////////////////////////
        /// Instruções de transferência entre memória e registradores
        /////////////////////////////////////////////////////////////

        // Registrador/Imediato para Registrador
        
        // TAY 
        case 0xA8: 
            cpu.reg[Y] = cpu.reg[A];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[A]);
            break;
        // TAX 
        case 0xAA: 
            cpu.reg[X] = cpu.reg[A];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[A]);
            break;
        // TSX 
        case 0xBA:
            cpu.reg[X] = cpu.reg[S];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[S]);            
            break;
        // TYA 
        case 0x98: 
            cpu.reg[A] = cpu.reg[Y];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[Y]);        
            break;
        // TXA
        case 0x8A: 
            cpu.reg[A] = cpu.reg[X];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[X]);        
            break;
        // TXS
        case 0x9A: 
            cpu.reg[S] = cpu.reg[X];
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[X]);
            break;
        // LDA
        case 0xA9: 
            cpu.reg[A] = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[A]);
            break;
        // LDX
        case 0xA2: 
            cpu.reg[X] = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[X]);
            break;
        // LDY
        case 0xA0:
            cpu.reg[Y] = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[Y]);
            break;

        // Carrega da memória para os registradores

        // LDA
        case 0xA5: case 0xB5: case 0xAD: case 0xBD: case 0xB9: case 0xA1: case 0xB1:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu.reg[A] = cpu_read_memory(addr);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[A]);
            break;
        // LDX
        case 0xA6: case 0xB6: case 0xAE: case 0xBE:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu.reg[X] =  cpu_read_memory(addr);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[X]);        
            break;
        // LDY
        case 0xA4: case 0xB4: case 0xAC: case 0xBC:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu.reg[Y] =  cpu_read_memory(addr);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[Y]);
            break;

        // Armazena o conteúdo do registrador na memória

        // STA
        case 0x85: case 0x95: case 0x8D: case 0x9D: case 0x99: case 0x81: case 0x91:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_write_memory(cpu.reg[A]);
            break;
        // STX
        case 0x86: case 0x96: case 0x8E:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_write_memory(cpu.reg[X]);
            break;        
        // STY    
        case 0x84: case 0x94: case 0x8C:
            addr = cpu_addressing(opcode, cpu_clock_cycles);
            cpu_write_memory(addr, cpu.reg[Y]);
            break;

        // Push/Pull

        // PHA
        case 0x48: 
            cpu_write_memory(cpu.reg[S] + 0x0100, cpu.reg[A]);
            cpu.reg[S]--;
            break;
        // PHP    
        case 0x08:
            cpu_write_memory(cpu.reg[S] + 0x0100, cpu.reg[P]);
            cpu.reg[S]--;
            break;
        // PLA 
        case 0x68:
            cpu.reg[S]++;
            cpu.reg[A] = cpu_read_memory(cpu.reg[S] + 0x0100);
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[A]);
            break;
        // PLP
        case 0x28:
            cpu.reg[S]++;
            cpu.reg[P] = cpu_read_memory(cpu.reg[S] + 0x0100)\
             | 0x20; // Protege as flags que não podem ser alteradas setando elas como "1" 
            cpu_update_flags(opcode, 0x00, 0x00, cpu.reg[P]);
            break;        
        default:
            break;
    }

    return cpu_clock_cycles;
}