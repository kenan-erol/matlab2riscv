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

# Example: make updategithub MESSAGE=Fixed bugs, updated makefile
updategithub:
	git pull origin master; \
	git add -A && git commit -m "$(MESSAGE)"; \
	git push origin master; \
	
