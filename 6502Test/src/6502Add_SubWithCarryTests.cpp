#include <gtest/gtest.h>
#include "m6502.h"

class M6502Add_SubWithCarryTests : public testing::Test
{
public:
	m6502::Mem mem;
	m6502::CPU cpu;

	virtual void SetUp()
	{
		cpu.Reset( mem );
	}

	virtual void TearDown()
	{
	}

    void ExpectUnaffectedRegisters( m6502::CPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flag.I, cpu.Flag.I );
        EXPECT_EQ( CPUBefore.Flag.D, cpu.Flag.D );
        EXPECT_EQ( CPUBefore.Flag.B, cpu.Flag.B );
    }

    struct ADCTestData 
    {
        bool Carry;
        m6502::Byte A;
        m6502::Byte Operand;
        m6502::Byte Answer;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
        bool ExpectV;
    };

    enum class EOperation
    {
        Add, Subtract
    };

    void TestSBCAbsolute(ADCTestData Test )
    {
        TestADCAbsolute( Test, EOperation::Subtract );
    }

    void TestADCAbsolute( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = ( Operation == EOperation::Add) ? CPU::INS_ADC_ABS : CPU::INS_SBC_ABS;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCImmediate( ADCTestData Test )
	{
		TestADCImmediate( Test, EOperation::Subtract );
	}

    void TestADCImmediate( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_IM  : CPU::INS_SBC_IM;
        mem[0xFF01] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCZeroPage( ADCTestData Test )
	{
		TestADCZeroPage( Test, EOperation::Subtract );
	}

    void TestADCZeroPage( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = 
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_ZP : CPU::INS_SBC_ZP;
        mem[0xFF01] = 0x42;
        mem[0x0042] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCZeroPageX( ADCTestData Test )
	{
		TestADCZeroPageX( Test, EOperation::Subtract );
	}

    void TestADCZeroPageX( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_ZPX : CPU::INS_SBC_ZPX;
        mem[0xFF01] = 0x42;
        mem[0x0042+0x10] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCAbsoluteX( ADCTestData Test )
	{
		TestADCAbsoluteX( Test, EOperation::Subtract );
	}

    void TestADCAbsoluteX( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_ABSX : CPU::INS_SBC_ABSX;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000+0x10] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCAbsoluteY( ADCTestData Test )
	{
		TestADCAbsoluteY( Test, EOperation::Subtract );
	}

    void TestADCAbsoluteY( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Y = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_ABSY : CPU::INS_SBC_ABSY;
        mem[0xFF01] = 0x00;
        mem[0xFF02] = 0x80;
        mem[0x8000+0x10] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCIndirectX( ADCTestData Test )
	{
		TestADCIndirectX( Test, EOperation::Subtract );
	}

    void TestADCIndirectX( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x04;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_INDX : CPU::INS_SBC_INDX;
        mem[0xFF01] = 0x02;
        mem[0x0006] = 0x00; //0x2 + 0x4
        mem[0x0007] = 0x80;
        mem[0x8000] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 6;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }

    void TestSBCIndirectY( ADCTestData Test )
	{
		TestADCIndirectY( Test, EOperation::Subtract );
	}

    void TestADCIndirectY( ADCTestData Test, EOperation Operation = EOperation::Add ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Y = 0x04;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] =
			(Operation == EOperation::Add) ?
			CPU::INS_ADC_INDY : CPU::INS_SBC_INDY;
        mem[0xFF01] = 0x02;
        mem[0x0002] = 0x00; 
        mem[0x0003] = 0x80;
        mem[0x8000 + 0x04] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 5;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.Answer );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.V, Test.ExpectV );
        ExpectUnaffectedRegisters( CPUCopy );
    }
};

#define BYTE( A ) ( (m6502::Byte)A )

TEST_F( M6502Add_SubWithCarryTests, ADCAbsCanAddZeroWithZeroAndGetZero )
{
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 0;
    Test.Operand = 0;
    Test.Answer = 0;    
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsCanAddCarryAndZeroToZeroAndGeTOne )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 0;
    Test.Operand = 0;
    Test.Answer = 1;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsCanAddOneToFFAndItWillCauseACarry )
{
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 0xFF;              
    Test.Operand = 1;           
    Test.Answer = 0;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsWillSetTheNegativeWhenTheResultIsNegative )
{
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 0;              
    Test.Operand = BYTE(-1);           
    Test.Answer = BYTE(-1);
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsWillSetTheOverflowWhenSignedNegativeAdditionFails )
{
    // A: 10000000  -128
    // O: 11111111  -1
    // =: 01111111  127
    // C:1 N:0 V:1 Z:0
    ADCTestData Test;
    Test.Carry = false;
    Test.A = BYTE( -128);              
    Test.Operand = BYTE(-1);           
    Test.Answer = 127;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = true;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsWillSetTheOverflowWhenSignedNegativeAdditionPassesDueToInitialCarryFlag )
{
    // C: 00000001  
    // A: 10000000  -128
    // O: 11111111  -1
    // =: 10000000  -128
    // C:1 N:1 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = BYTE( -128);              
    Test.Operand = BYTE(-1);           
    Test.Answer = BYTE(-128);
    Test.ExpectC = true;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsWillSetTheOverflowWhenSignedPositiveAdditionFails )
{
    // A: 01111111  127
    // O: 00000001  1
    // =: 10000000  128  
    // C:0 N:1 V:1 Z:0
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 127;              
    Test.Operand = 1;           
    Test.Answer = 128;
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = true;
    Test.ExpectZ = false;
	TestADCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCImmediateCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCImmediateCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCZeroPageCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCZeroPageCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCZeroPageXCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCZeroPageXCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsoluteXCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsoluteXCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsoluteYCanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCAbsoluteYCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCIndirectXanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCIndirectXCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCIndirectYanAddTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 38;
    Test.ExpectC = false;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, ADCIndirectYCanAddAPositiveAndNegativedNumber )
{
    // A: 00010100 20
    // O: 11101111 -17
    // =: 00000011
    // C:1 N:0 V:0 Z:0
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = BYTE(-17);
    Test.Answer = 4;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestADCIndirectY( Test );
}

// SBC Immediate ---------

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCImmediateCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCImmediate( Test );
}

// SBC Zero Page

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPage( Test );
}

// SBC Zero Page, X -------

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCZeroPageXCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCZeroPageX( Test );
}


// SBC Absolute

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractZeroFromZeroAndGetZero )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 0;
    Test.Operand = 0;
    Test.Answer = 0;    
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = true;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractOneFromZeroAndGetMinusOne )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 0;
    Test.Operand = 1;
    Test.Answer = BYTE( -1 );    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractOneFromZeroWithCarryAndGetMinusTwo )
{
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 0;
    Test.Operand = 1;
    Test.Answer = BYTE( -2 );    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractTwoNegativeNumbersAndGetSignedOverflow )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = BYTE(-128);
    Test.Operand = 1;
    Test.Answer = 127;    
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = true;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractAPositiveAndANegativeNumbersAndGetSignedOverflow )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 127;
    Test.Operand = BYTE(-1);
    Test.Answer = 128;    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = true;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractZeroFromZeroAndCarryAndGetMinusOne )
{
    ADCTestData Test;
    Test.Carry = false;
    Test.A = 0;
    Test.Operand = 0;
    Test.Answer = BYTE(-1);    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractTwoUnsignedNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 20;
    Test.Operand = 17;
    Test.Answer = 3;    
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstractTwoNegativeNumbers )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = BYTE(-20);
    Test.Operand = BYTE(-17);
    Test.Answer = BYTE(-3);    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

// SBC Absolute, X -------

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteXCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteX( Test );
}

// SBC Absolute, Y

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCAbsoluteYCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCAbsoluteY( Test );
}

// SBC Indirect, X ---------

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectXCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectX( Test );
}

// SBC Indirect, Y -----------

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractZeroFromZeroAndGetZero )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = 0;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = true;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractZeroFromZeroAndCarryAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 0;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractOneFromZeroAndGetMinusOne )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -1 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractOneFromZeroWithCarryAndGetMinusTwo )
{
	ADCTestData Test;
	Test.Carry = false;
	Test.A = 0;
	Test.Operand = 1;
	Test.Answer = BYTE( -2 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractTwoNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -128 );
	Test.Operand = 1;
	Test.Answer = 127;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractAPostitiveAndNegativeNumbersAndGetSignedOverflow )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 127;
	Test.Operand = BYTE( -1 );
	Test.Answer = 128;
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = true;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractTwoUnsignedNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = 20;
	Test.Operand = 17;
	Test.Answer = 3;
	Test.ExpectC = true;
	Test.ExpectN = false;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}

TEST_F( M6502Add_SubWithCarryTests, SBCIndirectYCanSubtractTwoNegativeNumbers )
{
	ADCTestData Test;
	Test.Carry = true;
	Test.A = BYTE( -20 );
	Test.Operand = BYTE( -17 );
	Test.Answer = BYTE( -3 );
	Test.ExpectC = false;
	Test.ExpectN = true;
	Test.ExpectV = false;
	Test.ExpectZ = false;
	TestSBCIndirectY( Test );
}