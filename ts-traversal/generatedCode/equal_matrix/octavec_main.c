//
//  equal_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Elementwise equality (==) of two matrices
int main() 
{
	bool flag = true;

	// Create a 3x3 matrix
	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Give the matrix some values
	int size = 9;
	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	input[0] = 0;
	input[1] = 10;
	input[2] = 10*I;

	input[3] = 10.102;
	input[4] = 10.102 + 0.5*I;
	input[5] = -12*I;

	input[6] = -0.0002-0.1*I;
	input[7] = -100.01*I;
	input[8] = 81;

	if(!writeM(tmp, size, input))
	{
		fprintf(stderr, "Failed to write to matrix\n");
	}

	// Print out the matrix
	printM(tmp);

	// Duplicate the matrix & print it out
	Matrix *tmp2 = duplicateM(tmp);
	printM(tmp2);

	// Check equality between the matrices
	if(!isEqualM(tmp, tmp2))
	{
		fprintf(stdout, "FAILURE\n");
	}

	// Get a matrix of element-wise comparison between the matrices
	// This should return a matrix of all 1's if functioning properly
	Matrix *tmp3 = equalM(tmp, tmp2);

	printM(tmp3);

	// Create a complex identity matrix & check equality with the normal (int) identity matrix
	Matrix *tmp4 = onesM(ndim, dim);
	if (tmp4 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	if(!isEqualM(tmp3, tmp4))
	{
		fprintf(stdout, "FAILURE\n");
	}


	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp4))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	free(input);
	return(flag);
}
