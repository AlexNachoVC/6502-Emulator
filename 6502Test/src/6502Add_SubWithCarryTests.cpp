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
        Add, Substract
    };

    void TestSBCAbsolute(ADCTestData Test )
    {
        TestADCAbsolute( Test, EOperation::Substract );
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

    void TestADCImmediate( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_IM;
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

    void TestADCZeroPage( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_ZP;
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

    void TestADCZeroPageX( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_ZPX;
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

    void TestADCAbsoluteX( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_ABSX;
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

    void TestADCAbsoluteY( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Y = 0x10;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_ABSY;
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

    void TestADCIndirectX( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.X = 0x04;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_INDX;
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

    void TestADCIndirectY( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Y = 0x04;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_INDY;
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

TEST_F( M6502Add_SubWithCarryTests, SBCAbsCanSubstracOneFromZeroAndGetZero )
{
    ADCTestData Test;
    Test.Carry = true;
    Test.A = 0;
    Test.Operand = 1;
    Test.Answer = BYTE(-1);    
    Test.ExpectC = false;
    Test.ExpectN = true;
    Test.ExpectV = false;
    Test.ExpectZ = false;
	TestSBCAbsolute( Test );
}

