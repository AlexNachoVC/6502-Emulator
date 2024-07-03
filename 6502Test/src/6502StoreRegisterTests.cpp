#include <gtest/gtest.h>
#include "m6502.h"

using namespace m6502;
class M6502StoreRegisterTests : public testing::Test {
protected:
    
    Mem mem;
    CPU cpu;

    virtual void SetUp(){
        cpu.Reset( mem );
    }

    virtual void TearDown(){
    }

};

#if 0
static void VerifyUnmodifiedFlagsFromStoreRegister( const CPU& cpu, const CPU& CPUCopy) { 
    EXPECT_EQ( cpu.C, CPUCopy.C);
    EXPECT_EQ( cpu.I, CPUCopy.I);
    EXPECT_EQ( cpu.D, CPUCopy.D);
    EXPECT_EQ( cpu.B, CPUCopy.B);
    EXPECT_EQ( cpu.V, CPUCopy.V);
    EXPECT_EQ( cpu.Z, CPUCopy.Z);
    EXPECT_EQ( cpu.N, CPUCopy.N);
}
#endif 