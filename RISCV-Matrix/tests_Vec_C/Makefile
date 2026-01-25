SHELL=/bin/bash

CC     = riscv64-unknown-elf-gcc
MATRIXLIB = -L../src -lmatrix
LIBS = -L../riscv-blas -L../fftw-3.3.10/.libs -llapacke -llapack -lcblas -lrefblas -lf2c -lfftw3 -lm
CFLAGS = -DUSE_RISCV_VECTOR  -march=rv64gcv1p0 -g3 -pedantic -Wall -O2

QRUN = qemu-riscv64 -cpu rv64,x-v=true,vlen=1024,elen=64,vext_spec=v1.0
SPIKE = /gpfs/gibbs/pi/manohar/local/esp-tools/chipyard/esp-tools-install/bin/spike --isa=rv64gcv --varch=vlen:1024,elen:64 pk
RUN = ${SPIKE}

ACCURACYFLAG = --accuracy

tests := $(shell ls *_test/*_test.c | sed 's/\.c//g')
objects := $(shell ls *_test/*_test.c | sed 's/\.c/.o/g')
executables := $(shell ls *_test/*_test.c | sed 's/\.c/.riscv/g')

tests_abbr := $(foreach tst,$(tests),$(word 1,$(subst /, ,$(tst))))

accuracy_outputs := $(shell ls *_test/*_test.c | sed 's/\.c/_accuracy_output.txt/g')
speed_outputs := $(shell ls *_test/*_test.c | sed 's/\.c/_speed_output.txt/g')


all: clearscreen preclean matrix common/utils.o run

# Compile all tests (or a specific one)
# Formatted as, for example: abs_vec_test/abs_vec_test 
$(tests): %: common/utils.o %.o
	@echo Compiling $(word 1,$(subst /, ,$@))
	@${CC} ${CFLAGS} ${INCLUDES} -o $@.riscv $^ ${MATRIXLIB} ${LIBS}


compile: preclean matrix common/utils.o $(tests)

# Objects
$(objects): %.o: %.c

# Compile & speed-run all tests (or a specific one), then format their outputs nicely
# Formatted as, for example: abs_vec_test
$(tests_abbr): common/utils.o FORCE
	@$(MAKE) $@/$@
	@echo 
	@date > $@/$@_speed_output.txt
	@echo Running $@
	@date
	@${RUN} $@/$@.riscv >> $@/$@_speed_output.txt 2>/dev/null || \
	echo -e "\e[1;32mFINISHED\e[0m : $@";
	@date
	@tail -n-3 $@/$@_speed_output.txt > ZZZZ_tmp.txt
	@head -n-3 $@/$@_speed_output.txt | column -ts $$';\t' > time_results/$@.txt
	@cat ZZZZ_tmp.txt >> time_results/$@.txt
	@rm ZZZZ_tmp.txt
	@echo 

# Compile & accuracy-run all tests
check: common/utils.o $(tests) 
	@echo 
	@date
	@for tst in $(tests) ; do \
		echo Checking $$tst ; \
		date > "$${tst}_accuracy_output.txt" ; \
		${RUN} $$tst.riscv ${ACCURACYFLAG} >> "$${tst}_accuracy_output.txt" 2>/dev/null \
		&& echo -e "\e[1;31mACCURACY ERROR\e[0m : $${tst}" \
		|| echo -e "\e[1;32mSUCCESS\e[0m : $${tst}"; \
	done
	@date
	@echo 

# Compile & speed-run all tests
time run: common/utils.o $(tests) date $(tests_abbr) date

# Print out the current time
date: FORCE
	@date

# Compile the utility functions
common/utils.o: common/utils.c FORCE
	@echo Compiling $@
	@${CC} ${CFLAGS} ${INCLUDES} ${MATRIXLIB} ${LIBS} common/utils.c -c -o $@

# Compile the matrix library
matrix: FORCE
	@echo Compiling Matrix Library
	@make -C ../src

clearscreen:
	clear

preclean: FORCE
	@rm -f common/utils.o
	@rm -f $(objects)
	@rm -f $(executables)

clean: FORCE
	@rm -f common/utils.o
	@rm -f $(objects)
	@rm -f $(executables)
	@rm -f $(accuracy_outputs)

FORCE: