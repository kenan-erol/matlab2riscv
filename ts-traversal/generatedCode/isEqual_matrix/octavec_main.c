//
//  isEqual_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Checks that two matrices are equal (boolean)
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
	free(input);

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

	// Check that the first matrix does not equal the identity matrix
	Matrix *tmp3 = identityM(3);
	if(isEqualM(tmp, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
	}

	// Create a complex identity matrix & check equality with the normal (int) identity matrix
	Matrix *tmp4 = createM(ndim, dim, COMPLEX);
	if (tmp4 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}
	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 3 ; i++)
	{
		for (int j = 0 ; j < 3 ; j++)
		{
			input2[3*i+j] = 0;
			if (i == j)
			{
				input2[3*i+j] = 1;
			}
		}
	}
	if(!writeM(tmp4, size, input2))
	{
		fprintf(stderr, "Failed to write to matrix\n");
	}
	free(input2);
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

	return(flag);
}
