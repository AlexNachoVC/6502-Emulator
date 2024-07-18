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

    void ExpectUnaffectedFlags( CPU CPUBefore ) {
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
    ExpectUnaffectedFlags( CPUCopy );
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
    ExpectUnaffectedFlags( CPUCopy );
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
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INYCanIncrementAZeroValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0x0;
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
    mem[0xFF00] = CPU::INS_INY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0x01 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INYCanIncrement255 )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0xFF;
    cpu.Flag.Z = false;
    cpu.Flag.N = true;
    mem[0xFF00] = CPU::INS_INY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0x0 );    // NOTE: does this instruction actualy wrap?
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, INYCanIncrementANegativeValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0b10001000;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_INY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0b10001001 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecrementAZeroValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0x0;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0xFF );   // NOTE: Does this wrap?
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecrement255 )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0xFF;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0xFE );    
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEXCanDecrementANegativeValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0b10001001;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0b10001000 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEYCanDecrementAZeroValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0x0;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0xFF );   // NOTE: Does this wrap?
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEYCanDecrement255 )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0xFF;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0xFE );    
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DEYCanDecrementANegativeValue )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Y = 0b10001001;
    cpu.Flag.Z = true;
    cpu.Flag.N = false;
    mem[0xFF00] = CPU::INS_DEY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.Y, 0b10001000 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPage )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
    mem[0xFF00] = CPU::INS_DEC_ZP;
    mem[0xFF01] = 0x42;
    mem[0x0042] = 0x57;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042], 0x56 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

TEST_F( M6502IncrementDecrementTests, DECCanDecrementAValueInTheZeroPageX )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = true;
    cpu.Flag.N = true;
    cpu.X = 0x10;
    mem[0xFF00] = CPU::INS_DEC_ZPX;
    mem[0xFF01] = 0x42;
    mem[0x0042+0x10] = 0x57;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x0042 + 0x10], 0x56 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    ExpectUnaffectedFlags( CPUCopy );
}

