#include "m6502.h"

#define ASSERT( Condition, Text ) { if ( !Condition ) { throw -1;}}

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

    /* Conditional Branch */
    auto BranchIf = [&Cycles, &memory, this] ( bool Test, bool Expected )
    {
        SByte Offset = FetchSByte( Cycles, memory );
        if ( Test == Expected ) 
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
    };

    /* Do add with carry given the operand */
    auto ADC = [&Cycles, &memory, this] ( Byte Operand )
    {
        ASSERT( Flag.D == false, "havent handled decimal mode!" );
        const bool AreSignBitsTheSame = !((A ^ Operand) & NegativeFlagBit);
        Word Sum = A;
        Sum += Operand;
        Sum += Flag.C;
        A = (Sum & 0xFF); 
        SetZeroAndNegativeFlags( A );
        Flag.C = Sum > 0xFF;
        Flag.V = AreSignBitsTheSame && ((A ^ Operand) & NegativeFlagBit);
    };

    /* Do subtract with carry given the operand */
    auto SBC = [&ADC] ( Byte Operand )
    {
        ADC( ~Operand );
    };

    /* Sets the processor status for a CMP/CPX/CPY instruction */
    auto RegisterCompare = [&Cycles, &memory, this] ( Byte Operand, Byte RegisterValue )
    {
        Byte Temp = RegisterValue - Operand;
        Flag.N = (Temp & NegativeFlagBit) > 0;
        Flag.Z = RegisterValue == Operand;
        Flag.C = RegisterValue >= Operand;
    };

    /* */
    auto ASL = [&Cycles, &memory, this] ( Byte Operand )
    {
        Flag.C = ( Operand & NegativeFlagBit ) > 0;
        Byte Result = Operand << 1;
        SetZeroAndNegativeFlags( Result );
        Cycles--;
        return Result;
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
                BranchIf( Flag.Z, true );
            } break;
            case INS_BNE: 
            {
                BranchIf( Flag.Z, false );
            } break;
            case INS_BCS:
            {
                BranchIf( Flag.C, true );
            } break;
            case INS_BCC:
            {
                BranchIf( Flag.C, false );
            } break;
            case INS_BMI:
            {
                BranchIf( Flag.N, true );
            } break;
            case INS_BPL:
            {
                BranchIf( Flag.N, false );
            } break;
            case INS_BVS:
            {
                BranchIf( Flag.V, true );
            } break;
            case INS_BVC:
            {
                BranchIf( Flag.V, false );
            } break;
            case INS_CLC: 
            {
                Flag.C = false;
                Cycles--;
            } break;
            case INS_SEC:
            {
                Flag.C = true;
                Cycles--;
            } break;
            case INS_CLD:
            {
                Flag.D = false;
                Cycles--;
            } break;
            case INS_SED:
            {
                Flag.D = true;
                Cycles--;
            } break;
            case INS_CLI:
            {
                Flag.I = false;
                Cycles--;
            } break;
            case INS_SEI:
            {
                Flag.I = true;
                Cycles--;
            } break;
            case INS_CLV: 
            {
                Flag.V = false;
                Cycles--;
            } break;
            case INS_NOP:
            {
                Cycles--;
            } break;
            case INS_ADC_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_IM:
            {
                Byte Operand = FetchByte( Cycles, memory );
                ADC( Operand );
            } break;
            case INS_ADC_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_ADC_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                ADC( Operand );
            } break;
            case INS_SBC_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                SBC( Operand );
            } break;
            case INS_CMP_IM:
            {
                Byte Operand = FetchByte( Cycles, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_ABSX:
            {
                Word Address = AddressAbsoluteX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_ABSY:
            {
                Word Address = AddressAbsoluteY( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_INDX:
            {
                Word Address = AddressIndirectX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
            } break;
            case INS_CMP_INDY:
            {
                Word Address = AddressIndirectY( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, A );
		    } break;
            case INS_CPX_IM:
            {
                Byte Operand = FetchByte( Cycles, memory );
                RegisterCompare( Operand, X );
            } break;
            case INS_CPX_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, X );
            } break;
            case INS_CPX_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, X );
            } break;
            case INS_CPY_IM:
            {
                Byte Operand = FetchByte( Cycles, memory );
                RegisterCompare( Operand, Y );
            } break;
            case INS_CPY_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, Y );
            } break;
            case INS_CPY_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                RegisterCompare( Operand, Y );
            } break;
            case INS_ASL:
            {
                A = ASL( A );
            } break;
            case INS_ASL_ZP:
            {
                Word Address = AddressZeroPage( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                Byte Result = ASL( Operand );
                WriteByte( Result, Cycles, Address, memory );
            } break;
            case INS_ASL_ZPX:
            {
                Word Address = AddressZeroPageX( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                Byte Result = ASL( Operand );
                WriteByte( Result, Cycles, Address, memory );
            } break;
            case INS_ASL_ABS:
            {
                Word Address = AddressAbsolute( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                Byte Result = ASL( Operand );
                WriteByte( Result, Cycles, Address, memory );
            } break;
            case INS_ASL_ABSX:
            {
                Word Address = AddressAbsoluteX_5( Cycles, memory );
                Byte Operand = ReadByte( Cycles, Address, memory );
                Byte Result = ASL( Operand );
                WriteByte( Result, Cycles, Address, memory );
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