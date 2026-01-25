#tests
SHELL=/bin/bash

CC     = riscv64-unknown-elf-gcc
LIBS = -L../riscv-blas -L../fftw-3.3.10/.libs -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm
CFLAGS = -DUSE_RISCV_VECTOR  -march=rv64gcv1p0 -g3 -pedantic -Wall -O2

MATRIXLIB = -L../src -lmatrix

QRUN = qemu-riscv64 -cpu rv64,x-v=true,vlen=1024,elen=64,vext_spec=v1.0
SPIKE = /gpfs/gibbs/pi/manohar/local/esp-tools/chipyard/esp-tools-install/bin/spike --isa=rv64gcv --varch=vlen:1024,elen:64 pk
RUN = ${SPIKE}

N_RUNS_SPEED=5
OUTPUT_FILE_timeit=speed_result.txt

tests := $(shell ls */*.c | sed 's/\.c//g')
objects := $(shell ls */*.c | sed 's/\.c/.o/g')
executables := $(shell ls */*.c | sed 's/\.c/.riscv/g')
tests_abbr := $(foreach tst,$(tests),$(word 1,$(subst /, ,$(tst))))

# tests := $(shell ls [^SPEED]*/*.c | sed 's/\.c//g')
# objects := $(shell ls [^SPEED]*/*.c | sed 's/\.c/.o/g')
# executables := $(shell ls [^SPEED]*/*.c | sed 's/\.c/.riscv/g')

.PHONY: clearscreen preclean clean

all: clearscreen preclean $(tests)

$(tests): %: %.o
	@echo Compiling $@
	@${CC} ${CFLAGS} ${INCLUDES} -o $@.riscv $^ ${MATRIXLIB} ${LIBS}
# 	${RUN} $@.riscv
# 	@VALGRIND=$(shell ${CC} ${CFLAGS} ${INCLUDES} -o $@ $^ ${LIBS} && ${VALGRIND_LOUD} ./$@ >/dev/null 2>&1)
# 	@if [ $(.SHELLSTATUS) -eq 22 ]; \
# 	then echo -e "\e[1;31mVALGRIND ERROR\e[0m : $@";\
# 	else \
# 		diff -wi <(./$@          2>/dev/null | sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | tr -s ' \t\n' | sed -z 's/\n/ \n/g' | sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') \
# 				<(octave -q $@.m 2>/dev/null | sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | tr -s ' \t\n' | sed -z 's/\n/ \n/g' | sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g')&&\
# 		echo -e "\e[1;32mSUCCESS\e[0m : $@"||\
# 		echo -e "\e[1;31mDIFF ERROR\e[0m : $@";\
# 	fi;

$(objects): %.o: %.c

# compare: $(tests)
# 	@for tst in $(tests) ; do \
# 		diff -wi <(./$$tst          2>/dev/null | sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | tr -s ' \t\n' | sed -z 's/\n/ \n/g' | sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') \
# 				<(octave -q $$tst.m 2>/dev/null | sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | tr -s ' \t\n' | sed -z 's/\n/ \n/g' | sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g')&&\
# 		echo -e "\e[1;32mSUCCESS\e[0m : $$tst"||\
# 		echo -e "\e[1;31mDIFF ERROR\e[0m : $$tst";\
# 	done;

# Compile the matrix library
matrix: FORCE
	@echo Compiling Matrix Library
	@make -C ../src	

runC: 
	for tst in $(tests_abbr) ; do \
		echo Running $$tst.riscv; \
		${RUN} $$tst/$$tst.riscv 2>/dev/null > "outputs/grace/grace_$${tst}_output.txt"; \
	done

compare_outputs: 
	for tst in $(tests_abbr) ; do \
		diff -wi  <(tail -n+2 "outputs/grace/grace_$${tst}_output.txt") \
		          "outputs/zoo/zoo_$${tst}_output.txt" &&\
		echo -e "\e[1;32mSUCCESS\e[0m : $$tst"||\
		echo -e "\e[1;31mDIFF ERROR\e[0m : $$tst";\
	done;

# runOctave: $(tests)
# 	for tst in $(tests) ; do \
# 		octave -q $$tst.m > "outputs/zoo/zoo_$${tst}_output.txt"; \
# 	done

timeit: $(tests)
	rm -f ${OUTPUT_FILE_timeit}
	if [ ${N_RUNS_SPEED} -eq 1 ];\
	then \
		echo -e "Timing runtime of all tests (${N_RUNS_SPEED} run each)\n" ;\
		echo -e "Test@C Runtime (${N_RUNS_SPEED} run)" > ${OUTPUT_FILE_timeit} ;\
	else \
		echo -e "Timing runtime of all tests (${N_RUNS_SPEED} runs each)\n" ;\
		echo -e "Test@C Runtime (${N_RUNS_SPEED} runs)" > ${OUTPUT_FILE_timeit} ;\
	fi;
	for tst in $(tests) ; do \
		echo -e "$$tst @" >> ${OUTPUT_FILE_timeit} ; \
		echo -e "\e[1A\e[2KTiming: $$tst.riscv" ; \
		TIMEFORMAT="%8R @"; { time for i in {1..${N_RUNS_SPEED}}; do ${RUN} $$tst.riscv >/dev/null 2>&1; done ; } 2>> ${OUTPUT_FILE_timeit}  ; \
		echo -e "\n" >> ${OUTPUT_FILE_timeit} ; \
	done ; \
	sed -i -z 's/ @\n/@/g' ${OUTPUT_FILE_timeit}
	column -t -s '@' ${OUTPUT_FILE_timeit} > TTTT.txt
	mv TTTT.txt ${OUTPUT_FILE_timeit}
	echo -e "\e[1A\e[2KRuntime results saved in ${OUTPUT_FILE_timeit}"

clearscreen:
	clear

preclean: FORCE
	@rm -f $(objects)
	@rm -f $(executables)

clean: FORCE
	@rm -f $(objects)
	@rm -f $(executables)

FORCE:
