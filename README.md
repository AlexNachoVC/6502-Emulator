# 6502-Emulator

Learning about processors!

# 08/2024

* All 6502 legal opcodes emulated
* Decimal mode is not handled
* Test program [/Klaus2m5/6502_65C02_functional_tests](https://github.com/Klaus2m5/6502_65C02_functional_tests) - will succeed if decimal is disabled.
* Counting cycles individually for each part of an instruction is cumbersome and probably should just deduct the correct number at the end of the instruction.
* There is no way to issue and interrupt to this virtual CPU
* There are no hooks for debugging.
* There is is no dissasembler or UI, this is just the CPU emulator & units test.
* There are no asserts if you write memory outside of the bounds (it will overwrite memory)
* Illegal opcodes are not implemented, the program will throw an exception.