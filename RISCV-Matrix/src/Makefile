CC     = riscv64-unknown-elf-gcc
AR     = riscv64-unknown-elf-ar
LIBS = -L../riscv-blas -L../fftw-3.3.10/.libs -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm
CFLAGS = -DUSE_RISCV_VECTOR  -march=rv64gcv1p0 -g3 -pedantic -Wall -O2

all: libmatrix.a

objects := $(shell ls *.c | sed 's/\.c/.o/g')

libmatrix.a: $(objects)
	${AR} rcs $@ $^

matrix: libmatrix.a

$(objects): %.o: %.c
	${CC} -c ${CFLAGS} $^

clearscreen:
	clear

preclean: FORCE
	rm -f libmatrix.a
	rm -f *.o

clean: FORCE
	rm -f *.o

FORCE: