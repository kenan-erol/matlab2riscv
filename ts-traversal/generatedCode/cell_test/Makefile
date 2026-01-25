OCTAVEC=/home/dlg59/project/Halo-Algorithm/OctaveC

INC = -I. -I${OCTAVEC} -I${EBROOTOPENBLAS}/include -I${EBROOTFFTW}
LIB = -L${EBROOTOPENBLAS}/lib -L${EBROOTSCALAPACK}/lib -L${EBROOTFFTW}/lib
VPATH = .:${OCTAVEC}

CC=gcc
CFLAGS = -lm -lopenblas -lscalapack -lfftw3
DEPS = main.h

# Compile automatically generated main file from tstraversal
OBJ = main.o matrix.o 
# Also compile test from octavec to compare and check output
OBJ2 = octavec_main.o matrix.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIB) $(INC)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB) $(INC)
	
check: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB) $(INC)