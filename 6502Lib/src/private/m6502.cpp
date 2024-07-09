#include "m6502.h"

m6502::s32 m6502::CPU::Execute ( s32 Cycles, Mem& memory ) {

    /* Load a Register with the value from the memory address */
    auto LoadRegister = 
        [&Cycles, &memory, this]  
        ( Word Address, Byte& Register ) 
    {
        Register = ReadByte( Cycles, Address, memory );
        LoadRegisterSetStatus( Register );
    };

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
                LoadRegister( Address, A );
            } break;
            case INS_LDX_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                LoadRegister( Address, X );
            } break;
            case INS_LDY_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                LoadRegister( Address, Y );
            } break;
            case INS_LDA_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_LDY_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                LoadRegister( Address, Y );
            } break;
            case INS_LDX_ZPY:
            {
                Word Address = AddressZeroPageY( Cycles, memory );
                LoadRegister( Address, X );
            } break;
            case INS_LDA_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_LDX_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                LoadRegister( Address, X );
            } break;
            case INS_LDY_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                LoadRegister( Address, Y );
            } break;
            case INS_LDA_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_LDY_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                LoadRegister( Address, Y );
            } break;
            case INS_LDA_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_LDX_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                LoadRegister( Address, X ); 
            } break;
            case INS_LDA_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_LDA_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                LoadRegister( Address, A );
            } break;
            case INS_STA_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STX_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                WriteByte( X, Cycles, Address, memory );
            } break;
            case INS_STY_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                WriteByte( Y, Cycles, Address, memory );
            } break;
            case INS_STA_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STX_ZPY:
            {
                Word Address = AddressZeroPageY( Cycles, memory );
                WriteByte( X, Cycles, Address, memory );
            } break;
            case INS_STY_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                WriteByte( Y, Cycles, Address, memory );
            } break;
            case INS_STA_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STX_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                WriteByte( X, Cycles, Address, memory );
            } break;
            case INS_STY_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                WriteByte( Y, Cycles, Address, memory );
            } break;
            case INS_STA_ABSX:
            {
                Word Address = AddressAbsoluteX_5( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STA_ABSY:
            {
                Word Address = AddressAbsoluteY_5( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STA_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_STA_INDY:
            {
                Word Address = AddressIndirectY_5( Cycles, memory );
                WriteByte( A, Cycles, Address, memory );
            } break;
            case INS_JSR:
            {
                Word SubAddress = FetchWord( Cycles, memory );
                PushPCToStack( Cycles, memory );
                PC = SubAddress;
                Cycles--;
            } break;
            case INS_RTS:
            {
                Word ReturnAddress = PopWordFromStack( Cycles, memory );
                PC = ReturnAddress + 1;
                Cycles -= 2;
            } break;
            //An original 6502 has does not correctly fetch the target 
            //address if the indirect vector falls on a page boundary
            //( e.g.$xxFF where xx is any value from $00 to $FF ).
            //In this case fetches the LSB from $xxFF as expected but 
            //takes the MSB from $xx00.This is fixed in some later chips 
            //like the 65SC02 so for compatibility always ensure the 
            //indirect vector is not at the end of the page.
            case INS_JMP_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                PC = Address;
            } break;
            case INS_JMP_IND:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Address = ReadWord( Cycles, Address, memory );
                PC = Address;
            } break;
            case INS_TSX:
            {
                X = SP;
                Cycles--;
                LoadRegisterSetStatus( X );
            } break;
            case INS_TXS:
            {
                SP = X;
                Cycles--;
            } break;
            case INS_PHA:
            {
                PushByteOntoStack( Cycles, A, memory );
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


m6502::Word m6502::CPU::AddressZeroPage( s32& Cycles, const Mem& memory ) {
    Byte ZeroPaggeAddress = FetchByte( Cycles, memory );
    return ZeroPaggeAddress;
}

m6502::Word m6502::CPU::AddressZeroPageX( s32& Cycles, const Mem& memory ) {
    Byte ZeroPageAddress = FetchByte(Cycles, memory); 
    ZeroPageAddress += X;
    Cycles--;
    return ZeroPageAddress;
}

m6502::Word m6502::CPU::AddressZeroPageY( s32& Cycles, const Mem& memory ) {
    Byte ZeroPageAddress = FetchByte(Cycles, memory); 
    ZeroPageAddress += Y;
    Cycles--;
    return ZeroPageAddress;
}

m6502::Word m6502::CPU::AddressAbsolute( s32& Cycles, const Mem& memory ) {
    Word AbsAddress = FetchWord( Cycles, memory );
    return AbsAddress;
}
m6502::Word m6502::CPU::AddressAbsoluteX( s32& Cycles, const Mem& memory ) {
    Word AbsAddress = FetchWord( Cycles, memory );
    Word AbsAddressX = AbsAddress + X;
    if ( AbsAddressX - AbsAddress >= 0xFF ) 
    {
        Cycles--;
    }
    return AbsAddressX;
}

m6502::Word m6502::CPU::AddressAbsoluteX_5( s32& Cycles, const Mem& memory ) {
    Word AbsAddress = FetchWord( Cycles, memory );
    Word AbsAddressX = AbsAddress + X;
    Cycles--;
    
    return AbsAddressX;
}

m6502::Word m6502::CPU::AddressAbsoluteY( s32& Cycles, const Mem& memory ) {
    Word AbsAddress = FetchWord( Cycles, memory );
    Word AbsAddressY = AbsAddress + Y;
    if ( AbsAddressY - AbsAddress >= 0xFF ) 
    {
        Cycles--;
    }
    return AbsAddressY;
}

m6502::Word m6502::CPU::AddressAbsoluteY_5( s32& Cycles, const Mem& memory ) {
    Word AbsAddress = FetchWord( Cycles, memory );
    Word AbsAddressY = AbsAddress + Y;
    Cycles--;

    return AbsAddressY;
}

m6502::Word m6502::CPU::AddressIndirectX( s32& Cycles, const Mem& memory ) {
    Byte ZPAdress = FetchByte( Cycles, memory );
    ZPAdress += X;
    Cycles--;
    Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
    return EffectiveAddress;
}

m6502::Word m6502::CPU::AddressIndirectY( s32& Cycles, const Mem& memory ) {
    Byte ZPAdress = FetchByte( Cycles, memory );
    Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
    Word EffectiveAddressY = EffectiveAddress + Y;
    if ( EffectiveAddressY - EffectiveAddress >= 0xFF ) 
    {
        Cycles--;
    }
    return EffectiveAddressY;
}

m6502::Word m6502::CPU::AddressIndirectY_5( s32& Cycles, const Mem& memory ) {
    Byte ZPAdress = FetchByte( Cycles, memory );
    Word EffectiveAddress = ReadWord( Cycles, ZPAdress, memory );
    Word EffectiveAddressY = EffectiveAddress + Y;
    Cycles--;

    return EffectiveAddressY;
}