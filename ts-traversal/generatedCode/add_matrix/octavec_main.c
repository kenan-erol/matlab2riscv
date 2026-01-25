//
//  add_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/25/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>


// Adds two matrices
int normalTest() 
{
	bool flag = true; 

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create a 3x3 identity matrix
	Matrix *id3 = identityM(3);

	// Create the first matrix
	double complex *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = 0.5;
	}
	input[0] = 0.5 + I;
	input[4] = 0.5 + I;
	input[8] = 0.5 + I;

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, size, input);
	free(input);
	printM(tmp);

	// Create the second matrix
	double complex input2[] = {
		0.5 - I, -0.5, -0.5,
		-0.5, 0.5 - I, -0.5,
		-0.5, -0.5, 0.5 - I};

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}
	writeM(tmp2, size, input2);
	printM(tmp2);

	Matrix *tmp3 = plusM(tmp, tmp2);
	printM(tmp3);

	Matrix *tmp4 = plusM(tmp3, tmp3);
	printM(tmp4);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "Not equal!\n");
		flag = false;
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Adds two int matrices, and converts the result to a double if there's an overflow
int overflowTest() 
{
	bool flag = true; 

	// Create a 3x3 identity matrix
	Matrix *id3 = identityM(3);

	// Adding two integer matrices will output an integer matrix, unless there's an int overflow
	int max = INT_MAX;
	Matrix *overflow_check = scaleM(id3, &max, INTEGER);
	Matrix *overflow_check2 = plusM(overflow_check, id3);
	printM(overflow_check2);

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(overflow_check))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(overflow_check2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int main()
{
	bool flag = true;
	if(!normalTest()){flag = false;}
	if(!overflowTest()){flag = false;}
	return(flag);
}
