//
//  ceil_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/25/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Takes the element-wise ceiling of a matrix
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Make a complex matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

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

	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	// Get the ceiling of the matrix
	Matrix *tmp2 = ceilM(tmp);
	printM(tmp2);

	if(isEqualM(tmp, tmp2))
	{
		fprintf(stdout, "FAILURE\n");
	}

	// The ceiling of an int matrix is itself
	Matrix *tmp3 = identityM(8);
	Matrix *tmp4 = ceilM(tmp3);
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
