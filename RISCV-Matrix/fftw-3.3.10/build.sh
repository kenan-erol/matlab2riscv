#!/bin/bash

export PATH=/home/xw424/project/riscv-v-toolchain_20220821/_install/bin:$PATH

./configure CC=riscv64-unknown-elf-gcc CFLAGS="-static -mcmodel=medany -march=rv64gcv_zfh" --prefix=/home/zjt2/scratch60/project/RISCV-MATRIX/fftw-3.3.10 --disable-fortran --host=riscv64-unknown-elf

make 

make install
