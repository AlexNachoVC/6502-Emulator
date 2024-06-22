#include <stdio.h>
#include <stdlib.h>


struct CPU {
    using Byte = unsigned char;
    using Word = unsigned short;

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

    void Reset() {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
    }
};

int main() {
    CPU cpu;
    cpu.Reset();
    return 0;
}