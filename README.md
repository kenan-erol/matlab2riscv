# matlab2riscv

End-to-end MATLAB -> RISC-V compilation pipeline for the HALO Brain-Computer Interface project. Walks any MATLAB `.m` file through type inference, custom function identification, C source generation, RVV-vectorized library linking, RISC-V cross-compilation, and Spike simulation.

This top-level repository pulls in two submodules that do the actual work:
- **`ts-traversal/`** — TypeScript MATLAB-to-C source-to-source compiler (parses the `.m` via tree-sitter-matlab, infers static types, emits C against matrix.h).
- **`RISCV-Matrix/`** — RVV-vectorized C matrix library (the runtime ts-traversal links against). Provides `_ref` and `_vec` paths for every supported kernel.

The full thesis report is in [`docs/final/main.pdf`](docs/final/main.pdf).

---

## Prerequisites

- macOS or Linux. Tested on macOS 14 with Homebrew.
- Approximately 4 GB free disk for the toolchain build, 2 GB for `ts-traversal/node_modules` and FFTW/BLAS object files.
- `git`, `bash`, `make` (already shipped on macOS Command Line Tools and most Linux distros).

If you are on the Yale Zoo cluster, replace the `brew install` lines below with `module load` equivalents (`module load binutils nodejs GCC ScaLAPACK/2.1.0-gompi-2020b FFTW`).

## Clone

```bash
git clone --recurse-submodules https://github.com/kenan-erol/matlab2riscv.git
cd matlab2riscv
# If you cloned without --recurse-submodules:
git submodule update --init --recursive
```

## Install the RISC-V toolchain

You need `riscv64-unknown-elf-gcc` 15.2.0 or newer with the `gcv1p0` (RVV 1.0) extension enabled. The simplest path on macOS is the official `riscv-gnu-toolchain` build:

```bash
# 1. Source toolchain
git clone https://github.com/riscv-collab/riscv-gnu-toolchain ~/riscv-gnu-toolchain
cd ~/riscv-gnu-toolchain
./configure --prefix=$HOME/riscv \
            --with-arch=rv64gcv1p0 \
            --with-abi=lp64d
make -j$(sysctl -n hw.ncpu)
```

After this, `$HOME/riscv/bin/riscv64-unknown-elf-gcc --version` should print `gcc (g) 15.2.0` (or newer).

## Install Spike + the proxy kernel

Spike (the official RISC-V ISA simulator) plus pk (the proxy kernel that handles syscalls):

```bash
# Spike
git clone https://github.com/riscv-software-src/riscv-isa-sim ~/riscv-isa-sim
cd ~/riscv-isa-sim
mkdir build && cd build
../configure --prefix=$HOME/riscv
make -j$(sysctl -n hw.ncpu) && make install

# pk (proxy kernel — runs the riscv binary under spike)
git clone https://github.com/riscv-software-src/riscv-pk ~/riscv-pk
cd ~/riscv-pk
mkdir build && cd build
../configure --prefix=$HOME/riscv \
             --host=riscv64-unknown-elf \
             --with-arch=rv64gcv1p0
make && make install
```

`which spike` should show `~/riscv/bin/spike`, and `~/riscv/riscv64-unknown-elf/bin/pk` should exist.

## Install host dependencies

```bash
# macOS (Homebrew)
brew install node octave coreutils gnu-sed

# Octave statistics package (needed by tests_C_Octave/advanced_stats.m)
octave --eval "pkg install -forge io"
octave --eval "pkg install -forge datatypes" # if it asks for an octave update, skip
octave --eval "pkg install -forge statistics"
```

## Set environment variables

```bash
export RISCV="$HOME/riscv"
export PATH="$RISCV/bin:$PATH"
export TS_TRAVERSAL="$(pwd)/ts-traversal"
export OCTAVEC="$(pwd)/RISCV-Matrix/src"
# Only needed if you want to run the EEGLAB-derived tests (once class derivation is fixed):
# export EEGLAB="$HOME/eeglab"
```

Add those to `~/.zshrc` or `~/.bashrc` so future shells pick them up.

Verify the toolchain is reachable:

```bash
riscv64-unknown-elf-gcc --version # should be 15.2.0+
spike --help 2>&1 # "Spike RISC-V ISA simulator"
ls $RISCV/riscv64-unknown-elf/bin/pk # should exist
```

## Build the matrix library

```bash
make -C RISCV-Matrix/src # or
make -C RISCV-Matrix matrix		# rebuilds libmatrix.a
```

## Set up ts-traversal

```bash
cd ts-traversal
npm install

# If you plan to run the ts-traversal test suite (make compareall),
# you must ensure MATLAB is activated and reachable in your environment:
cd generatedCode
matlab -nodesktop -nosplash -nodisplay -r "disp('hello'); exit"
cd ../..
```


### The standalone tool: `bin/translate`

The repo ships a single shell script that does the whole pipeline on any `.m` at any path:

```bash
# Translate, compile, run, and Octave-diff myscript.m
bin/translate --compare myscript.m

# Just translate + compile (no run)
bin/translate myscript.m

# Translate + compile + run on Spike (no Octave diff)
bin/translate --run myscript.m
```

A top-level Makefile mirrors the same operations:

```bash
make translate ARGS=myscript.m
make run       ARGS=myscript.m
make compare   ARGS=myscript.m
make matrix
make clean
```

The `--compare` flow uses the same `tolerant_diff.py` (5e-10 frexp gate) as the C-Octave and ts-traversal test suites.

### Falling back to the legacy ts-traversal `make` flow

If `bin/translate` doesn't fit your workflow (e.g. you want to inspect the per-test `Makefile` recipes, or you're running the full `compareall` regression suite), the original ts-traversal flow still works:

```bash
mkdir -p ts-traversal/generatedCode/myscript
cp myscript.m ts-traversal/generatedCode/myscript/
cd ts-traversal/generatedCode
make genone     ARGS=myscript   # MATLAB -> main.c
make compileone ARGS=myscript   # main.c -> test (RISC-V ELF)
make compareone ARGS=myscript   # spike + MATLAB diff
```

`make compareone` requires MATLAB on the host. `bin/translate --compare` uses Octave instead.

## Run the test suites

Three independent test surfaces; all read from the same matrix library:

```bash
# 1. RISC-V vectorized vs reference (tests every kernel's _vec vs _ref)
make -C RISCV-Matrix/tests_Vec_C check        # accuracy
make -C RISCV-Matrix/tests_Vec_C run          # speed

# 2. C vs Octave end-to-end (validates that printM output matches Octave reference)
make -C RISCV-Matrix/tests_C_Octave runC      # run all C tests, capture stdout
make -C RISCV-Matrix/tests_C_Octave runOctave # run all .m tests, capture stdout
make -C RISCV-Matrix/tests_C_Octave compare_outputs > \
    RISCV-Matrix/tests_C_Octave/outputs/output_comparison.txt

# 3. ts-traversal end-to-end (every .m -> main.c -> .riscv -> output, against MATLAB reference)
( cd ts-traversal/generatedCode && make compareall )
( cd ts-traversal/generatedCode && make timeall_fast  )
```

Both suites use the same tolerance gate (`tolerant_diff.py` at `MANTISSA_TOL = ABS_TOL = 5e-10`).

