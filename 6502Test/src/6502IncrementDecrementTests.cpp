#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502IncrementDecrementTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    void ExpectUnaffectedRegisters( CPU CPUBefore ) {
        EXPECT_EQ( CPUBefore.Flag.C, cpu.Flag.C);
        EXPECT_EQ( CPUBefore.Flag.I, cpu.Flag.I);
        EXPECT_EQ( CPUBefore.Flag.D, cpu.Flag.D);
        EXPECT_EQ( CPUBefore.Flag.B, cpu.Flag.B);
        EXPECT_EQ( CPUBefore.Flag.V, cpu.Flag.V);
    }
};

TEST_F( M6502IncrementDecrementTests, INXCanIncrementAZeroValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0x0;
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
    mem[0xFF00] = CPU::INS_INX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x01 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedRegisters( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INXCanIncrement255 )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0xFF;
    cpu.Flag.Z = false;
    cpu.Flag.N = true;
    mem[0xFF00] = CPU::INS_INX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x0 );    // NOTE: does this instruction actualy wrap?
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedRegisters( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INXCanIncrementANegativeValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0b10001000;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_INX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0b10001001 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedRegisters( CPUCopy );
}