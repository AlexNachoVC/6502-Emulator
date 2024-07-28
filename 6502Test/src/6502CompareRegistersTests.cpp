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
        Byte A;
        Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    CMPTestData CompareTwoIdenticalValues() 
    {
        CMPTestData Test;
        Test.A = 26;    
        Test.Operand = 26;
        Test.ExpectZ = true;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareTwoDifferentPositiveValues() 
    {
        CMPTestData Test;
        Test.A = 48;    
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareANegativeNumberToAPositive()
    {
        CMPTestData Test;
        Test.A = 130;    // Negative number!
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = false;
        Test.ExpectC = true;
        return Test;
    }

    CMPTestData CompareTwoValuesThatResultInANegativeFlagSet() 
    {
        CMPTestData Test;
        Test.A = 8;    
        Test.Operand = 26;
        Test.ExpectZ = false;
        Test.ExpectN = true;
        Test.ExpectC = false;
        return Test;
    }

    void CMPImmediate (CMPTestData Test )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
        cpu.A = Test.A;
        mem[0xFF00] = CPU::INS_CMP_IM;
        mem[0xFF01] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void CMPZeroPage (CMPTestData Test )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
        cpu.A = Test.A;
        mem[0xFF00] = CPU::INS_CMP_ZP;
        mem[0xFF01] = 0x42;
        mem[0x0042] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
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
		cpu.A = Test.A;
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
		EXPECT_EQ( cpu.A, Test.A );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

    void CMPAbsolute( CMPTestData Test )
	{
		// given:
		using namespace m6502;
		cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
		cpu.Flag.C = !Test.ExpectC;
		cpu.A = Test.A;
		mem[0xFF00] = CPU::INS_CMP_ABS;
		mem[0xFF01] = 0x00;
		mem[0xFF02] = 0x80;
		mem[0x8000] = Test.Operand;
		constexpr s32 EXPECTED_CYCLES = 4;
		CPU CPUCopy = cpu;

		// when:
		const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

		// then:
		EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
		EXPECT_EQ( cpu.A, Test.A );
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
		cpu.A = Test.A;
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
		EXPECT_EQ( cpu.A, Test.A );
		EXPECT_EQ( cpu.X, 4 );
		EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
		EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
		EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
		ExpectUnaffectedRegisters( CPUCopy );
	}

};

//-- Immediate

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CMPImmediate( Test );
}

//-- Zero Page

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoIdenticalValues )
{
    CMPTestData Test = CompareTwoIdenticalValues();
    CMPZeroPage( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test = CompareTwoDifferentPositiveValues();
    CMPZeroPage( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageeCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test = CompareANegativeNumberToAPositive();
    CMPZeroPage( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
    CMPZeroPage( Test );
}

//-- Zero Page X

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPZeroPageX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPZeroPageXCanCompareALargePositiveToASmallPositive )
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
	CMPAbsolute( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareALargePositiveToASmallPositive )
{
	CMPTestData Test = CompareTwoDifferentPositiveValues();
	CMPAbsolute( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareANegativeNumberToAPositive )
{
	CMPTestData Test = CompareANegativeNumberToAPositive();
	CMPAbsolute( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteCanCompareTwoValuesThatResultInANegativeFlagSet )
{
	CMPTestData Test = CompareTwoValuesThatResultInANegativeFlagSet();
	CMPAbsolute( Test );
}

//-- Absolute X

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanCompareTwoIdenticalValues )
{
	CMPTestData Test = CompareTwoIdenticalValues();
	CMPAbsoluteX( Test );
}

TEST_F( M6502CompareRegistersTests, CMPAbsoluteXCanCompareALargePositiveToASmallPositive )
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
