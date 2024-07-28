#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502CompareRegistersTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    void ExpectUnaffectedRegisters( CPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flag.I, cpu.Flag.I );
        EXPECT_EQ( CPUBefore.Flag.D, cpu.Flag.D );
        EXPECT_EQ( CPUBefore.Flag.B, cpu.Flag.B );
        EXPECT_EQ( CPUBefore.Flag.V, cpu.Flag.V );
    }

    struct CMPTestData
    {
        Byte RegisterValue;
        Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    CMPTestData CompareTwoIdenticalValues() 
    {
        CMPTestData Test;
        Test.RegisterValue = 26;    
        Test.Operand = 26;
        Test.ExpectZ = true;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareTwoDifferentPositiveValues() 
    {
        CMPTestData Test;
        Test.RegisterValue = 48;    
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareANegativeNumberToAPositive()
    {
        CMPTestData Test;
        Test.RegisterValue = 130;    // Negative number!
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareTwoValuesThatResultInANegativeFlagSet() 
    {
        CMPTestData Test;
        Test.RegisterValue = 8;    
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = true;
        Test.ExpectC = false;
        return Test;
    }

    enum class ERegister
    {
        A, X, Y
    };

    void CompareImmediate ( CMPTestData Test, ERegister RegisterToCompare )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
        Byte* Register = &cpu.A;
        Byte Opcode = CPU::INS_CMP_IM;
        switch ( RegisterToCompare )
        {
            case ERegister::X:
                Register = &cpu.X;
                Opcode = CPU::INS_CPX_IM;
                break;
            case ERegister::Y:
                Register = &cpu.Y;
                Opcode = CPU::INS_CPY_IM;
                break;
        }
        *Register = Test.RegisterValue;

        mem[0xFF00] = Opcode;
        mem[0xFF01] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( *Register, Test.RegisterValue );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void CompareZeroPage ( CMPTestData Test, ERegister RegisterToCompare )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
        Byte* Register = &cpu.A;
        Byte Opcode = CPU::INS_CMP_ZP;
        switch ( RegisterToCompare )
        {
            case ERegister::X:
                Register = &cpu.X;
                Opcode = CPU::INS_CPX_ZP;
                break;
            case ERegister::Y:
                Register = &cpu.Y;
                Opcode = CPU::INS_CPY_ZP;
                break;
        }
        *Register = Test.RegisterValue;
        mem[0xFF00] = Opcode;
        mem[0xFF01] = 0x42;
        mem[0x0042] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ(*Register, Test.RegisterValue );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void CMPZeroPageX( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
		cpu.Flag.Z = !Test.ExpectZ;
		cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
		cpu.A = Test.RegisterValue;
		cpu.X = 4;
		mem[0xFF00] = CPU::INS_CMP_ZPX;
		mem[0xFF01] = 0x42;
		mem[0x0042+0x4] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CompareAbsolute( CMPTestData Test, ERegister RegisterToCompare )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
		cpu.Flag.C = !Test.ExpectC;
		Byte* Register = &cpu.A;
		Byte Opcode = CPU::INS_CMP_ABS;
		switch ( RegisterToCompare )
		{
		case ERegister::X:
			Register = &cpu.X;
			Opcode = CPU::INS_CPX_ABS;
			break;
		case ERegister::Y:
			Register = &cpu.Y;
			Opcode = CPU::INS_CPY_ABS;
			break;
		};
		*Register = Test.RegisterValue;

		mem[0xFF00] = Opcode;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CMPAbsoluteX( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
		cpu.Flag.Z = !Test.ExpectZ;
		cpu.Flag.N = !Test.ExpectN;
		cpu.Flag.C = !Test.ExpectC;
		cpu.A = Test.RegisterValue;
		cpu.X = 4;
		mem[0xFF00] = CPU::INS_CMP_ABSX;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000+4] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CMPAbsoluteY( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
		cpu.Flag.Z = !Test.ExpectZ;
		cpu.Flag.N = !Test.ExpectN;
		cpu.Flag.C = !Test.ExpectC;
		cpu.A = Test.RegisterValue;
		cpu.Y = 4;
		mem[0xFF00] = CPU::INS_CMP_ABSY;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000 + 4] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.Y, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CMPIndirectX( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
		cpu.Flag.C = !Test.ExpectC;
		cpu.Flag.Z = !Test.ExpectZ;
		cpu.Flag.N = !Test.ExpectN;
		cpu.A = Test.RegisterValue;
		cpu.X = 4;
		mem[0xFF00] = CPU::INS_CMP_INDX;
		mem[0xFF01] = 0x42;
		mem[0x42 + 4] = 0x00;
		mem[0x42 + 5] = 0x80;
		mem[0x8000] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 6;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CMPIndirectY( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
		cpu.Flag.C = !Test.ExpectC;
		cpu.Flag.Z = !Test.ExpectZ;
		cpu.Flag.N = !Test.ExpectN;
		cpu.A = Test.RegisterValue;
		cpu.Y = 4;
		mem[0xFF00] = CPU::INS_CMP_INDY;
		mem[0xFF01] = 0x42;
		mem[0x42] = 0x00;
		mem[0x43] = 0x80;
		mem[0x8000+4] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 5;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.RegisterValue );
		EXPECT_EQ( cpu.Y, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

};

// Compare
//-- Immediate

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CompareImmediate( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CompareImmediate( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CompareImmediate( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CompareImmediate( Test, ERegister::A );
}

//-- Zero Page

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CompareZeroPage( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CompareZeroPage( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageeCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CompareZeroPage( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CompareZeroPage( Test, ERegister::A );
}

//-- Zero Page X

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPZeroPageX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPZeroPageX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPZeroPageX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPZeroPageX( Test );
}

//-- Absolute

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareAbsolute( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CompareAbsolute( Test, ERegister::A );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareAbsolute( Test, ERegister::A);
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareAbsolute( Test, ERegister::A );
}

//-- Absolute X

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPAbsoluteX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanComparTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPAbsoluteX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPAbsoluteX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPAbsoluteX( Test );
}

//-- Absolute Y

TEST_F( M6502CompareRegistersTests, CMPAbsoluteYCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPAbsoluteY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteYCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPAbsoluteY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteYCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPAbsoluteY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteYCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPAbsoluteY( Test );
}

//-- Indirect X

TEST_F( M6502CompareRegistersTests, CMPIndirectXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectXCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectXCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPIndirectX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectXCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPIndirectX( Test );
}

//-- Indirect Y

TEST_F( M6502CompareRegistersTests, CMPIndirectYCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectYCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectYCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPIndirectY( Test );
}

TEST_F( M6502CompareRegistersTests, CMPIndirectYCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPIndirectY( Test );
}

// Compare X Register
//-- Immediate

TEST_F( M6502CompareRegistersTests, CPXImmediateCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXImmediateCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXImmediateCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CompareImmediate( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CompareImmediate( Test, ERegister::X );
}

//-- Zero Page

TEST_F( M6502CompareRegistersTests, CPXZeroPageCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXZeroPageCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXZeroPageCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareZeroPage( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareZeroPage( Test, ERegister::X );
}

//-- Absolute

TEST_F( M6502CompareRegistersTests, CPXAbsoluteCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXAbsoluteCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareAbsolute( Test, ERegister::X );
}

TEST_F( M6502CompareRegistersTests, CPXAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareAbsolute( Test, ERegister::X );
}

// Compare Y Register
//-- Immediate

TEST_F( M6502CompareRegistersTests, CPYImmediateCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYImmediateCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYImmediateCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CompareImmediate( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CompareImmediate( Test, ERegister::Y );
}

//-- Zero Page

TEST_F( M6502CompareRegistersTests, CPYZeroPageCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYZeroPageCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYZeroPageCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareZeroPage( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareZeroPage( Test, ERegister::Y );
}

//-- Absolute

TEST_F( M6502CompareRegistersTests, CPYAbsoluteCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYAbsoluteCanCompareTwoDifferentPositiveValues )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CompareAbsolute( Test, ERegister::Y );
}

TEST_F( M6502CompareRegistersTests, CPYAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CompareAbsolute( Test, ERegister::Y );
}