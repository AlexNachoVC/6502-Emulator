cmake_minimum_required(VERSION 3.14)
project(6502_Emulator)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# source for the test executable
set  (M6502_SOURCES
    "src/main_6502.cpp"
    "src/6502LoadRegisterTests.cpp"
    "src/6502StoreRegisterTests.cpp"
    "src/6502JumpsAndCallsTests.cpp"
    "src/6502StackOperationsTests.cpp"
    "src/6502LogicalOpTests.cpp"
    "src/6502LoadPrgTests.cpp"
    "src/6502TransferRegisterTests.cpp"
    "src/6502IncrementDecrementTests.cpp"
    "src/6502BranchTests.cpp"
    "src/6502StatusFlagChangeTests.cpp"
    "src/6502SystemFunctionsTests.cpp"
    "src/6502Add_SubWithCarryTests.cpp"
    "src/6502CompareRegistersTests.cpp"
    "src/6502ShiftsTests.cpp")
    
source_group("src" FILES ${M6502_SOURCES})

add_executable( M6502Test ${M6502_SOURCES} )
add_dependencies( M6502Test M6502Lib )
target_link_libraries( M6502Test gtest )
target_link_libraries( M6502Test M6502Lib )
