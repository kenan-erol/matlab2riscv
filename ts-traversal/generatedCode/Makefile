#tests
OCTAVEC=/home/dlg59/project/Halo-Algorithm/OctaveC

INC = -I. -I${OCTAVEC} -I${EBROOTOPENBLAS}/include -I${EBROOTFFTW}
LIB = -L${EBROOTOPENBLAS}/lib -L${EBROOTSCALAPACK}/lib -L${EBROOTFFTW}/lib
VPATH = .:${OCTAVEC}

CC=gcc
CFLAGS = -std=c99 -lm -lopenblas -lscalapack -lfftw3

all_dirs := $(shell ls -d */)
speed_dirs := $(shell ls -d SPEED*/)
nonspeed_dirs := $(shell ls -d */ | grep '^[^SPEED].*')

setup:
	module load binutils; \
	module load nodejs; \
	module load GCC; \
	module load ScaLAPACK/2.1.0-gompi-2020b; \
	module load FFTW; \
	
# Create matrix.o
compmatrix:
	eval $(CC) -c -o matrix.o $$OCTAVEC/matrix.c $(CFLAGS) $(LIB) $(INC); \

# Copy mfiles from tests_C_Octave folder into generatedCode and "clean"
copyall:
	if [ ! -f "matrix.o" ]; then \
		eval $(CC) -c -o matrix.o $$OCTAVEC/matrix.c $(CFLAGS) $(LIB) $(INC); \
	fi; \
	if [ ! -f "convertSubscript.o" ]; then \
		eval $(CC) -c -o convertSubscript.o $$TS_TRAVERSAL/generatedCode/convertSubscript.c $(CFLAGS) $(LIB) $(INC); \
	fi; \
	if [ ! -f "unknownTypes.o" ]; then \
		eval $(CC) -c -o unknownTypes.o $$TS_TRAVERSAL/generatedCode/unknownTypes.c $(CFLAGS) $(LIB) $(INC); \
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

matlabtorun:
	for dir in $(all_dirs) ; do \
		dir=$${dir//\//}; \
		echo $$dir; \
		eval npx ts-node $$TS_TRAVERSAL/matlabToRun.ts $$dir.m $$TS_TRAVERSAL; \
	done; \


		
# Generate one C program
# make genone ARGS=testname
genone:
	eval npx tsc -sourcemap $$TS_TRAVERSAL/index.ts; \
	if [[ "$(ARGS)" == "class_test" ]]; then \
		eval npx ts-node $$TS_TRAVERSAL/index.ts $(shell pwd)/$(ARGS)/$(ARGS).m $$EEGLAB/functions $$TS_TRAVERSAL 0 0; \
	else \
		eval npx ts-node $$TS_TRAVERSAL/index.ts $(shell pwd)/$(ARGS)/$(ARGS).m $(shell pwd)/$(ARGS) $$TS_TRAVERSAL 0 0; \
	fi; \
	eval npx ts-node $$TS_TRAVERSAL/matlabToRun.ts $(ARGS).m $$TS_TRAVERSAL; \
	
# Generate main.c for all mfiles that haven't already been generated
# eval npx ts-node $$TS_TRAVERSAL/index.ts $$fulldir/$$dir.m $$TEST $$TS_TRAVERSAL 0 0;
genall:
	for dir in $(all_dirs) ; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		if [ ! -f "$$fulldir/main.c" ]; then \
			echo $$dir; \
			eval npx ts-node $$TS_TRAVERSAL/index.ts $$fulldir/$$dir.m $$fulldir $$TS_TRAVERSAL 0 0; \
			eval npx ts-node $$TS_TRAVERSAL/matlabToRun.ts $$dir.m $$TS_TRAVERSAL; \
		fi \
	done; \
	
	
# Generate main.c for all mfiles, regardless of whether already generated
regenall:
	for dir in $(all_dirs) ; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		echo $$dir; \
		if [[ "$$dir" == "class_test" ]]; then \
			eval npx ts-node $$TS_TRAVERSAL/index.ts $$fulldir/$$dir.m $$EEGLAB/functions $$TS_TRAVERSAL 0 0; \
		else \
			eval npx ts-node $$TS_TRAVERSAL/index.ts $$fulldir/$$dir.m $$fulldir $$TS_TRAVERSAL 0 0; \
		fi; \
		eval npx ts-node $$TS_TRAVERSAL/matlabToRun.ts $$dir.m $$TS_TRAVERSAL; \
	done; \


# Compile all C programs
# Example: make compileall SPEED=1
compileall:
	dirs=$$(echo $(all_dirs)); \
	if [[ "$(SPEED)" == "1" ]]; then \
		echo "Compiling all nonspeed tests";\
		dirs=$$(echo $(nonspeed_dirs)); \
	elif [[ "$(SPEED)" == "2" ]]; then \
		echo "Compiling all speed tests";\
		dirs=$$(echo $(speed_dirs)); \
	fi; \
	eval $(CC) -c -o $(shell pwd)/convertSubscript.o $(shell pwd)/convertSubscript.c $(CFLAGS) $(LIB) $(INC); \
	eval $(CC) -c -o $(shell pwd)/unknownTypes.o $(shell pwd)/unknownTypes.c $(CFLAGS) $(LIB) $(INC); \
	good=(); \
	bad=(); \
	for dir in $$dirs; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		echo $$dir; \
		mfiles=$$(ls -d $$fulldir/* | grep -oP '^((?!_torun).)*\.m'); \
		srcs=$$(echo "$$mfiles" | sed "s/$$dir\.m/main/"); \
		srcs=$$(echo "$$srcs" | sed "s/\.m//"); \
		objects=$$(echo "$$mfiles" | sed "s/$$dir\.m/main.o/"); \
		objects=$$(echo "$$objects" | sed "s/\.m/.o/"); \
		objects+=" $(shell pwd)/convertSubscript.o";\
		objects+=" $(shell pwd)/unknownTypes.o";\
		for src in $$srcs; do \
			eval $(CC) -c -o $$src.o $$src.c $(CFLAGS) $(LIB) $(INC); \
		done; \
		eval $(CC) -o $$fulldir/test $$objects matrix.o $(CFLAGS) $(LIB) $(INC); \
		if $$($(CC) -o $$fulldir/test $$objects matrix.o $(CFLAGS) $(LIB) $(INC)); then \
			good+="$$dir\n"; \
		else \
			bad+="$$dir\n"; \
		fi; \
		if [ -f "$$fulldir/octavec_main.c" ]; then \
			eval $(CC) -c -o $$fulldir/octavec_main.o $$fulldir/octavec_main.c $(CFLAGS) $(LIB) $(INC); \
			eval $(CC) -o $$fulldir/check $$fulldir/octavec_main.o matrix.o $(CFLAGS) $(LIB) $(INC); \
		else \
			echo "Custom test"; \
		fi; \
	done; \
	echo -e "SUCCESS" > compile_log.txt; \
	echo -e $${good[@]} >> compile_log.txt; \
	echo -e "\nERROR" >> compile_log.txt; \
	echo -e $${bad[@]} >> compile_log.txt; \
	
# Compile one C program
# Example: make compileone ARGS=testname
compileone:
	fulldir="$(shell pwd)/$(ARGS)"; \
	mfiles=$$(ls -d $$fulldir/* | grep -oP '^((?!_torun).)*\.m'); \
	srcs=$$(echo "$$mfiles" | sed "s/$(ARGS)\.m/main/"); \
	srcs=$$(echo "$$srcs" | sed "s/\.m//"); \
	srcs+=" $(shell pwd)/convertSubscript";\
	srcs+=" $(shell pwd)/unknownTypes";\
	objects=$$(echo "$$mfiles" | sed "s/$(ARGS)\.m/main.o/"); \
	objects=$$(echo "$$objects" | sed "s/\.m/.o/"); \
	objects+=" $(shell pwd)/convertSubscript.o";\
	objects+=" $(shell pwd)/unknownTypes.o";\
	for src in $$srcs; do \
		eval $(CC) -c -o $$src.o $$src.c $(CFLAGS) $(LIB) $(INC); \
	done; \
	eval $(CC) -o $$fulldir/test $$objects matrix.o $(CFLAGS) $(LIB) $(INC); \
	if [ -f "$$fulldir/octavec_main.c" ]; then \
		eval $(CC) -c -o $$fulldir/octavec_main.o $$fulldir/octavec_main.c $(CFLAGS) $(LIB) $(INC); \
		eval $(CC) -o $$fulldir/check $$fulldir/octavec_main.o matrix.o $(CFLAGS) $(LIB) $(INC); \
	else \
		echo "Custom test"; \
	fi \
	
# Show MATLAB output for all mfiles
runmatlab:
	module load MATLAB/2021a; \
	mfiles=$$(find $(shell pwd) -type f -name "*.m"); \
	for mfile in $$mfiles; do \
		filename=$$(basename -- "$$mfile"); \
		echo $$filename; \
		matlab -nojvm -nodisplay -nosplash < $$mfile; \
	done; \
	module unload MATLAB/2021a; \
	
# Show MATLAB output for all mfiles
runmatlabone:
	module load MATLAB/2021a; \
	matlab -nojvm -nodisplay -nosplash < $(shell pwd)/$(ARGS)/$(ARGS)_torun.m; \
	module unload MATLAB/2021a; \
	

# Compare outputs of generated C program and original mfile for all tests
# Example: make compareall SPEED=1
compareall:
	module load MATLAB/2021a; \
	dirs=$$(echo $(all_dirs)); \
	if [[ "$(SPEED)" == "1" ]]; then \
		echo "Comparing all nonspeed tests";\
		dirs=$$(echo $(nonspeed_dirs)); \
	elif [[ "$(SPEED)" == "2" ]]; then \
		echo "Comparing all speed tests";\
		dirs=$$(echo $(speed_dirs)); \
	fi; \
	good=(); \
	bad=(); \
	for dir in $$dirs; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		echo $$dir; \
		(timeout 120 $$fulldir/test 2>/dev/null | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/inf +infi/inf/gi' | sed -z 's/inf + infi/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/\+0i//g' | sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f1; \
		timeout 120 matlab -nodisplay -nosplash < $$fulldir/matlabToRun/$${dir}_torun.m; \
		(cat $$fulldir/output.txt 2>/dev/null | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/\+0i//g' | \
			sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f2; \
		timeout 120 diff -wi $$fulldir/f1 $$fulldir/f2; \
		RESULT=$$?; \
		if [[ "$$RESULT" == "0" ]]; then \
			echo -e "\e[1;32mSUCCESS\e[0m : $$dir";\
			good+="$$dir\n"; \
		else \
			echo -e "\e[1;31mDIFF ERROR\e[0m : $$dir";\
			bad+="$$dir\n";\
		fi; \
		rm $$fulldir/f1; \
		rm $$fulldir/f2; \
		rm $$fulldir/output.txt; \
	done; \
	echo -e "SUCCESS" > compare_log.txt; \
	echo -e $${good[@]} >> compare_log.txt; \
	echo -e "\nERROR" >> compare_log.txt; \
	echo -e $${bad[@]} >> compare_log.txt; \
	module unload MATLAB/2021a; \
	
# Compare outputs of generated C program and original mfile for one test
# Example: make compareone ARGS=testname
compareone:
	module load MATLAB/2021a; \
	fulldir="$(shell pwd)/$(ARGS)"; \
	($$fulldir/test 2>/dev/null | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/inf +infi/inf/gi' | sed -z 's/inf + infi/inf/gi' | \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/\+0i//g' | sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f1; \
	matlab -nodisplay -nosplash < $$fulldir/matlabToRun/$(ARGS)_torun.m; \
	(cat $$fulldir/output.txt 2>/dev/null | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/Inf/inf/gi'| \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/\+0i//g' | sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f2; \
	diff -wi $$fulldir/f1 $$fulldir/f2 && \
		echo -e "\e[1;32mSUCCESS\e[0m : $(ARGS)"||\
		echo -e "\e[1;31mDIFF ERROR\e[0m : $(ARGS)";\
	module unload MATLAB/2021a; \
	
# Compare outputs of generated C program and original mfile for one test
# Example: make compareone ARGS=testname
compareone2:
	module load MATLAB/2021a; \
	fulldir="$(shell pwd)/$(ARGS)"; \
	($$fulldir/test 2>/dev/null | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/inf +infi/inf/gi' | sed -z 's/inf + infi/inf/gi' | \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/\+0i//g' | sed -z 's/\n\s/\n/gi' | \
		grep "\S" | sed -z -E 's/\t/ /gi' | sed -z -E 's/ +/ /gi') > $$fulldir/f1; \
	matlab -nodisplay -nosplash < $$fulldir/matlabToRun/$(ARGS)_torun.m; \
	(cat $$fulldir/output.txt 2>/dev/null | \
		sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/Inf/inf/gi'| \
		sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
		tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
		sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
		sed -z -E 's/\+0i//g' | sed -z 's/\n\s/\n/gi' | \
		grep "\S" | sed -z 's/\t/ /gi' | sed -z 's/ +/ /gi') > $$fulldir/f2; \
	diff -wi $$fulldir/f1 $$fulldir/f2 && \
		echo -e "\e[1;32mSUCCESS\e[0m : $(ARGS)"||\
		echo -e "\e[1;31mDIFF ERROR\e[0m : $(ARGS)";\
	module unload MATLAB/2021a; \
	

# OLD STUFF

testing2:
	for dir in $(all_dirs) ; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		rm $$fulldir/$${dir}_torun.m; \
	done; \

# For testing
testing:
	for dir in $(all_dirs) ; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		mfiles=$$(ls $$fulldir | grep -oP '^((?!_torun).)*\.m'); \
		mfiles2=$$(ls -d $$fulldir/* | grep -oP '^((?!_torun).)*\.m'); \
		mfiles3=$$(find $$fulldir -type f -name "*.m"); \
		echo $$mfiles2; \
	done; \

# Compare outputs of generated C program and original mfile for all tests
# add argument to ignore speed tests
compareall2:
	module load MATLAB/2021a; \
	dirs=$$(echo $(all_dirs)); \
	if [[ "$(SPEED)" == "1" ]]; then \
		echo "Comparing all nonspeed tests";\
		dirs=$$(echo $(nonspeed_dirs)); \
	elif [[ "$(SPEED)" == "2" ]]; then \
		echo "Comparing all speed tests";\
		dirs=$$(echo $(speed_dirs)); \
	fi; \
	good=(); \
	bad=(); \
	for dir in $$dirs; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		echo $$dir; \
		(timeout 120 $$fulldir/test 2>/dev/null | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/inf +infi/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') > $$fulldir/f1; \
		(timeout 120 matlab -nodisplay -nosplash < $$fulldir/$${dir}_torun.m 2>/dev/null | \
			tail -n+10 | sed -z 's/>>//gi' | \
			sed -E "s/\"@(-?[0-9]+(\.[0-9]+)?( [+-] ?[0-9]+(\.[0-9]+)?i)?)\"/\1/g" | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f2; \
		timeout 120 diff -wi $$fulldir/f1 $$fulldir/f2 && \
			(echo -e "\e[1;32mSUCCESS\e[0m : $$dir" && good+="$$dir\n")||\
			(echo -e "\e[1;31mDIFF ERROR\e[0m : $$dir" && bad+="$$dir\n");\
		rm $$fulldir/f1; \
		rm $$fulldir/f2; \
	done; \
	echo -e "SUCCESS" > compare_log.txt; \
	echo -e $${good[@]} >> compare_log.txt; \
	echo -e "\nERROR" >> compare_log.txt; \
	echo -e $${bad[@]} >> compare_log.txt; \
	module unload MATLAB/2021a; \
	
# Compare outputs of generated C program and original mfile for all tests
# add argument to ignore speed tests
compareall3:
	module load MATLAB/2021a; \
	dirs=$$(echo $(all_dirs)); \
	if [[ "$(SPEED)" == "1" ]]; then \
		echo "Comparing all nonspeed tests";\
		dirs=$$(echo $(nonspeed_dirs)); \
	elif [[ "$(SPEED)" == "2" ]]; then \
		echo "Comparing all speed tests";\
		dirs=$$(echo $(speed_dirs)); \
	fi; \
	good=(); \
	bad=(); \
	for dir in $$dirs; do \
		dir=$${dir//\//}; \
		fulldir="$(shell pwd)/$$dir"; \
		echo $$dir; \
		(timeout 120 $$fulldir/test 2>/dev/null | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | sed -z 's/inf +infi/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g') > $$fulldir/f1; \
		(timeout 120 matlab -nodisplay -nosplash < $$fulldir/$${dir}_torun.m 2>/dev/null | \
			tail -n+10 | sed -z 's/>>//gi' | \
			sed -E "s/\"@(-?[0-9]+(\.[0-9]+)?( [+-] ?[0-9]+(\.[0-9]+)?i)?)\"/\1/g" | \
			sed -z 's/-nan/inf/gi' | sed -z 's/nan/inf/gi' | \
			sed -z 's/\.000000//g' | sed -z 's/\.00000//g' | sed -z 's/\.0000//g' | \
			tr -s ' \t\n' | sed -z 's/\n/ \n/g' | \
			sed -z -E 's/-\s*(0+(\.0+)?(-|\+|\s))/\1/g' | sed -z -E 's/-\s*(0+(\.0+)?i)/+\1/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z -E 's/(\.[[:digit:]]+)0+(\-|\+|i|\s|\n)/\1\2/g' | \
			sed -z 's/\n\s/\n/gi' | grep "\S") > $$fulldir/f2; \
		if $$(timeout 120 diff -wi $$fulldir/f1 $$fulldir/f2); then \
			echo -e "\e[1;32mSUCCESS\e[0m : $$dir";\
			good+="$$dir\n"; \
		else \
			echo -e "\e[1;31mDIFF ERROR\e[0m : $$dir";\
			bad+="$$dir\n";\
		fi; \
		rm $$fulldir/f1; \
		rm $$fulldir/f2; \
	done; \
	echo -e "SUCCESS" > compare_log.txt; \
	echo -e $${good[@]} >> compare_log.txt; \
	echo -e "\nERROR" >> compare_log.txt; \
	echo -e $${bad[@]} >> compare_log.txt; \
	module unload MATLAB/2021a; \
