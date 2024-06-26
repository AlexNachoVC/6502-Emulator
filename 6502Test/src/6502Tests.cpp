#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;
class M6502Test1 : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    void TestLoadRegisterImmediate( Byte Opcode, Byte CPU::*Register );
    void TestLoadRegisterZeroPage( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterZeroPageX( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterZeroPageY( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterAbsolute( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterAbsoluteX( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterAbsoluteY( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterAbsoluteXWhenCrossingPageBoundary( Byte OpcodeToTest, Byte CPU::*RegisterToTest );
    void TestLoadRegisterAbsoluteYWhenCrossingPageBoundary( Byte OpcodeToTest, Byte CPU::*RegisterToTest );


};

static void VerifyUnmodifiedFlagsFromLoadRegister( const CPU& cpu, const CPU& CPUCopy) { 
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


void M6502Test1::TestLoadRegisterImmediate( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: 
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x84;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 2, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x84 );
    EXPECT_EQ( CyclesUsed, 2 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_TRUE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAImmediateCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterImmediate( CPU::INS_LDA_IM, &CPU::A );
}

TEST_F( M6502Test1, LDXImmediateCanLoadAValueIntoTheXRegister ) 
{
    TestLoadRegisterImmediate( CPU::INS_LDX_IM, &CPU::X );
}

TEST_F( M6502Test1, LDYImmediateCanLoadAValueIntoTheYRegister ) 
{
    TestLoadRegisterImmediate( CPU::INS_LDY_IM, &CPU::Y );
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
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502Test1::TestLoadRegisterZeroPage( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: 
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 3, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 3 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterZeroPage( CPU::INS_LDA_ZP, &CPU::A );
}

TEST_F( M6502Test1, LDXZeroPageCanLoadAValueIntoTheXRegister ) 
{
    TestLoadRegisterZeroPage( CPU::INS_LDX_ZP, &CPU::X );
}

TEST_F( M6502Test1, LDYZeroPageCanLoadAValueIntoTheYRegister ) 
{
    TestLoadRegisterZeroPage( CPU::INS_LDY_ZP, &CPU::Y );
}

void M6502Test1::TestLoadRegisterZeroPageX( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: 
    cpu.X = 5;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502Test1::TestLoadRegisterZeroPageY( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: 
    cpu.Y = 5;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterZeroPageX( CPU::INS_LDA_ZPX, &CPU::A );
}

TEST_F( M6502Test1, LDXZeroPageYCanLoadAValueIntoTheXRegister ) 
{
    TestLoadRegisterZeroPageY( CPU::INS_LDX_ZPY, &CPU::X );
}

TEST_F( M6502Test1, LDYZeroPageXCanLoadAValueIntoTheYRegister ) 
{
    TestLoadRegisterZeroPageX( CPU::INS_LDY_ZPX, &CPU::Y );
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
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502Test1::TestLoadRegisterAbsolute( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: 
    cpu.Z = cpu.N = true;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterAbsolute( CPU::INS_LDA_ABS, &CPU::A );
}

TEST_F( M6502Test1, LDXAbsoluteCanLoadAValueIntoTheXRegister ) 
{
    TestLoadRegisterAbsolute( CPU::INS_LDX_ABS, &CPU::X );
}

TEST_F( M6502Test1, LDYAbsoluteCanLoadAValueIntoTheYRegister ) 
{
    TestLoadRegisterAbsolute( CPU::INS_LDY_ABS, &CPU::Y );
}

void M6502Test1::TestLoadRegisterAbsoluteX( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: ;
    cpu.Z = cpu.N = true;
    cpu.X = 1;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502Test1::TestLoadRegisterAbsoluteY( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: ;
    cpu.Z = cpu.N = true;
    cpu.Y = 1;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterAbsoluteX( CPU::INS_LDA_ABSX, &CPU::A );
}

TEST_F( M6502Test1, LDXAbsoluteYCanLoadAValueIntoTheXRegister ) 
{
    TestLoadRegisterAbsoluteY( CPU::INS_LDX_ABSY, &CPU::X );
}

TEST_F( M6502Test1, LDYAbsoluteXCanLoadAValueIntoTheYRegister ) 
{
    TestLoadRegisterAbsoluteX( CPU::INS_LDY_ABSX, &CPU::Y );
}

void M6502Test1::TestLoadRegisterAbsoluteXWhenCrossingPageBoundary( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: ;
    cpu.X = 0xFF;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

void M6502Test1::TestLoadRegisterAbsoluteYWhenCrossingPageBoundary( Byte OpcodeToTest, Byte CPU::*RegisterToTest )
{
    // Given: ;
    cpu.Y = 0xFF;
    mem[0xFFFC] = OpcodeToTest;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.*RegisterToTest, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    TestLoadRegisterAbsoluteXWhenCrossingPageBoundary( CPU::INS_LDA_ABSX, &CPU::A );
}

TEST_F( M6502Test1, LDYAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    TestLoadRegisterAbsoluteXWhenCrossingPageBoundary( CPU::INS_LDY_ABSX, &CPU::Y );
}

TEST_F( M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegister ) 
{
    TestLoadRegisterAbsoluteY( CPU::INS_LDA_ABSY, &CPU::A );
}

TEST_F( M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    TestLoadRegisterAbsoluteYWhenCrossingPageBoundary( CPU::INS_LDA_ABSY, &CPU::A );
}

TEST_F( M6502Test1, LDXAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    TestLoadRegisterAbsoluteYWhenCrossingPageBoundary( CPU::INS_LDX_ABSY, &CPU::X );
}

TEST_F( M6502Test1, LDAIndirectXCanLoadAValueIntoTheARegister ) 
{
    // Given: ;
    cpu.Z = cpu.N = true;
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
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAIndirectYCanLoadAValueIntoTheARegister ) 
{
    // Given: ;
    cpu.Z = cpu.N = true;
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00; 
    mem[0x0003] = 0x80; 
    mem[0x8004] = 0x37; // 0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
}

TEST_F( M6502Test1, LDAIndirectYCanLoadAValueIntoTheARegisterWhenItCrossesAPageBoundary ) 
{
    // Given: ;
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02; 
    mem[0x0003] = 0x80; 
    mem[0x8101] = 0x37; // 0x8002 + 0xFF
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    EXPECT_EQ( cpu.A, 0x37 );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Z );
    EXPECT_FALSE( cpu.N );
    VerifyUnmodifiedFlagsFromLoadRegister( cpu, CPUCopy );
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
