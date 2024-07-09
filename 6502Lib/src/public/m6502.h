#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace m6502 
{
    using Byte = unsigned char;
    using Word = unsigned short;

    using u32 = unsigned int;
    using s32 = signed int;

    struct Mem;
    struct CPU;
    struct StatusFlags;
}


struct m6502::Mem {
    
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Initialise() {
        for (u32 i = 0; i < MAX_MEM; i++) {
            Data[i] = 0;
        }
    };

    /* Read 1 byte */
    Byte operator[] (u32 Address) const {

        // Assert here Addres is < MAX_MEM
        return Data[Address];
    }

    /* Write 1 byte */
    Byte& operator[] (u32 Address)  {
        
        // Assert here Addres is < MAX_MEM
        return Data[Address];
    }

};

struct m6502::StatusFlags {
    Byte C : 1;         // Status Flag                
    Byte Z : 1;         // Status Flag  
    Byte I : 1;         // Status Flag  
    Byte D : 1;         // Status Flag  
    Byte B : 1;         // Status Flag  
    Byte Unused : 1;    // Not used. Supposed to be logical 1 at all times
    Byte V : 1;         // Status Flag  
    Byte N : 1;         // Status Flag  
};
struct m6502::CPU {

    Word PC;            // Program Counter
    Byte SP;            // Stack Pointer

    Byte A, X, Y;       // Registers

    union // processor status
    {
        Byte PS;
        StatusFlags Flag;
    };

    void Reset( Mem& memory) {
        Reset( 0xFFFC, memory );
        
    }

    void Reset( Word ResetVector, Mem& memory) {
        PC = ResetVector;
        SP = 0xFF;
        Flag.C = Flag.Z = Flag.I = Flag.D = Flag.B = Flag.V = Flag.N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte( s32& Cycles, const Mem& memory ) {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord( s32& Cycles, const Mem& memory ) {
        // 6502 is little endian
        Word Data = memory[PC];
        PC++;
        
        Data |= (memory[PC] << 8);
        PC++;

        Cycles -=2;

        return Data;
    }

    Byte ReadByte( s32& Cycles, Word Address, const Mem& memory ){
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Word ReadWord( s32& Cycles, Word Address, const Mem& memory ){
        Byte LoByte = ReadByte( Cycles, Address, memory );
        Byte HiByte = ReadByte( Cycles, Address + 1, memory );
        return LoByte | (HiByte << 8);
    }
    
    /* Write 1 byte to memory */
    void WriteByte( Byte Value, s32& Cycles, Word Address, Mem& memory ) {
        memory[Address] = Value;
        Cycles--;
    }

    /* Write 2 bytes to memory */
    void WriteWord( Word Value, s32& Cycles, Word Address, Mem& memory ) {
        memory[Address]       = Value & 0xFF;
        memory[Address + 1]   = (Value >> 8);
        Cycles -= 2;
    }

    /* @return the stack pointer as a full 16-bit address (in the 1st page) */
    Word SPToAddress() const {
        return 0x100 | SP;
    }

    /* Push the PC-1 onto the stack */
    void PushPCToStack( s32& Cycles, Mem& memory ) {
        WriteWord( PC - 1, Cycles, SPToAddress() - 1, memory );
        SP -= 2;
    }

    void PushByteOntoStack( s32& Cycles, Byte Value, Mem& memory ) {
        Word SPWord = SPToAddress();
        memory[SPWord] = Value;
        Cycles--;
        SP--;
        Cycles--;
    }

    Word PopWordFromStack( s32& Cycles, Mem& memory ) {
        Word ValueFromStack = ReadWord( Cycles, SPToAddress() + 1, memory );
        SP += 2;
        Cycles--;
        
        return ValueFromStack;
    }

    Byte PopByteFromStack( s32& Cycles, Mem& memory ){
        Byte ValueFromStack = ReadByte( Cycles, SPToAddress() + 1, memory );
        Cycles -= 2;

        return ValueFromStack;
    }

    // Opcodes
    static constexpr Byte 
        // LDA
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0xA5,
        INS_LDA_ZPX = 0xB5,
        INS_LDA_ABS = 0xAD,
        INS_LDA_ABSX = 0xBD,
        INS_LDA_ABSY = 0xB9,
        INS_LDA_INDX = 0xA1,
        INS_LDA_INDY = 0xB1,
        // LDX
        INS_LDX_IM = 0xA2,
        INS_LDX_ZP = 0xA6,
        INS_LDX_ZPY = 0xB6,
        INS_LDX_ABS = 0xAE,
        INS_LDX_ABSY = 0xBE,
        // LDY
        INS_LDY_IM = 0xA0,
        INS_LDY_ZP = 0xA4,
        INS_LDY_ZPX = 0xB4,
        INS_LDY_ABS = 0xAC,
        INS_LDY_ABSX = 0xBC,
        // STA
        INS_STA_ZP = 0x85,
        INS_STA_ABS = 0x8D,
        INS_STA_ZPX = 0x95,
        INS_STA_ABSX = 0x9D,
        INS_STA_ABSY = 0x99,
        INS_STA_INDX = 0x81,
        INS_STA_INDY = 0x91,
        // STX
        INS_STX_ZP = 0x86,
        INS_STX_ABS = 0x8E,
        INS_STX_ZPY = 0x96,
        // STY 
        INS_STY_ZP = 0x84,
        INS_STY_ABS = 0x8C,
        INS_STY_ZPX = 0x94,
        //

        INS_TSX = 0xBA,
        INS_TXS = 0x9A,
        INS_PHA = 0x48,
        INS_PHP = 0x08,
        INS_PLA = 0x68,
        INS_PLP = 0x28,

        INS_JMP_ABS = 0x4C,
        INS_JMP_IND = 0x6C,
        INS_JSR = 0x20,
        INS_RTS = 0x60;

    /* Sets the correct Process status after a load register instruction
    *  - LDA, LDX, LDY
    *  @Register The A,X or Y Register */
    void LoadRegisterSetStatus( Byte Register ) {
        Flag.Z = (Register == 0);
        Flag.N = (Register & 0b10000000) > 0;
    }

    /* @return the number of cycles that were used */
    s32 Execute ( s32 Cycles, Mem& memory );
    
    /* Addresing mode - Zero Page */
    Word AddressZeroPage(s32 &Cycles, const Mem &memory);
    
    /* Addressing mode - Zero Page X*/
    Word AddressZeroPageX(s32 &Cycles, const Mem &memory);
    
    /* Addressing mode - Zero Page Y*/
    Word AddressZeroPageY(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Absolute*/
    Word AddressAbsolute(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Absolute with X offset*/
    Word AddressAbsoluteX(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Absolute with X offset 
    *  - (Always takes a cycle for the X page boundary)
    *  - See "STA Absolute, X" */
    Word AddressAbsoluteX_5(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Absolute with Y offset*/
    Word AddressAbsoluteY(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Absolute with Y offset
    *  - (Always takes a cycle for the Y page boundary)
    *  - See "STA Absolute, Y" */
    Word AddressAbsoluteY_5(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Indirect X | Indexed Indirect*/
    Word AddressIndirectX(s32 &Cycles, const Mem &memory);
    
    /* Addressing mode - Indirect Y | Indirect Indexed*/
    Word AddressIndirectY(s32 &Cycles, const Mem &memory);

    /* Addressing mode - Indirect Y | Indirect Indexed
    *  - (Always takes a cycle for the Y page boundary)
    *  - See "STA (Indirect, Y)" */
    Word AddressIndirectY_5(s32 &Cycles, const Mem &memory);
};