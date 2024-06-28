#include <gtest/gtest.h>
#include "main_6502.h"


class M6502Test1 : public testing::Test {
protected:
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }
};

static void VerifyUnmodifiedFlagsFromLDA( const CPU& cpu, const CPU& CPUCopy) { 
    EXPECT_EQ( cpu.C, CPUCopy.C);
    EXPECT_EQ( cpu.I, CPUCopy.I);
    EXPECT_EQ( cpu.D, CPUCopy.D);
    EXPECT_EQ( cpu.B, CPUCopy.B);
    EXPECT_EQ( cpu.V, CPUCopy.V);
}

TEST_F( M6502Test1, TheCPUDoesNothingWhenWeExecuteZeroCycles ) 
{   
    // Given: 
    constexpr s32 NUM_CYCLES = 0;

    // When:
    s32 CyclesUsed = cpu.Execute( NUM_CYCLES, mem );

    // Then:
    EXPECT_EQ( CyclesUsed, 0 );
}

TEST_F( M6502Test1, CPUCanExecuteMoreCyclesThanRequestedIfRequiredByTheInstruction ) 
{
    // Given: 
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    CPU CPUCopy = cpu;
    constexpr s32 NUM_CYCLES = 1;

    // When:
    s32 CyclesUsed = cpu.Execute( NUM_CYCLES, mem );

    // Then: 
    EXPECT_EQ( CyclesUsed, 2 );
}

TEST_F( M6502Test1, ExecutingABadInstructionDoesNotPutUsInAnInfiniteLoop ) 
{
    // Given: 
    mem[0xFFFC] = 0x0;  // Invalid Instruction/Opcode
    mem[0xFFFD] = 0x0;
    CPU CPUCopy = cpu;
    constexpr s32 NUM_CYCLES = 1;

    // When:
    s32 CyclesUsed = cpu.Execute( NUM_CYCLES, mem );

    // Then: 
    EXPECT_EQ( CyclesUsed, NUM_CYCLES );
}

TEST_F( M6502Test1, LDAImmediateCanLoadAValueIntoTheARegister ) 
{
    // Given: 
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    // End - inline a little program

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 2, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x84 );
    EXPECT_EQ( CyclesUsed, 2 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_TRUE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAImmediateCanAffectTheZeroFlag ) 
{
    // Given: 
    cpu.A = 0x44;
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x0;
    CPU CPUCopy = cpu;

    // When:   
    cpu.Execute( 2, mem );

    // Then: 
    EXPECT_TRUE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister ) 
{
    // Given: 
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;
    // End - inline a little program

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 3, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, 3 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister ) 
{
    // Given: 
    cpu.X = 5;
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;
    // End - inline a little program

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps ) 
{
    // Given: 
    cpu.X = 0xFF;
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;
    // End - inline a little program

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister ) 
{
    // Given: 
    mem[0xFFFC] = CPU::INS_LDA_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegister ) 
{
    // Given: ;
    cpu.X = 1;
    mem[0xFFFC] = CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    // Given: ;
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegister ) 
{
    // Given: ;
    cpu.X = 1;
    mem[0xFFFC] = CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    // Given: ;
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAIndirectXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    // Given: ;
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; // 0x2 + 0x4
    mem[0x0007] = 0x80; 
    mem[0x8000] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLDA( cpu, CPUCopy );
}

#if 0
#include "main_6502.h"

int main() {
    Mem mem;
    CPU cpu;
    cpu.Reset( mem );
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFDE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    // End - inline a little program
    cpu.Execute( 9, mem);
    return 0;
}

#endif
