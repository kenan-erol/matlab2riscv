//
//  SPEED_small_matrix_transpose.c
//  Octave-C
//
//  Created by Zach Taylor on 8/18/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Speedtest: Transpose a small double matrix
int main() 
{
	bool flag = true;

	// Initialize matrix metadata
	int ndim = 2;
	int dim[2] = {2,2};

	// Initialize matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Create the matrix data and write it
	int size = 4;
	double *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5;
	}

	writeM(tmp, size, input);
	free(input); // writeM copies the input into the matrix, so it can be freed immediately
	printM(tmp);

	// The meat of the speed test
	int iterations = 1000000;
	Matrix **invs = NULL;
	invs = malloc(iterations*sizeof(*invs));

	// Get the eigenvalues and vectors of the matrix
	for (int i = 0 ; i < iterations ; i++)
	{
		invs[i] = transposeM(tmp);
	}

	// Clean up all the product matrices created in the loop
	for (int i = 0 ; i < iterations ; i++)
	{
		if (!destroyM(invs[i]))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
	}
	free(invs);

	// Clean up the original matrices
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}