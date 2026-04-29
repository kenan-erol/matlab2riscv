# matlab2riscv

End-to-end MATLAB → RISC-V compilation pipeline for the HALO Brain-Computer Interface project. Walks any MATLAB `.m` file through type inference, C source generation, RVV-vectorized library linking, RISC-V cross-compilation, and Spike simulation.

This top-level repository pulls in two submodules that do the actual work:
- **`ts-traversal/`** — TypeScript MATLAB-to-C source-to-source compiler (parses the `.m` via tree-sitter-matlab, infers static types, emits C against the matrix library's API).
- **`RISCV-Matrix/`** — RVV-vectorized C matrix library (the runtime ts-traversal links against). Provides `_ref` and `_vec` paths for every supported kernel.

The full thesis report is in [`docs/final/main.pdf`](docs/final/main.pdf).

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Clone](#clone)
- [Install the RISC-V toolchain](#install-the-risc-v-toolchain)
- [Install Spike + the proxy kernel](#install-spike--the-proxy-kernel)
- [Install host dependencies](#install-host-dependencies)
- [Set environment variables](#set-environment-variables)
- [Build the matrix library](#build-the-matrix-library)
- [Set up ts-traversal](#set-up-ts-traversal)
- [Translate a MATLAB file end-to-end](#translate-a-matlab-file-end-to-end)
- [Run the test suites](#run-the-test-suites)
- [Repository layout](#repository-layout)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

- macOS (Apple Silicon or Intel) or Linux. Tested on macOS 14 with Homebrew and Yale's Grace cluster (deprecated April 2026; everything in this repo now builds locally).
- Approximately 4 GB free disk for the toolchain build, 2 GB for `node_modules` and FFTW/BLAS object files.
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
make -j$(sysctl -n hw.ncpu)        # ~30 minutes on M-series, ~45 on Intel
```

After this, `$HOME/riscv/bin/riscv64-unknown-elf-gcc --version` should print `gcc (g) 15.2.0` (or newer). If you have an older `riscv64-elf-gcc` from Homebrew or another source, make sure `$HOME/riscv/bin` is *first* on `PATH` so the right one is picked up.

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

Sanity-check: `which spike` should show `~/riscv/bin/spike`, and `~/riscv/riscv64-unknown-elf/bin/pk` should exist.

## Install host dependencies

Octave (for golden-output generation), Node.js (ts-traversal runtime), FFTW (RISCV-Matrix's signal-processing kernels link against it cross-compiled), GNU coreutils (the test Makefiles use `gsed` and `gtimeout` on macOS):

```bash
# macOS (Homebrew)
brew install binutils node gcc fftw octave coreutils gnu-sed
brew install openblas lapack scalapack       # for BLAS-backed kernels

# Octave statistics package (required by tests_C_Octave/advanced_stats.m)
octave --eval "pkg install -forge io"
octave --eval "pkg install -forge datatypes"
octave --eval "pkg install -forge statistics"
```

`fftw` is also vendored in `RISCV-Matrix/fftw-3.3.10/` and is built cross-compiled for RISC-V during the matrix library build — the Homebrew install above is only used by Octave for reference output generation.

## Set environment variables

```bash
export RISCV="$HOME/riscv"
export PATH="$RISCV/bin:$PATH"
export TS_TRAVERSAL="$(pwd)/ts-traversal"
export OCTAVEC="$(pwd)/RISCV-Matrix/src"
# Only needed if you want to run the EEGLAB-derived tests:
# export EEGLAB="$HOME/eeglab"
```

Add those to `~/.zshrc` or `~/.bashrc` so future shells pick them up.

Verify the toolchain is reachable:

```bash
riscv64-unknown-elf-gcc --version | head -1     # should be 15.2.0+
spike --help 2>&1 | head -1                     # "Spike RISC-V ISA simulator"
ls $RISCV/riscv64-unknown-elf/bin/pk            # should exist
```

## Build the matrix library

```bash
make -C RISCV-Matrix/src
```

This cross-compiles every `.c` in `RISCV-Matrix/src/` with `-march=rv64gcv1p0 -O2` and packs the result into `RISCV-Matrix/src/libmatrix.a`. ~20 seconds on first run; incremental thereafter. You can watch which kernels are vectorized vs reference in `RISCV-Matrix/README.md` § "Table of Vectorization Status".

If you also want to regenerate FFTW and the BLAS shims:

```bash
make -C RISCV-Matrix matrix       # rebuilds libmatrix.a
# riscv-blas and fftw-3.3.10 are built one-time when first invoked by tests
```

## Set up ts-traversal

```bash
cd ts-traversal
npm install                       # ~1 minute, pulls tree-sitter-matlab + @types/node

# If you plan to run the ts-traversal test suite (make compareall),
# you must ensure MATLAB is activated and reachable in your environment:
cd generatedCode
matlab -nodesktop -nosplash -nodisplay -r "disp('hello'); exit"
cd ../..
```

That's it — TypeScript runs via `ts-node`, no separate compile step needed.

## Translate a MATLAB file end-to-end

The two pieces of the pipeline are invoked separately:
1. **Transpile**: `npx ts-node` against `ts-traversal/index.ts` produces `main.c` + `main.h`.
2. **Compile + link**: `riscv64-unknown-elf-gcc` against `main.c` and a handful of helper objects (`convertSubscript.o`, `unknownTypes.o`, `wrapper.o`, `matrix.o`) produces a static RISC-V ELF.
3. **Run**: `spike` + `pk` execute the ELF and write Spike's `BENCHMARK_INSTRET` trailer to stdout.

You can do all three from the repository root without ever entering the `ts-traversal/` `make` system. Pick a name for your test (here `myscript`):

```bash
# Convenience handles
GEN=$TS_TRAVERSAL/generatedCode
WORKDIR=$GEN/myscript
OCTAVEC=$(pwd)/RISCV-Matrix/src
RISCVMAT=$(pwd)/RISCV-Matrix
INC="-I. -I$OCTAVEC -I$GEN"
CFLAGS="-std=gnu99 -DUSE_RISCV_VECTOR -march=rv64gcv1p0 -include $OCTAVEC/rvv_compat.h"
LIBS="-L$OCTAVEC -L$RISCVMAT/riscv-blas -L$RISCVMAT/fftw-3.3.10/.libs \
      -lmatrix -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm"

# 1. Drop your MATLAB file into a new per-test directory
mkdir -p $WORKDIR
cp /path/to/myscript.m $WORKDIR/

# 2. Transpile MATLAB → C
npx --prefix $TS_TRAVERSAL ts-node \
    $TS_TRAVERSAL/index.ts \
    $WORKDIR/myscript.m  $WORKDIR  $TS_TRAVERSAL  0  0
# Produces: $WORKDIR/main.c, $WORKDIR/main.h

# 3. (One-time) compile the helper objects shared by every translated test
riscv64-unknown-elf-gcc -c -o $GEN/matrix.o           $OCTAVEC/matrix.c           $CFLAGS $INC
riscv64-unknown-elf-gcc -c -o $GEN/convertSubscript.o $GEN/convertSubscript.c     $CFLAGS $INC
riscv64-unknown-elf-gcc -c -o $GEN/unknownTypes.o     $GEN/unknownTypes.c         $CFLAGS $INC
riscv64-unknown-elf-gcc -c -o $GEN/wrapper.o          $GEN/wrapper.c              $CFLAGS $INC

# 4. Compile and link your test
riscv64-unknown-elf-gcc -c -o $WORKDIR/main.o  $WORKDIR/main.c  $CFLAGS -Dmain=generated_main $INC
riscv64-unknown-elf-gcc -o $WORKDIR/test \
    $WORKDIR/main.o \
    $GEN/convertSubscript.o $GEN/unknownTypes.o $GEN/wrapper.o $GEN/matrix.o \
    $CFLAGS $INC $LIBS

# 5. Run on Spike
spike -m4096 --isa=rv64gcv_zvl1024b_zicntr $RISCV/riscv64-unknown-elf/bin/pk $WORKDIR/test
```

The helper-object compilation in step 3 is one-time per checkout (re-run only if `RISCV-Matrix/src` or the helper sources change). For subsequent tests, only steps 1, 2, 4, and 5 are needed.

### Smoke-test the pipeline

A 7-line MATLAB script that exercises matrix literal construction, scalar broadcast, elementwise add, and a column-wise reduction:

```bash
mkdir -p $TS_TRAVERSAL/generatedCode/myscript
cat > $TS_TRAVERSAL/generatedCode/myscript/myscript.m <<'EOF'
a = [1, 2, 3; 4, 5, 6];
disp(a);

b = 10*ones(2,3);
c = a + b;
disp(c);

m = mean(a);
disp(m);
EOF

WORKDIR=$TS_TRAVERSAL/generatedCode/myscript

# Transpile
npx --prefix $TS_TRAVERSAL ts-node \
    $TS_TRAVERSAL/index.ts \
    $WORKDIR/myscript.m $WORKDIR $TS_TRAVERSAL 0 0

# Compile (helper objects assumed already built per Step 3 above)
GEN=$TS_TRAVERSAL/generatedCode
OCTAVEC=$(pwd)/RISCV-Matrix/src
RISCVMAT=$(pwd)/RISCV-Matrix
CFLAGS="-std=gnu99 -DUSE_RISCV_VECTOR -march=rv64gcv1p0 -include $OCTAVEC/rvv_compat.h"
INC="-I. -I$OCTAVEC -I$GEN"
LIBS="-L$OCTAVEC -L$RISCVMAT/riscv-blas -L$RISCVMAT/fftw-3.3.10/.libs \
      -lmatrix -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm"

riscv64-unknown-elf-gcc -c -o $WORKDIR/main.o $WORKDIR/main.c $CFLAGS -Dmain=generated_main $INC
riscv64-unknown-elf-gcc -o $WORKDIR/test \
    $WORKDIR/main.o \
    $GEN/convertSubscript.o $GEN/unknownTypes.o $GEN/wrapper.o $GEN/matrix.o \
    $CFLAGS $INC $LIBS

# Run
spike -m4096 --isa=rv64gcv_zvl1024b_zicntr $RISCV/riscv64-unknown-elf/bin/pk $WORKDIR/test
```

Expected output (verified on a fresh checkout, April 2026):

```
1   2   3
4   5   6


11  12  13
14  15  16


2.5  3.5  4.5


BENCHMARK_INSTRET: 217523
```

The trailing `BENCHMARK_INSTRET` line is emitted by the `wrapper.o` shim and reports the retired-instruction count for the entire `main()` — useful for cycle-budget analysis. Strip it if you need byte-clean output.

### Notes on what ts-traversal accepts as input

The transpiler handles a subset of MATLAB. For a fresh `.m` to compile cleanly without manual cleanup:
- Use `disp(...)` for output. The Octave-only helpers `intDisp`/`doubleDisp`/`complexDisp` are mapped to `disp` only inside the `tests_C_Octave/` regression suite (via `cleanUp.ts`); for arbitrary user `.m`s, write `disp` directly.
- Avoid Octave-only directives (`more off`, `format short`, `source ...;`). The transpiler treats those as identifier statements and emits invalid C.
- Avoid scalar-broadcast on `Matrix*` (e.g., `b = a + 10`). Construct an explicit `10*ones(rows,cols)` and use `Matrix + Matrix` instead — that path is properly mapped to `plusM(...)`.
- Avoid `endwhile`, multi-function `.m` files, and `classdef`. See [`ts-traversal/README.md`](ts-traversal/README.md) § "Current limitations" for the full list.

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

# 3. ts-traversal end-to-end (every .m → main.c → .riscv → output, against MATLAB reference)
( cd ts-traversal/generatedCode && make compareall )
( cd ts-traversal/generatedCode && make timeall  )
```

Both suites use the same tolerance gate (`tolerant_diff.py` at `MANTISSA_TOL = ABS_TOL = 5e-10`).

## Repository layout

```
matlab2riscv/
├── README.md               # this file
├── index.html              # CPSC 4900 submission landing page
├── docs/
│   ├── final/main.pdf      # final thesis report
│   ├── poster.pdf          # final poster
│   └── Kenan Erol Spring 2026 Project Proposal for CPSC 4900-2.pdf
├── RISCV-Matrix/           # submodule: vectorized C library + tests
│   ├── src/                # library sources (compiled into libmatrix.a)
│   ├── tests_Vec_C/        # vec-vs-ref accuracy + speed harness
│   ├── tests_C_Octave/     # C-vs-Octave correctness harness
│   ├── BSC-RVV/            # BSC RVV macro layer (vector_defines.h)
│   ├── fftw-3.3.10/        # cross-compiled FFTW
│   ├── riscv-blas/         # RISC-V port of OpenBLAS
│   └── README.md           # library reference incl. vectorization status table
└── ts-traversal/           # submodule: TypeScript MATLAB→C compiler
    ├── *.ts                # source: index.ts, generateCode.ts, etc.
    ├── generatedCode/      # per-test work directory + Makefile
    │   ├── myscript/       # your MATLAB file lands here
    │   ├── tolerant_diff.py
    │   └── timediff_latex.py
    └── README.md           # transpiler reference
```

## Troubleshooting

**`undefined reference to *_vec` at link time.** The matrix library is stale. Force-rebuild:
```bash
rm RISCV-Matrix/src/*.o RISCV-Matrix/src/libmatrix.a
make -C RISCV-Matrix/src
```

**`vsetvl_e32m1: implicit declaration`.** You're on an older toolchain that doesn't ship the LMUL=1 set. Pull the latest `riscv-gnu-toolchain` and rebuild — RVV 1.0 intrinsics names changed between 0.7 and 1.0.

**`pkg load statistics` fails in Octave.** Install the dependency chain in order: `pkg install -forge io`, then `datatypes`, then `statistics`.

**`tic`/`toc` rewriting eats `static`.** Was a real bug in `cleanUp.ts`; fixed by adding `\b` word boundaries. If you hit it on an older checkout, pull the fix from the latest `ts-traversal` `main`.

**Tests fail with token-count mismatch but values look identical.** The C side likely emits a stderr diagnostic that pk routes to stdout (e.g., singular-matrix or non-diagonalizable warnings). Mirror the diagnostic literal in the matching `.m` — see `tests_C_Octave/invert_matrix/invert_matrix.m` for the canonical pattern.

**FFTW-vs-fftpack ULP scatter > tolerance.** Documented in `RISCV-Matrix/README.md` § "Numerical Divergence and the Tolerant Diff". Worst residual on the suite is `filterM` token 3586 at `5.246e-10` (just above the `5e-10` `ABS_TOL` short-circuit). Loosen `ABS_TOL` to `1e-9` if you need that test green.

For more, see [`RISCV-Matrix/README.md`](RISCV-Matrix/README.md) and [`ts-traversal/README.md`](ts-traversal/README.md), which document each subsystem in depth.
