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

    void TestStoreRegisterAbsolute( Byte OpcodeToTest, Byte CPU::*RegisterToTest ) 
    {
        // Given:
        cpu.*RegisterToTest = 0x2F;
        mem[0xFFFC] = OpcodeToTest;
        mem[0xFFFD] = 0x00;
        mem[0xFFFE] = 0x80;
        mem[0x8000] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // When:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

        // Then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( mem[0x8000], 0x2F );
        VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
    }

    void TestStoreRegisterZeroPageX( Byte OpcodeToTest, Byte CPU::*RegisterToTest ) 
    {
        // Given:
        cpu.*RegisterToTest = 0x42;
        cpu.X = 0x0F;
        mem[0xFFFC] = OpcodeToTest;
        mem[0xFFFD] = 0x80;
        mem[0x008F] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // When:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

        // Then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( mem[0x008F], 0x42 );
        VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
    }

    void TestStoreRegisterZeroPageY( Byte OpcodeToTest, Byte CPU::*RegisterToTest ) 
    {
        // Given:
        cpu.*RegisterToTest = 0x42;
        cpu.Y = 0x0F;
        mem[0xFFFC] = OpcodeToTest;
        mem[0xFFFD] = 0x80;
        mem[0x008F] = 0x00;
        constexpr s32 EXPECTED_CYCLES = 4;
        CPU CPUCopy = cpu;

        // When:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

        // Then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( mem[0x008F], 0x42 );
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

TEST_F( M6502StoreRegisterTests, STAAbsoluteCanStoreTheARegisterIntoMemory )
{
    TestStoreRegisterAbsolute( CPU::INS_STA_ABS, &CPU::A );
}

TEST_F( M6502StoreRegisterTests, STXAbsoluteCanStoreTheXRegisterIntoMemory )
{
    TestStoreRegisterAbsolute( CPU::INS_STX_ABS, &CPU::X );
}

TEST_F( M6502StoreRegisterTests, STYAbsoluteCanStoreTheYRegisterIntoMemory )
{
    TestStoreRegisterAbsolute( CPU::INS_STY_ABS, &CPU::Y );
}

TEST_F( M6502StoreRegisterTests, STAZeroPageXCanStoreTheARegisterIntoMemory )
{
    TestStoreRegisterZeroPageX( CPU::INS_STA_ZPX, &CPU::A );
}

TEST_F( M6502StoreRegisterTests, STXZeroPageYCanStoreTheXRegisterIntoMemory )
{
    TestStoreRegisterZeroPageY( CPU::INS_STX_ZPY, &CPU::X );
}

TEST_F( M6502StoreRegisterTests, STYZeroPageXCanStoreTheYRegisterIntoMemory )
{
    TestStoreRegisterZeroPageX( CPU::INS_STY_ZPX, &CPU::Y );
}

TEST_F( M6502StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory )
{
    // Given:
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_ABSX;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x800F], 0x42 );
    VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

TEST_F( M6502StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory )
{
    // Given:
    cpu.A = 0x42;
    cpu.Y = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_ABSY;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x80;
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x008F], 0x42 );
    VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}


TEST_F( M6502StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory )
{
    // Given:
    cpu.A = 0x42;
    cpu.X = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_INDX;
    mem[0xFFFD] = 0x20;
    mem[0x002F] = 0x00;
    mem[0x0030] = 0x80;
    mem[0x8000] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000], 0x42 );
    VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

TEST_F( M6502StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory )
{
    // Given:
    cpu.A = 0x42;
    cpu.Y = 0x0F;
    mem[0xFFFC] = CPU::INS_STA_INDY;
    mem[0xFFFD] = 0x20;
    mem[0x0020] = 0x00;
    mem[0x0021] = 0x80;
    mem[0x8000 + 0x0F] = 0x00;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    // When:
    const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem);

    // Then:
    EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
    EXPECT_EQ( mem[0x8000 + 0x0F], 0x42 );
    VerifyUnmodifiedFlagsFromStoreRegister( cpu, CPUCopy );
}

