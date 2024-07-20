#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502BranchTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

};


TEST_F( M6502BranchTests, BEQCanBranchForwardsWhenZeroIsSet )
{
    // Given:
    cpu.Reset( 0xFF00, mem );
    cpu.Flag.Z = true;
    mem[0xFF00] = CPU::INS_BEQ;
    mem[0xFF01] = 0x1;
    constexpr s32 EXPECTED_CYCLES = 3;  
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( cpu.PC, 0xFF03 );
    EXPECT_EQ( cpu.PS, CPUCopy.PS );
}
