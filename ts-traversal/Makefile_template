#tests
OCTAVEC=/home/dlg59/project/Halo-Algorithm/OctaveC

INC = -I. -I${OCTAVEC} -I${EBROOTOPENBLAS}/include -I${EBROOTFFTW}
LIB = -L${EBROOTOPENBLAS}/lib -L${EBROOTSCALAPACK}/lib -L${EBROOTFFTW}/lib
VPATH = .:${OCTAVEC}

CC=gcc
CFLAGS = -lm -lopenblas -lscalapack -lfftw3

# Copy mfiles from tests_C_Octave folder into generatedCode and "clean"
copyall:
	if [ -f "matrix.o" ]; then \
		eval $(CC) -c -o matrix.o $$OCTAVEC/matrix.c $(CFLAGS) $(LIB) $(INC); \
	fi; \
	mfiles=$$(find $$OCTAVEC/tests_C_Octave -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		filename="$${filename%.*}"; \
		if [ ! -d "$(shell pwd)/$$filename" ]; then \
			echo $$filename; \
			eval npx ts-node $$TS_TRAVERSAL/cleanUp.ts $$filename.m $$TS_TRAVERSAL; \
		fi \
	done; \
	
# Generate main.c for all mfiles that haven't already been generated	
genall:
	mfiles=$$(find $$OCTAVEC/tests_C_Octave -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		filename="$${filename%.*}"; \
		if [ ! -f "$(shell pwd)/$$filename/main.c" ]; then \
			echo $$filename; \
			eval npx ts-node $$TS_TRAVERSAL/index.ts $(shell pwd)/$$filename/$$filename.m $$TEST $$TS_TRAVERSAL 0 0; \
		fi \
	done; \

# Generate main.c for all mfiles, regardless of whether already generated
regenall:
	mfiles=$$(find $(shell pwd) -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		filename="$${filename%.*}"; \
		echo $$filename; \
		eval npx ts-node $$TS_TRAVERSAL/index.ts $(shell pwd)/$$filename/$$filename.m $$TEST $$TS_TRAVERSAL 0 0; \
	done; \

# Compile all C programs
dirs := $(shell ls -d */)
compileall: $(dirs)
	@for dir in $(dirs) ; do \
		fulldir="$(shell pwd)/$$dir"; \
		dir=$${dir//\//}; \
		echo $$dir; \
		mfiles=$$(find $$fulldir -type f -name "*.m"); \
		srcs=$$(echo "$$mfiles" | sed "s/$$dir.m/main/"); \
		objects=$$(echo "$$mfiles" | sed "s/$$dir.m/main.o/"); \
		for src in $$srcs; do \
			eval $(CC) -c -o $$src.o $$src.c $(CFLAGS) $(LIB) $(INC); \
		done; \
		eval $(CC) -c -o $$fulldir/octavec_main.o $$fulldir/octavec_main.c $(CFLAGS) $(LIB) $(INC); \
		eval $(CC) -o $$fulldir/test $$objects matrix.o $(CFLAGS) $(LIB) $(INC); \
		if [ -f "$$fulldir/octavec_main.c" ]; then \
			eval $(CC) -o $$fulldir/check $$fulldir/octavec_main.o matrix.o $(CFLAGS) $(LIB) $(INC); \
		else \
			echo "Custom test"; \
		fi \
	done; \
	
# Compile one C program
# make compileone ARGS=testname
compileone:
	fulldir="$(shell pwd)/$(ARGS)"; \
	mfiles=$$(find $$fulldir -type f -name "*.m"); \
	srcs=$$(echo "$$mfiles" | sed "s/$(ARGS).m/main/"); \
	objects=$$(echo "$$mfiles" | sed "s/$(ARGS).m/main.o/"); \
	for src in $$srcs; do \
		eval $(CC) -c -o $$src.o $$src.c $(CFLAGS) $(LIB) $(INC); \
	done; \
	eval $(CC) -c -o $$fulldir/octavec_main.o $$fulldir/octavec_main.c $(CFLAGS) $(LIB) $(INC); \
	eval $(CC) -o $$fulldir/test $$objects matrix.o $(CFLAGS) $(LIB) $(INC); \
	if [ -f "$$fulldir/octavec_main.c" ]; then \
		eval $(CC) -o $$fulldir/check $$fulldir/octavec_main.o matrix.o $(CFLAGS) $(LIB) $(INC); \
	else \
		echo "Custom test"; \
	fi \
	
# Show MATLAB output for all mfiles
runmatlab:
	mfiles=$$(find $(shell pwd) -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		echo $$filename; \
		matlab -nojvm -nodisplay -nosplash < $$mfile; \
	done; \
	
# Compare outputs of generated C program and original mfile for all tests
compareall:
	mfiles=$$(find $(shell pwd) -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		filename="$${filename%.*}"; \
		fulldir="$(shell pwd)/$$filename"; \
		($$fulldir/test 2>/dev/null | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') > $$fulldir/f1; \
		(matlab -nodisplay -nosplash < $$fulldir/$$filename.m 2>/dev/null | \
			tail -n+10 | sed -z 's/>>//gi' | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') | \
			sed -z 's/\n\s/\n/gi' | sed -z 's/  \n//gi' > $$fulldir/f2; \
		diff -wi $$fulldir/f1 $$fulldir/f2 && \
			echo -e "\e[1;32mSUCCESS\e[0m : $$filename"||\
			echo -e "\e[1;31mDIFF ERROR\e[0m : $$filename";\
	done; \
	
# Compare outputs of generated C program and original mfile for one test
# make compareone ARGS=testname
compareone:
	fulldir="$(shell pwd)/$(ARGS)"; \
	($$fulldir/test 2>/dev/null | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') > $$fulldir/f1; \
	(matlab -nodisplay -nosplash < $$fulldir/$(ARGS).m 2>/dev/null | \
		tail -n+10 | sed -z 's/>>//gi' | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') | \
		sed -z 's/\n\s/\n/gi' | sed -z 's/  \n//gi' > $$fulldir/f2; \
	diff -wi $$fulldir/f1 $$fulldir/f2 && \
		echo -e "\e[1;32mSUCCESS\e[0m : $(ARGS)"||\
		echo -e "\e[1;31mDIFF ERROR\e[0m : $(ARGS)";\