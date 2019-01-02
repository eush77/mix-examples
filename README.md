# mix-examples

This repository contains examples for `llvm.mix` extension. Each example can
be run as a benchmark, and can optionally write a dump of `stage(1)` LLVM IR.

## Configuration

LLVM and Clang with `llvm.mix` support are needed to be built and installed
somewhere.

Set `LLVM_DIR` variable to the directory containing the installed
`LLVMConfig.cmake` when invoking CMake:

    $ cmake /path/to/src -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=/path/to/dist/lib/cmake/llvm

## Running

All examples are linked into a single executable `mix-examples`. Use
`--benchmark_filter` command-line option to separate particular examples to
run:

    $ ./clang/mix-examples --benchmark_filter=PowerIter

See `--help` and the [documentation] for Google Benchmark for more options.

[documentation]: https://github.com/google/benchmark

Additional options are controlled by the following environment variables:

| Variable        | Values | Default | Description                    |
|-----------------|--------|---------|--------------------------------|
| `MIX_DUMP`      | 0 or 1 | 0       | Dump stage(1) LLVM IR to files |
| `MIX_OPT_LEVEL` | 0..3   | 2       | JIT optimization level         |
