//
//  main.c
//  Octave-C
//
//  Created by Zach Taylor on 12/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "src/tgmath.h"
    
int main(int argc, char *argv[]) 
{
    printf("Hello World!\n");

    fprintf(stdout,"int size: %ld\n", sizeof(int));
    fprintf(stdout,"double size: %ld\n", sizeof(double));
    fprintf(stdout,"complex size: %ld\n", sizeof(double complex));

    return 1;
}
