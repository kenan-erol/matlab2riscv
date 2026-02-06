Welcome to my senior thesis jit

to clone:
```
git clone --recurse-submodules https://github.com/kenan-erol/matlab2riscv.git

# Or if already cloned without submodules:
git submodule update --init --recursive
```

Prerequisites
You need these tools installed:

RISC-V GNU Toolchain (for riscv64-unknown-elf-gcc)
Spike RISC-V Simulator (for running the compiled code)
GNU Octave (for comparison with MATLAB-equivalent output)
Node.js (for ts-traversal)
