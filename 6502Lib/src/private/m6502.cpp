#include "m6502.h"

m6502::s32 m6502::CPU::Execute(s32 Cycles, Mem &memory)
{

    /* Load a Register with the value from the memory address */
    auto LoadRegister = 
        [&Cycles, &memory, this]  
        ( Word Address, Byte& Register ) 
    {
        Register = ReadByte( Cycles, Address, memory );
        SetZeroAndNegativeFlags( Register );
    };

    /* And the A Register with the value from the memory address */
    auto And = 
        [&Cycles, &memory, this]  
        ( Word Address ) 
    {
        A &= ReadByte( Cycles, Address, memory );
        SetZeroAndNegativeFlags( A );
    };

    /* Or the A Register with the value from the memory address */
    auto Ora = 
        [&Cycles, &memory, this]  
        ( Word Address ) 
    {
        A |= ReadByte( Cycles, Address, memory );
        SetZeroAndNegativeFlags( A );
    };

    /* Eor the A Register with the value from the memory address */
    auto Eor = 
        [&Cycles, &memory, this]  
        ( Word Address ) 
    {
        A ^= ReadByte( Cycles, Address, memory );
        SetZeroAndNegativeFlags( A );
    };

    const s32 CyclesRequested = Cycles;
    while (Cycles > 0) {
        Byte Ins = FetchByte(Cycles, memory);
        switch ( Ins ) {
            case INS_AND_IM:
            {
                A &= FetchByte( Cycles, memory );
                SetZeroAndNegativeFlags( A );
            } break;
            case INS_ORA_IM:
            {
                A |= FetchByte( Cycles, memory );
                SetZeroAndNegativeFlags( A );
            } break;
            case INS_EOR_IM: 
            {
                A ^= FetchByte( Cycles, memory );
                SetZeroAndNegativeFlags( A );  
            } break;
            case INS_AND_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                Eor( Address );
            } break;
            case INS_AND_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                And( Address );
            } break;
            case INS_ORA_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                Ora( Address );
            } break;
            case INS_EOR_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                Eor( Address );
            } break;
            case INS_BIT_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Value =  ReadByte( Cycles, Address, memory );
                Flag.Z = !(A & Value);
                Flag.N = (Value & NegativeFlagBit) != 0;
                Flag.V = (Value & OverflowFlagBit) != 0;
            } break;
            case INS_BIT_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Value =  ReadByte( Cycles, Address, memory );
                Flag.Z = !(A & Value);
                Flag.N = (Value & NegativeFlagBit) != 0;
                Flag.V = (Value & OverflowFlagBit) != 0;
            } break;
            case INS_LDA_IM:
            {
                A = FetchByte(Cycles, memory);
                SetZeroAndNegativeFlags( A );
            } break;
            case INS_LDX_IM:
            {
                X = FetchByte(Cycles, memory);
                SetZeroAndNegativeFlags( X );
            } break;
            case INS_LDY_IM:
            {
                Y = FetchByte(Cycles, memory);
                SetZeroAndNegativeFlags( Y );
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
                SetZeroAndNegativeFlags( X );
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
            case INS_PHP:
            {
                PushByteOntoStack( Cycles, PS, memory );
            } break;
            case INS_PLA:
            {
                A = PopByteFromStack( Cycles, memory );       
                SetZeroAndNegativeFlags( A );         
            } break;
            case INS_PLP:
            {
                PS = PopByteFromStack( Cycles, memory );
            } break;
            case INS_TAX:
            {
                X = A;
                Cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case INS_TAY:
            {
                Y = A;
                Cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case INS_TXA:
            {
                A = X;
                Cycles--;
                SetZeroAndNegativeFlags( A );
            } break;
            case INS_TYA:
            {
                A = Y;
                Cycles--;
                SetZeroAndNegativeFlags( A );
            } break;
            case INS_INX:
            {
                X++;
                Cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case INS_INY:
            {
                Y++;
                Cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case INS_DEX:
            {
                X--;
                Cycles--;
                SetZeroAndNegativeFlags( X );
            } break;
            case INS_DEY: 
            {
                Y--;
                Cycles--;
                SetZeroAndNegativeFlags( Y );
            } break;
            case INS_DEC_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value--;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_DEC_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value--;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_DEC_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value--;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_DEC_ABSX: 
            {
                Word Address = AddressAbsoluteX_5( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value--;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_INC_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value++;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_INC_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value++;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_INC_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value++;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_INC_ABSX: 
            {
                Word Address = AddressAbsoluteX_5( Cycles, memory );
                Byte Value = ReadByte( Cycles, Address, memory );
                Value++;
                Cycles--;
                WriteByte( Value, Cycles, Address, memory );
                SetZeroAndNegativeFlags( Value );
            } break;
            case INS_BEQ: 
            {
                Byte Offset = FetchByte( Cycles, memory );
                if ( Flag.Z ) 
                {
                    const Word PCOld = PC;
                    PC += Offset;
                    Cycles--;

                    const bool PageChanged = ( PC >> 8) != (PCOld >> 8);
                    if ( PageChanged )
                    {
                        Cycles -= 2;
                    }
                }
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

m6502::Word m6502::CPU::LoadPrg( const Byte* Program, u32 NumBytes, Mem& memory ) const {
    Word LoadAddress = 0;
    if ( Program && NumBytes > 2 )
    {
        u32 At = 0;
        const Word Lo = Program[At++];
        const Word Hi = Program[At++] << 8;
        LoadAddress = Lo | Hi;
        for ( Word i = LoadAddress; i < LoadAddress+NumBytes-2; i++ )
        {
            // TODO : mem copy?
            memory[i] = Program[At++];
        }
    }

    return LoadAddress;
}

void m6502::CPU::PrintStatus() const {
    printf( "A: %d X: %d Y: %d\n", A, X, Y );
    printf( "PC: %d SP: %d\n", PC, SP);
    printf( "PS: %d\n", PS);
}