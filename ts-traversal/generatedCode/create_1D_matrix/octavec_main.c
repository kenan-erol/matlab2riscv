//
//  create_1D_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Creates a 1D row matrix
int main() 
{
	bool flag = true;

	// Set up the matrix metadata
	int ndim = 1;
	int dim[1] = {3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create an uninitialized double matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Write to the double matrix
	double *input = NULL;
	input = malloc(size*sizeof(*input));
	input[0] = 1;
	input[1] = 4;
	input[2] = 9.5;
	writeM(tmp, size, input);
	free(input);

	// Print out the matrix
	printM(tmp);

	// Clean up
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}
