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

    struct CMPTestData
    {
        Byte A;
        Byte Operand;

        bool ExpectC;
        bool ExpectZ;
        bool ExpectN;
    };

    void CMPImmediate (CMPTestData Test )
    {
        // given:
        using namespace m6502;
        cpu.Reset( 0xFF00, mem );
        cpu.Flag.Z = !Test.ExpectZ;
        cpu.Flag.N = !Test.ExpectN;
        cpu.Flag.C = !Test.ExpectC;
        cpu.A = Test.A;
        mem[0xFF00] = CPU::INS_CMP_IM;
        mem[0xFF01] = Test.Operand;
        constexpr s32 EXPECTED_CYCLES = 2;
        CPU CPUCopy = cpu;

        // when:
        const s32 ActualCycles = cpu.Execute( EXPECTED_CYCLES, mem );

        // then:
        EXPECT_EQ( ActualCycles, EXPECTED_CYCLES );
        EXPECT_EQ( cpu.A, Test.A );
        EXPECT_EQ( cpu.Flag.Z, Test.ExpectZ );
        EXPECT_EQ( cpu.Flag.N, Test.ExpectN );
        EXPECT_EQ( cpu.Flag.C, Test.ExpectC );
        ExpectUnaffectedRegisters( CPUCopy );
    }

};

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoIdenticalValues )
{
    CMPTestData Test;
    Test.A = 26;    
    Test.Operand = 26;
    Test.ExpectZ = true;
    Test.ExpectN = false;
    Test.ExpectC = true;
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoDifferentPositiveValues )
{
    CMPTestData Test;
    Test.A = 48;    
    Test.Operand = 26;
    Test.ExpectZ = false;
    Test.ExpectN = false;
    Test.ExpectC = true;
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareANegativeNumberToAPositive )
{   
    CMPTestData Test;
    Test.A = 130;    // Negative number!
    Test.Operand = 26;
    Test.ExpectZ = false;
    Test.ExpectN = false;
    Test.ExpectC = true;
    CMPImmediate( Test );
}

TEST_F( M6502CompareRegistersTests, CMPImmediateCanCompareTwoValuesThatResultInANegativeFlagSet )
{
    CMPTestData Test;
    Test.A = 8;    
    Test.Operand = 26;
    Test.ExpectZ = false;
    Test.ExpectN = true;
    Test.ExpectC = false;
    CMPImmediate( Test );
}

