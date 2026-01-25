//
//  reshape_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Reshapes a matrix
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



	// Create a 3x3 matrix
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

	input[6] = -0.0002-0.1*I;
	input[7] = -100.01*I;
	input[8] = 81;

	writeM(tmp, size, input);
	free(input);
	printM(tmp);



	// Reshape into a 9x1 matrix
	int second_dims[2] = {9,1};
	if(!reshapeM(tmp, 2, second_dims))
	{
		fprintf(stderr, "Failed to resize matrix\n");
		flag = false;
	}
	printM(tmp);

	// Fail if you try to reshape it invalidly
	int third_dims[2] = {4,9};
	if(reshapeM(tmp, 2, third_dims))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}
	printM(tmp);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}
