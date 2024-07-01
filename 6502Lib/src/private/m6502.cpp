#include "m6502.h"

m6502::Word m6502::CPU::AddressZeroPage( s32& Cycles, Mem& memory ) {
    Byte ZeroPaggeAddress = FetchByte( Cycles, memory );
    return ZeroPaggeAddress;
}

m6502::s32 m6502::CPU::Execute ( s32 Cycles, Mem& memory ) {

    const s32 CyclesRequested = Cycles;
    while (Cycles > 0) {
        Byte Ins = FetchByte(Cycles, memory);
        switch ( Ins ) {
            case INS_LDA_IM:
            {
                A = FetchByte(Cycles, memory);
                LoadRegisterSetStatus( A );
            } break;
            case INS_LDX_IM:
            {
                X = FetchByte(Cycles, memory);
                LoadRegisterSetStatus( X );
            } break;
            case INS_LDY_IM:
            {
                Y = FetchByte(Cycles, memory);
                LoadRegisterSetStatus( Y );
            } break;
            case INS_LDA_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                A = ReadByte( Cycles, Address, memory );
                LoadRegisterSetStatus( A );
            } break;
            case INS_LDX_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                X = ReadByte( Cycles, Address, memory );
                LoadRegisterSetStatus( X );
            } break;
            case INS_LDY_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Y = ReadByte( Cycles, Address, memory );
                LoadRegisterSetStatus( Y );
            } break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory); 
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte( Cycles, ZeroPageAddress, memory );
                LoadRegisterSetStatus( A );
            } break;
            case INS_LDA_ABS:
            {
                Word AbsAddress = FetchWord( Cycles, memory );
                A = ReadByte( Cycles, AbsAddress, memory );
                LoadRegisterSetStatus( A ); 
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
                LoadRegisterSetStatus( A ); 
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
                LoadRegisterSetStatus( A ); 
            } break;
            case INS_LDA_INDX:
            {
                Byte ZPAdress = FetchByte( Cycles, memory );
                ZPAdress += X;
                Cycles--;
                Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
                A = ReadByte( Cycles, EffectiveAddress, memory );
                LoadRegisterSetStatus( A ); 
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
                LoadRegisterSetStatus( A ); 
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