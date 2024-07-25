#include <gtest/gtest.h>
#include "m6502.h"

class M6502AddWithCarryTests : public testing::Test
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

    void TestAbsolute( ADCTestData Test ) {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.A = Test.A;
        cpu.Flag.C = Test.Carry;
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.V = !Test.ExpectV;
        mem[0xFF00] = CPU::INS_ADC_ABS;
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

};

#define BYTE( A ) ( (m6502::Byte)A )

TEST_F( M6502AddWithCarryTests, ADCCanAddZeroWithZeroAndGetZero )
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
	TestAbsolute( Test );
}

TEST_F( M6502AddWithCarryTests, ADCCanAddCarryAndZeroToZeroAndGetZero )
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
	TestAbsolute( Test );
}

TEST_F( M6502AddWithCarryTests, ADCCanAddOneToFFAndItWillCauseACarry )
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
	TestAbsolute( Test );
}

TEST_F( M6502AddWithCarryTests, ADCWillSetTheNegativeWhenTheResultIsNegative )
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
	TestAbsolute( Test );
}

TEST_F( M6502AddWithCarryTests, ADCWillSetTheOverflowWhenSignedAdditionFails )
{
    // A: 10000000  -128
    // O: 11111111  -1
    // =: 01111111  127
    // C:1 N:0 V:1 
    ADCTestData Test;
    Test.Carry = false;
    Test.A = BYTE( -128) ;;              
    Test.Operand = BYTE(-1);           
    Test.Answer = 127;
    Test.ExpectC = true;
    Test.ExpectN = false;
    Test.ExpectV = true;
    Test.ExpectZ = false;
	TestAbsolute( Test );
}