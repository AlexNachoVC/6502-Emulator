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

TEST_F( M6502Test1, LDAImmediateCanLoadAValueIntoTheARegister) 
{
    // Given: 
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    // End - inline a little program

    // When:
    cpu.Execute( 2, mem);

    // Then: 
    EXPECT_EQ( cpu.A, 0x84);
}

TEST_F( M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister) 
{
    // Given: 
    // Start - Inline a little program
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;
    // End - inline a little program

    // When:
    cpu.Execute( 3, mem);

    // Then: 
    EXPECT_EQ( cpu.A, 0x37);
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
