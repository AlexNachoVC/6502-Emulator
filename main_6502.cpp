#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;
struct Mem {
    
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
    void WriteWord( Word Value, u32 Address, u32& Cycles) {
        Data[Address]       = Value & 0xFF;
        Data[Address + 1]   = (Value >> 8);
        Cycles -= 2;
    }
};
struct CPU {

    Word PC;            // Program Counter
    Word SP;            // Stack Pointer

    Byte A, X, Y;       // Registers

    Byte C : 1;         // Status Flag                
    Byte Z : 1;         // Status Flag  
    Byte I : 1;         // Status Flag  
    Byte D : 1;         // Status Flag  
    Byte B : 1;         // Status Flag  
    Byte C : 1;         // Status Flag  
    Byte V : 1;         // Status Flag  
    Byte N : 1;         // Status Flag  

    void Reset( Mem& memory) {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialise();
    }

    Byte FetchByte( u32& Cycles, Mem& memory ) {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord( u32& Cycles, Mem& memory ) {
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

    Byte ReadByte( u32& Cycles, Byte Address, Mem& memory ){
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    // Opcodes
    static constexpr Byte 
        INS_LDA_IM = 0xA9,
        INS_LDA_ZP = 0XA5,
        INS_LDA_ZPX = 0XB5,
        INS_JSR = 0X20;

    void LDASetStatus() {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    void Execute ( u32 Cycles, Mem& memory ) {

        while (Cycles > 0) {
            Byte Ins = FetchByte(Cycles, memory);
            switch ( Ins ) {
                case INS_LDA_IM:
                {
                    Byte Value = FetchByte(Cycles, memory); 
                    A = Value;
                    LDASetStatus();
                } break;
                case INS_LDA_ZP:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory); 
                    A = ReadByte( Cycles, ZeroPageAddress, memory );
                    LDASetStatus();
                } break;
                case INS_LDA_ZPX:
                {
                    Byte ZeroPageAddress = FetchByte(Cycles, memory); 
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte( Cycles, ZeroPageAddress, memory );
                    LDASetStatus();
                } break;
                case INS_JSR:
                {
                    Word SubAddress = FetchWord( Cycles, memory );
                    memory.WriteWord( PC - 1, SP, Cycles );
                    PC = SubAddress;
                    Cycles--;
                } break;

                default:
                {
                    printf("Instruction not handled %d", Ins);
                } break;
            }
        }
    }
};

int main() {
    Mem mem;
    CPU cpu;
    cpu.Reset( mem );
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFDE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    // End - inline a little program
    cpu.Execute( 9, mem);
    return 0;
}