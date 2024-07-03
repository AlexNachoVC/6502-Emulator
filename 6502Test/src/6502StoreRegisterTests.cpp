#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;

static void VerifyUnmodifiedFlagsFromStoreRegister( const CPU& cpu, const CPU& CPUCopy) { 
    EXPECT_EQ( cpu.C, CPUCopy.C);
    EXPECT_EQ( cpu.I, CPUCopy.I);
    EXPECT_EQ( cpu.D, CPUCopy.D);
    EXPECT_EQ( cpu.B, CPUCopy.B);
    EXPECT_EQ( cpu.V, CPUCopy.V);
    EXPECT_EQ( cpu.Z, CPUCopy.Z);
    EXPECT_EQ( cpu.N, CPUCopy.N);
}
class M6502StoreRegisterTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

    void TestStoreRegisterZeroPage( Byte OpcodeToTest, Byte CPU::*RegisterToTest ) 
    {
        // Given:
        cpu.*RegisterToTest = 0x2F;
        mem[0xFFFC] = OpcodeToTest;
        mem[0xFFFD] = 0x80;
        mem[0x0080] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 3;
        CPU CPUCopy = cpu;

        // When:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

        // Then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( mem[0x0080], 0x2F );
        VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
    }
};

TEST_F( M6502StoreRegisterTests, STAZeroPageCanStoreTheARegisterIntoMemory )
{
    TestStoreRegisterZeroPage( CPU::INS_STA_ZP, &CPU::A );
}

TEST_F( M6502StoreRegisterTests, STXZeroPageCanStoreTheXRegisterIntoMemory )
{
    TestStoreRegisterZeroPage( CPU::INS_STX_ZP, &CPU::X );
}

TEST_F( M6502StoreRegisterTests, STYZeroPageCanStoreTheYRegisterIntoMemory )
{
    TestStoreRegisterZeroPage( CPU::INS_STY_ZP, &CPU::Y );
}