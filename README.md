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

Also this is being developed locally, not on Grace, since Grace is deprecated starting 04/01/26

# ts-traversal

You will need to setup nodejs and related dependencies for ts-traversal, see the readme

```
brew install binutils node gcc fftw
```

for scalapack

```
brew install openblas lapack scalapack
```

setup ts-traversal with

```
cd ./ts-traversal
npm install
export TS_TRAVERSAL="$YOUR_PATH/ts-traversal"
export RISCV="$HOME/riscv"
export PATH="$RISCV/bin:$PATH"
```

- can skip modifyHalo

# RISCV_MATRIX

You will need to setup GNU and Spike and related dependencies for RISCV_Matrix, see readme
