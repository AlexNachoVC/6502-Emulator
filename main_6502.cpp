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

    void Execute ( u32 Cycles, Mem& memory ) {

        while (Cycles > 0) {
            Byte Ins = FetchByte(Cycles, memory);
        }
    }
};

int main() {
    Mem mem;
    CPU cpu;
    cpu.Reset( mem );
    cpu.Execute( 2, mem);
    return 0;
}