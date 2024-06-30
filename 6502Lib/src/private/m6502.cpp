#include "m6502.h"

m6502::s32 m6502::CPU::Execute ( s32 Cycles, Mem& memory ) {

    const s32 CyclesRequested = Cycles;
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
            case INS_LDA_ABS:
            {
                Word AbsAddress = FetchWord( Cycles, memory );
                A = ReadByte( Cycles, AbsAddress, memory );
            } break;
            case INS_LDA_ABSX:
            {
                Word AbsAddress = FetchWord( Cycles, memory );
                Word AbsAddressX = AbsAddress + X;
                A = ReadByte( Cycles, AbsAddressX, memory );
                if ( AbsAddressX - AbsAddress >= 0xFF ) 
                {
                    Cycles--;
                }
            } break;
            case INS_LDA_ABSY:
            {
                Word AbsAddress = FetchWord( Cycles, memory );
                Word AbsAddressY = AbsAddress + Y;
                A = ReadByte( Cycles, AbsAddressY, memory );
                if ( AbsAddressY - AbsAddress >= 0xFF ) 
                {
                    Cycles--;
                }
            } break;
            case INS_LDA_INDX:
            {
                Byte ZPAdress = FetchByte( Cycles, memory );
                ZPAdress += X;
                Cycles--;
                Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
                A = ReadByte( Cycles, EffectiveAddress, memory );
            } break;
            case INS_LDA_INDY:
            {
                Byte ZPAdress = FetchByte( Cycles, memory );
                Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
                Word EffectiveAddressY = EffectiveAddress + Y;
                A = ReadByte( Cycles, EffectiveAddressY, memory );
                if ( EffectiveAddressY - EffectiveAddress >= 0xFF ) 
                {
                    Cycles--;
                }
            } break;
            case INS_JSR:
            {
                Word SubAddress = FetchWord( Cycles, memory );
                memory.WriteWord( PC - 1, SP, Cycles );
                SP += 2;
                PC = SubAddress;
                Cycles--;
            } break;

            default:
            {
                printf("Instruction %d not handled\n", Ins);
                throw -1;
            } break;
        }
    }

    const s32 NumCyclesUsed = CyclesRequested - Cycles;
    return NumCyclesUsed;
}