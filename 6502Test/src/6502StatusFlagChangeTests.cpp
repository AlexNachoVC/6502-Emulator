#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502StatusFlagChangeTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

};


TEST_F( M6502StatusFlagChangeTests, CLWillClearTheCarryFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.C = true;
    mem[0xFF00] = CPU::INS_CLC;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.C );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.I, CPUCopy.Flag.I );
    EXPECT_EQ( cpu.Flag.D, CPUCopy.Flag.D );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

TEST_F( M6502StatusFlagChangeTests, SECWillSetTheCarryFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.C = false;
    mem[0xFF00] = CPU::INS_SEC;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flag.C );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.I, CPUCopy.Flag.I );
    EXPECT_EQ( cpu.Flag.D, CPUCopy.Flag.D );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

TEST_F( M6502StatusFlagChangeTests, CLDWillClearTheDecimalModeFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.D = true;
    mem[0xFF00] = CPU::INS_CLD;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.D );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.I, CPUCopy.Flag.I );
    EXPECT_EQ( cpu.Flag.C, CPUCopy.Flag.C );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

TEST_F( M6502StatusFlagChangeTests, SEDWillSetTheDecimalModeFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.D = false;
    mem[0xFF00] = CPU::INS_SED;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flag.D );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.I, CPUCopy.Flag.I );
    EXPECT_EQ( cpu.Flag.C, CPUCopy.Flag.C );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

TEST_F( M6502StatusFlagChangeTests, CLIWillClearTheInterruptDisableFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.I = true;
    mem[0xFF00] = CPU::INS_CLI;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.I );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.D, CPUCopy.Flag.D );
    EXPECT_EQ( cpu.Flag.C, CPUCopy.Flag.C );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

TEST_F( M6502StatusFlagChangeTests, SEIWillSetTheInterruptDisableFlag )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.I = false;
    mem[0xFF00] = CPU::INS_SEI;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_TRUE( cpu.Flag.I );
    EXPECT_EQ( cpu.Flag.Z, CPUCopy.Flag.Z );
    EXPECT_EQ( cpu.Flag.D, CPUCopy.Flag.D );
    EXPECT_EQ( cpu.Flag.C, CPUCopy.Flag.C );
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B );
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V );
    EXPECT_EQ( cpu.Flag.N, CPUCopy.Flag.N );
}

