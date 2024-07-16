#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502LogicalOpTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    static void VerifyUnmodifiedFlagsFromLogicalOpInstructions( const CPU& cpu, const CPU& CPUCopy) { 
    EXPECT_EQ( cpu.Flag.C, CPUCopy.Flag.C);
    EXPECT_EQ( cpu.Flag.I, CPUCopy.Flag.I);
    EXPECT_EQ( cpu.Flag.D, CPUCopy.Flag.D);
    EXPECT_EQ( cpu.Flag.B, CPUCopy.Flag.B);
    EXPECT_EQ( cpu.Flag.V, CPUCopy.Flag.V);
    }  

    enum class ELogicalOp
    {
        And, Eor, Or
    };

    Byte DoLogicalOp( Byte A, Byte B, ELogicalOp LogicalOp )
    {
        switch ( LogicalOp )
        {
        case ELogicalOp::And:
            return A & B;
            break;
        case ELogicalOp::Or:
            return A | B;
            break;
        case ELogicalOp::Eor:
            return A ^ B;
            break;
        }    

        throw -1; // Invalid Logical Op
    }

    void TestLogicalOpImmediate( ELogicalOp LogicalOp ) 
    {
    // Given: 
    cpu.A = 0xCC;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_IM;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_IM;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_IM;
        break;
    }    
    mem[0xFFFD] = 0x84;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 2, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x84, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, 2 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
    }

    void TestLogicalOpZeroPage( ELogicalOp LogicalOp ) 
    {
    // Given:  
    cpu.A = 0xCC;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ZP;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ZP;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ZP;
        break;
    }    
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 3, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, 3 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

    void TestLogicalOpZeroPageX( ELogicalOp LogicalOp )
    {
    // Given: 
    cpu.A = 0xCC;
    cpu.X = 5;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ZPX;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ZPX;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ZPX;
        break;
    }    
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

    void TestLogicalOpZeroPageXWhenItWraps( ELogicalOp LogicalOp )
    {
    // Given: 
    cpu.A = 0xCC;
    cpu.X = 0xFF;
    // Start - Inline a little program
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ZPX;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ZPX;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ZPX;
        break;
    }
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;
    // End - inline a little program

    // When:
    CPU CPUCopy = cpu;
    s32 CyclesUsed = cpu.Execute( 4, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, 4 );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}
    
    void TestLogicalOpAbsolute( ELogicalOp LogicalOp )
    {
    // Given: 
    cpu.A = 0xCC;
    cpu.Flag.Z = cpu.Flag.N = true;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ABS;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ABS;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ABS;
        break;
    }
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4480] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}
    
    void TestLogicalOpAbsoluteX( ELogicalOp LogicalOp )
    {
    // Given: 
    cpu.A = 0xCC;
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.X = 1;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ABSX;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ABSX;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ABSX;
        break;
    }
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}
    
    void TestLogicalOpAbsoluteY( ELogicalOp LogicalOp )
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.Y = 1;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ABSY;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ABSY;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ABSY;
        break;
    }
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4481] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 4;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
    }

    void TestLogicalOpAbsoluteXWhenCrossingPageBoundary( ELogicalOp LogicalOp )
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.X = 0xFF;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ABSX;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ABSX;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ABSX;
        break;
    }
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}
    
    void TestLogicalOpAbsoluteYWhenCrossingPageBoundary( ELogicalOp LogicalOp  )
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.Y = 0xFF;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_ABSY;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_ABSY;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_ABSY;
        break;
    }
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; // 0x4480
    mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

    void TestLogicalOpIndirectX( ELogicalOp LogicalOp) 
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.X = 0x04;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_INDX;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_INDX;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_INDX;
        break;
    }
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; // 0x2 + 0x4
    mem[0x0007] = 0x80; 
    mem[0x8000] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

    void TestLogicalOpIndirectY( ELogicalOp LogicalOp) 
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.Flag.Z = cpu.Flag.N = true;
    cpu.Y = 0x04;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_INDY;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_INDY;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_INDY;
        break;
    }
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00; 
    mem[0x0003] = 0x80; 
    mem[0x8004] = 0x37; // 0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

    void TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp LogicalOp )
    {
    // Given: ;
    cpu.A = 0xCC;
    cpu.Y = 0xFF;
    switch ( LogicalOp )
    {
    case ELogicalOp::And:
        mem[0xFFFC] = CPU::INS_AND_INDY;
        break;
    case ELogicalOp::Or:
        mem[0xFFFC] = CPU::INS_ORA_INDY;
        break;
    case ELogicalOp::Eor:
        mem[0xFFFC] = CPU::INS_EOR_INDY;
        break;
    }
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02; 
    mem[0x0003] = 0x80; 
    mem[0x8101] = 0x37; // 0x8002 + 0xFF
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    const Byte ExpectedResult  = DoLogicalOp( 0xCC, 0x37, LogicalOp );
    const bool ExpectedNegative = (ExpectedResult & 0b10000000 ) > 0;
    EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_EQ( cpu.Flag.N, ExpectedNegative );
    VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

};

TEST_F( M6502LogicalOpTests, TestLogicalOpAndOnARegisterImmediate ) 
{
    TestLogicalOpImmediate( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrOnARegisterImmediate ) 
{
    TestLogicalOpImmediate( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorROnARegisterImmediate ) 
{
    TestLogicalOpImmediate( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndOnARegisterZeroPage ) 
{
    TestLogicalOpZeroPage( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrOnARegisterZeroPage ) 
{
    TestLogicalOpZeroPage( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorOnARegisterZeroPage ) 
{
    TestLogicalOpZeroPage( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorImmediateCanAffectZeroFlag )
{
	// given:
	using namespace m6502;
	cpu.A = 0xCC;
	mem[0xFFFC] = CPU::INS_EOR_IM;
	mem[0xFFFD] = cpu.A;
	CPU CPUCopy = cpu;

	//when:
	cpu.Execute( 2, mem );

	//then:
	EXPECT_TRUE( cpu.Flag.Z );
	EXPECT_FALSE( cpu.Flag.N );
	VerifyUnmodifiedFlagsFromLogicalOpInstructions( cpu, CPUCopy );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndOnARegisterZeroPageX ) 
{
    TestLogicalOpZeroPageX( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrOnARegisterZeroPageX ) 
{
    TestLogicalOpZeroPageX( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorOnARegisterZeroPageX ) 
{
    TestLogicalOpZeroPageX( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, LogicalOpAndCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
	TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, LogicalOpOrCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
	TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::Or );
}
TEST_F( M6502LogicalOpTests, LogicalOpEorCanLoadAValueIntoTheARegisterWhenItWrapsZeroPageX )
{
	TestLogicalOpZeroPageXWhenItWraps( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndOnARegisterAbsolute ) 
{
    TestLogicalOpAbsolute( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrOnARegisterAbsolute ) 
{
    TestLogicalOpAbsolute( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorOnARegisterAbsolute ) 
{
    TestLogicalOpAbsolute( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndOnARegisterAbsoluteX )
{
	TestLogicalOpAbsoluteX( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrOnARegisterAbsoluteX )
{
	TestLogicalOpAbsoluteX( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorOnARegisterAbsoluteX )
{
	TestLogicalOpAbsoluteX( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteX )
{
	TestLogicalOpAbsoluteXWhenCrossingPageBoundary( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteX )
{
	TestLogicalOpAbsoluteXWhenCrossingPageBoundary( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteX )
{
	TestLogicalOpAbsoluteXWhenCrossingPageBoundary( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndAbsoluteY )
{
	TestLogicalOpAbsoluteY( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrAbsoluteY )
{
	TestLogicalOpAbsoluteY( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorAbsoluteY )
{
	TestLogicalOpAbsoluteY( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndWhenItCrossesAPageBoundaryAbsoluteY )
{
	TestLogicalOpAbsoluteYWhenCrossingPageBoundary( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrWhenItCrossesAPageBoundaryAbsoluteY )
{
	TestLogicalOpAbsoluteYWhenCrossingPageBoundary( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorWhenItCrossesAPageBoundaryAbsoluteY )
{
	TestLogicalOpAbsoluteYWhenCrossingPageBoundary( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndIndirectX )
{
	TestLogicalOpIndirectX( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorIndirectX )
{
	TestLogicalOpIndirectX( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrIndirectX )
{
	TestLogicalOpIndirectX( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndIndirectY )
{
	TestLogicalOpIndirectY( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrIndirectY )
{
	TestLogicalOpIndirectY( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorIndirectY )
{
	TestLogicalOpIndirectY( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpAndWhenItCrossesAPageIndirectY )
{
	TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::And );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpOrWhenItCrossesAPageIndirectY )
{
	TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::Or );
}

TEST_F( M6502LogicalOpTests, TestLogicalOpEorWhenItCrossesAPageIndirectY )
{
	TestLogicalOpIndirectYWhenItCrossesAPage( ELogicalOp::Eor );
}

TEST_F( M6502LogicalOpTests, TestBitZeroPage )
{
    // Given:  
    cpu.A = 0xCC;
    mem[0xFFFC] = CPU::INS_BIT_ZP; 
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0xCC;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.V );
    EXPECT_TRUE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitZeroPageResultZero )
{
    // Given:  
    cpu.A = 0xCC;
    mem[0xFFFC] = CPU::INS_BIT_ZP; 
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x33;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.V );
    EXPECT_FALSE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitZeroPageResultZeroBit6And7Zero )
{
    // Given:  
    cpu.A = 0x33;
    mem[0xFFFC] = CPU::INS_BIT_ZP; 
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0xCC;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x33 );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.V );
    EXPECT_TRUE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitZeroPageResultZeroBits6And7Mixed )
{
    // Given:  
    mem[0xFFFC] = CPU::INS_BIT_ZP; 
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0b01000000;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 3;
    cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_TRUE( cpu.Flag.V );
    EXPECT_FALSE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitAbsolute )
{
    // Given:  
    cpu.A = 0xCC;
    mem[0xFFFC] = CPU::INS_BIT_ABS; 
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0xCC;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_FALSE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.V );
    EXPECT_TRUE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitAbsoluteResultZero )
{
    // Given:  
    cpu.A = 0xCC;
    mem[0xFFFC] = CPU::INS_BIT_ABS; 
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0x33;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0xCC );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_FALSE( cpu.Flag.V );
    EXPECT_FALSE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitAbsoluteResultZeroBits6And7Zero )
{
    // Given:  
    cpu.A = 0x33;
    mem[0xFFFC] = CPU::INS_BIT_ABS; 
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0xCC;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    s32 CyclesUsed = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_EQ( CyclesUsed, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.A, 0x33 );
    EXPECT_TRUE( cpu.Flag.Z );
    EXPECT_TRUE( cpu.Flag.V );
    EXPECT_TRUE( cpu.Flag.N );
}

TEST_F( M6502LogicalOpTests, TestBitAbsoluteResultZeroBits6And7Mixed )
{
    // Given:  
    mem[0xFFFC] = CPU::INS_BIT_ABS; 
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    mem[0x8000] = 0b10000000;

    // When:
    CPU CPUCopy = cpu;
    constexpr s32 EXPECTED_CYCLES = 4;
    cpu.Execute( EXPECTED_CYCLES, mem );

    // Then: 
    //EXPECT_EQ( cpu.A, ExpectedResult );
    EXPECT_FALSE( cpu.Flag.V );
    EXPECT_TRUE( cpu.Flag.N );
}
