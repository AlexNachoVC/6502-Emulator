#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502StackOperationsTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

};

TEST_F( M6502StackOperationsTests, TSXCanTransferTheStackPointerToXRegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.X = 0x00;
    cpu.SP = 0x01;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x01 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
}


TEST_F( M6502StackOperationsTests, TSXCanTransferAZeroStackPointerToXRegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.X = 0x00;
    cpu.SP = 0x00;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0x00 );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
}


TEST_F( M6502StackOperationsTests, TSXCanTransferANegativeStackPointerToXRegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = cpu.Flag.N = false;
    cpu.X = 0x00;
    cpu.SP = 0b10000000;
    mem[0xFF00] = CPU::INS_TSX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.X, 0b10000000 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.N );
}


TEST_F( M6502StackOperationsTests, TXSCanTransferXRegisterToTheStack )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.X = 0xFF;
    cpu.SP = 0;
    mem[0xFF00] = CPU::INS_TXS;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.SP, 0xFF );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
}


TEST_F( M6502StackOperationsTests, PHACanPushARegisterOntoTheStack )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.A = 0x42;
    mem[0xFF00] = CPU::INS_PHA;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[cpu.SPToAddress() + 1], cpu.A );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.SP, 0xFE );
}

TEST_F( M6502StackOperationsTests, PHPCanPushProcessorStatusOntoTheStack )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.PS = 0xCC;  
    mem[0xFF00] = CPU::INS_PHP;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[cpu.SPToAddress() + 1], 0xCC | CPU::UnusedFlagBit | CPU::BreakFlagBit );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
    EXPECT_EQ( cpu.SP, 0xFE );
}

TEST_F( M6502StackOperationsTests, PHPAlwaysSetsBits4And5OnTheStack )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.PS = 0x0;  
    mem[0xFF00] = CPU::INS_PHP;
    constexpr s32 EXPECTED_CYCLES = 3;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    Word AddPSOnStack = cpu.SPToAddress() + 1;
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );

    // https://wiki.nesdev.com/w/index.php/Status_flags
	//Two interrupts (/IRQ and /NMI) and two instructions (PHP and BRK) push 
	// the flags to the stack. In the byte pushed, bit 5 is always set to 1, 
	//and bit 4 is 1 if from an instruction (PHP or BRK) or 0 if from an 
	// interrupt line being pulled low (/IRQ or /NMI). This is the only time 
	// and place where the B flag actually exists: not in the status register 
	// itself, but in bit 4 of the copy that is written to the stack. 
    const Byte FlagsOnStack = CPU::UnusedFlagBit | CPU::BreakFlagBit;
    EXPECT_EQ( mem[AddPSOnStack], FlagsOnStack );
}

TEST_F( M6502StackOperationsTests, PLACanPullAValueFromTheStackIntoTheARegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.A = 0x00;
    cpu.SP = 0xFE;
    mem[0x01FF] = 0x42; 
    mem[0xFF00] = CPU::INS_PLA;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x42 );
    EXPECT_EQ( cpu.SP, 0xFF );
}

TEST_F( M6502StackOperationsTests, PLACanPullAZeroValueFromTheStackIntoTheARegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = false;
    cpu.Flag.N = true;
    cpu.A = 0x42;
    cpu.SP = 0xFE;
    mem[0x01FF] = 0x00; 
    mem[0xFF00] = CPU::INS_PLA;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x00 );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.N );
    EXPECT_EQ( cpu.SP, 0xFF );
}

TEST_F( M6502StackOperationsTests, PLACanPullANegativeValueFromTheStackIntoTheARegister )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.N = false;
    cpu.Flag.Z = true;
    cpu.A = 0x42;
    cpu.SP = 0xFE;
    mem[0x01FF] = 0b10000001; 
    mem[0xFF00] = CPU::INS_PLA;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0b10000001 );
    EXPECT_TRUE( cpu.Flag.N );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.SP, 0xFF );  
}

TEST_F( M6502StackOperationsTests, PLPCanPullAValueFromTheStackIntoTheProcessorStatus )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.SP = 0xFE;
    cpu.PS = 0;
    mem[0x01FF] = 0x42 | CPU::BreakFlagBit | CPU::UnusedFlagBit; 
    mem[0xFF00] = CPU::INS_PLP;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, 0x42 );
}

TEST_F( M6502StackOperationsTests, PLPClearsBits4And5WhenPullingFromTheStack )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.SP = 0xFE;
    cpu.PS = 0;
    mem[0x01FF] = CPU::BreakFlagBit | CPU::UnusedFlagBit; 
    mem[0xFF00] = CPU::INS_PLP;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PS, 0 );
}
