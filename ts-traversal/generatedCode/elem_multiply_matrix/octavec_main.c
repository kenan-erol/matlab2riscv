//
//  elem_multiply_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/25/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Elementwise multiplication of two matrices
int main() 
{
	bool flag = true; 

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *id3 = identityM(3);



	// Create a matrix
	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = 0;
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
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);





	// Create a second matrix
	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = 0;
	}
	input2[0] = 0.4 - 0.8*I;
	input2[4] = 0.4 - 0.8*I;
	input2[8] = 0.4 - 0.8*I;

	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);
	printM(tmp2);


	// Multiply the matrices
	Matrix *tmp3 = timesM(tmp, tmp2);
	printM(tmp3);

	Matrix *tmp4 = timesM(id3, tmp);
	printM(tmp4);



	// Overflow checking
	int max = INT_MAX;
	int two = 2;
	Matrix *overflow_check = scaleM(id3, &max, INTEGER);
	Matrix *overflow_check1 = scaleM(id3, &two, INTEGER);
	Matrix *overflow_check2 = timesM(overflow_check, overflow_check1);
	printM(overflow_check2);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stderr, "Not equal!\n");
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

	if (!destroyM(overflow_check))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(overflow_check1))
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
