# mix-examples

This repository contains example programs for `llvm.mix` extension.

## Configuration

LLVM and Clang with `llvm.mix` support are needed to be built and installed
somewhere.

Specify `LLVM_DIR` variable to the directory containing the installed
`LLVMConfig.cmake` when invoking CMake:

    $ cmake -DLLVM_DIR=/path/to/dist/lib/cmake/llvm /path/to/src

## Running

The build compiles executable examples in the build tree.

Pass flags `--baseline` and `--mix` to compare baseline and specialized code.

    $ ./clang/Power/power-iter --baseline
    $ ./clang/Power/power-iter --mix
