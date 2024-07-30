#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502ShiftsTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

};



TEST_F( M6502ShiftsTests, ASLCanShiftTheValueOfOne )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.C = true;
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
    cpu.A = 1;
    mem[0xFF00] = CPU::INS_ASL;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 2 );
    EXPECT_FALSE( cpu.Flag.C );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
}

TEST_F( M6502ShiftsTests, ASLCanShiftANegativeValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.C = false;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    cpu.A = 0b11000010;
    mem[0xFF00] = CPU::INS_ASL;
    constexpr s32 EXPECTED_CYCLES = 2;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10000100 );
    EXPECT_TRUE( cpu.Flag.C );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
}