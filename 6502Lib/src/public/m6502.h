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

    /* Write 2 bytes */
    void WriteWord( Word Value, u32 Address, s32& Cycles) {
        Data[Address]       = Value & 0xFF;
        Data[Address + 1]   = (Value >> 8);
        Cycles -= 2;
    }
};
struct m6502::CPU {

    Word PC;            // Program Counter
    Word SP;            // Stack Pointer

    Byte A, X, Y;       // Registers

    Byte C : 1;         // Status Flag                
    Byte Z : 1;         // Status Flag  
    Byte I : 1;         // Status Flag  
    Byte D : 1;         // Status Flag  
    Byte B : 1;         // Status Flag  
    Byte V : 1;         // Status Flag  
    Byte N : 1;         // Status Flag  

    void Reset( Mem& memory) {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
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

        // if you wanted to handle endianness
        // you would have to swap bytes here
        // if ( PLATFORM_BIG_ENDIAN )
        //    SwapBytesInWord(Data) 

        return Data;
    }

    Byte ReadByte( s32& Cycles, Word Address, Mem& memory ){
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Word ReadWord( s32& Cycles, Word Address, Mem& memory ){
        Byte LoByte = ReadByte( Cycles, Address, memory );
        Byte HiByte = ReadByte( Cycles, Address + 1, memory );
        return LoByte | (HiByte << 8);
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
        INS_JSR = 0x20;

    /* Sets the correct Process status after a load register instruction
    *  - LDA, LDX, LDY
    *  @Register The A,X or Y Register */
    void LoadRegisterSetStatus( Byte Register ) {
        Z = (Register == 0);
        N = (Register & 0b10000000) > 0;
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

    /* Addressing mode - Absolute with Y offset*/
    Word AddressAbsoluteY(s32 &Cycles, const Mem &memory);
};