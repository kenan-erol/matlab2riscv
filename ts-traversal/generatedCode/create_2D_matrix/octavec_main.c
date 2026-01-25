//
//  create_2D_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Creates a 2D matrix
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(size*sizeof(*input));
	input[0] = 0;
	input[1] = 10;

	input[2] = 10*I;
	input[3] = 10.102;

	input[4] = 10.102 + 0.5*I;
	input[5] = -12*I;

	writeM(tmp, size, input);
	printM(tmp);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	free(input);
	return(flag);
}
