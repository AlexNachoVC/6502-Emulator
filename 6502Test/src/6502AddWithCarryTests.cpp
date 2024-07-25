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

};

TEST_F( M6502AddWithCarryTests, ADCCanAddZeroWithZeroAndGetZero )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
    cpu.A = 0x0;
    cpu.Flag.C = false;
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
	mem[0xFF00] = CPU::INS_ADC_ABS;
    mem[0xFF01] = 0x00;
    mem[0xFF02] = 0x80;
    mem[0x8000] = 0x00;
	constexpr s32 EXPECTED_CYCLES = 4;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.A, 0x0 );
	EXPECT_EQ( cpu.Flag.C, false );
	EXPECT_EQ( cpu.Flag.Z, true );
    EXPECT_EQ( cpu.Flag.N, false );
	EXPECT_EQ( cpu.Flag.V, false );
	EXPECT_EQ( cpu.SP, CPUCopy.SP );
    ExpectUnaffectedRegisters( CPUCopy );
}