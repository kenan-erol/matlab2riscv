CC     = riscv64-unknown-elf-gcc
MATRIXLIB = -Lsrc -lmatrix
LIBS = -Lriscv-blas -Lfftw-3.3.10/.libs -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm
CFLAGS = -DUSE_RISCV_VECTOR  -march=rv64gcv1p0 -g3 -pedantic -Wall -O2

all: clearscreen clean matrix main

main: main.o
	${CC} ${CFLAGS} -o $@.riscv $^ ${MATRIXLIB} ${LIBS} 

main.o: main.c

matrix: FORCE
	make -C src

tests_Vec_C: matrix FORCE
	make -C tests_Vec_C

clearscreen:
	clear

clean: FORCE
	rm -f *.o
	rm -f src/*.o

FORCE: