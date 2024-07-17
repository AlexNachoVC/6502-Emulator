#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;

/*
; TestPrg

* = $1000

lda = #$FF

start
sta $90
sta $8000
eor #$CC
jmp start

 */
static Byte TestPrg[] = {
        0x00, 0x10, 0xA9, 0xFF, 0x85, 0x90,
        0x8D, 0x00, 0x80, 0x49, 0xCC, 0x4C, 0x02, 0x10 };

static const u32 NumBytesInPrg = 14;

class M6502LoadPrgTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }
};

TEST_F( M6502LoadPrgTests, TestLoadProgramIntoTheCorrectAreaOfMemory )
{
    // Given:
    
    // Verify program is in memory correctly!

    // When: 
    cpu.LoadPrg( TestPrg, 14, mem );

    // Then: 
    EXPECT_EQ( mem[0x0FFF], 0x0 );
    EXPECT_EQ( mem[0x1000], 0xA9 );
    EXPECT_EQ( mem[0x1001], 0xFF );
    EXPECT_EQ( mem[0x1002], 0x85 );
    //...
    EXPECT_EQ( mem[0x1009], 0x4C );
    EXPECT_EQ( mem[0x100A], 0x02 );
    EXPECT_EQ( mem[0x100B], 0x10 );
    EXPECT_EQ( mem[0x100C], 0x0 );
}

TEST_F( M6502LoadPrgTests, TestLoadProgramAndExecuteIt )
{
    // Given:

    // When: 
    Word StartAddress = cpu.LoadPrg( TestPrg, NumBytesInPrg, mem );
    cpu.PC = StartAddress;

    // Then:
    for( s32 Clock = 10000; Clock > 0; )
    {
        Clock -= cpu.Execute( 20, mem );
    }
}
