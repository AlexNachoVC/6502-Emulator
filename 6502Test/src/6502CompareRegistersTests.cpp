#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;


class M6502CompareRegistersTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    void ExpectUnaffectedRegisters( CPU CPUBefore )
    {
        EXPECT_EQ( CPUBefore.Flag.I, cpu.Flag.I );
        EXPECT_EQ( CPUBefore.Flag.D, cpu.Flag.D );
        EXPECT_EQ( CPUBefore.Flag.B, cpu.Flag.B );
        EXPECT_EQ( CPUBefore.Flag.V, cpu.Flag.V );
    }

};

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoIdenticalValues )
{
    // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = false;
        cpu.Flag.N = true;
        cpu.Flag.C = false;
        cpu.A = 26;
        mem[0xFF00] = CPU::INS_CMP_IM;
        mem[0xFF01] = 26;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, 26 );
        EXPECT_TRUE( cpu.Flag.Z );
        EXPECT_FALSE( cpu.Flag.N );
        EXPECT_TRUE( cpu.Flag.C );
        ExpectUnaffectedRegisters( CPUCopy );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoDifferentPositiveValues )
{
    // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = true;
        cpu.Flag.N = true;
        cpu.Flag.C = false;
        cpu.A = 48;
        mem[0xFF00] = CPU::INS_CMP_IM;
        mem[0xFF01] = 26;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, 48 );
        EXPECT_FALSE( cpu.Flag.Z );
        EXPECT_FALSE( cpu.Flag.N );
        EXPECT_TRUE( cpu.Flag.C );
        ExpectUnaffectedRegisters( CPUCopy );
}
