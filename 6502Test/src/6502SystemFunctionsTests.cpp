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

TEST_F( M6502SystemFunctionsTests, BRKWillLoadTheProgramCounterFromTheInterruptVector )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x80;
	constexpr s32 EXPECTED_CYCLES = 7;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0x8000 );
}


TEST_F( M6502SystemFunctionsTests, BRKWillLoadTheProgramCounterFromTheInterruptVector2 )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x90;
	constexpr s32 EXPECTED_CYCLES = 7;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.PC, 0x9000 );
}

TEST_F( M6502SystemFunctionsTests, BRKWillSetTheBreakFlag )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	cpu.Flag.B = false;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x90;
	constexpr s32 EXPECTED_CYCLES = 7;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_TRUE( cpu.Flag.B );
}

TEST_F( M6502SystemFunctionsTests, BRKWillPush3BytesOntoTheStack )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	constexpr s32 EXPECTED_CYCLES = 7;
	CPU CPUCopy = cpu;

	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( cpu.SP, CPUCopy.SP - 3 );
}

TEST_F( M6502SystemFunctionsTests, BRKWillPushPCAndPSOntoTheStack )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	constexpr s32 EXPECTED_CYCLES = 7;
	CPU CPUCopy = cpu;
	Byte OldSP = CPUCopy.SP;
	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( mem[(0x100 | OldSP) -0], 0xFF );
	EXPECT_EQ( mem[(0x100 | OldSP) -1], 0x01 );
	EXPECT_EQ( mem[(0x100 | OldSP) -2], CPUCopy.PS  );
}

TEST_F( M6502SystemFunctionsTests, RTICanReturnFromAnInterruptLeavingTheCPUInTheStateWhenItEntered )
{
	// given:
	using namespace m6502;
	cpu.Reset( 0xFF00, mem );
	mem[0xFF00] = CPU::INS_BRK;
	mem[0xFFFE] = 0x00;
	mem[0xFFFF] = 0x80;
	mem[0x8000] = CPU::INS_RTI;
	constexpr s32 EXPECTED_CYCLES = 7 + 6;
	CPU CPUCopy = cpu;
	Byte OldSP = CPUCopy.SP;
	// when:
	const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

	// then:
	EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
	EXPECT_EQ( CPUCopy.SP, cpu.SP );
	EXPECT_EQ( CPUCopy.PC, cpu.PC );
	EXPECT_EQ( CPUCopy.PS, cpu.PS );
}