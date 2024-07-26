#include <gtest/gtest.h>
#include "m6502.h"

class M6502SystemFunctionsTests : public testing::Test
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
};

TEST_F( M6502SystemFunctionsTests, NOPWillDoNothingButConsumeACycle )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_NOP;
	constexpr s32 EXPECTED_CYCLES = 2;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PS, CPUCopy.PS );
	EXPECT_EQ( cpu.PC, 0xFF01 );
	EXPECT_EQ( cpu.A, CPUCopy.A );
	EXPECT_EQ( cpu.X, CPUCopy.X );
	EXPECT_EQ( cpu.Y, CPUCopy.Y );
	EXPECT_EQ( cpu.SP, CPUCopy.SP );
}