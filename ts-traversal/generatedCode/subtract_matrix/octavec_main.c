//
//  abs_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/25/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Subtracts a matrix
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
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);




	// Create a second matrix
	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	input2[0] = -0.5 + I;
	input2[1] = 0.5;
	input2[2] = 0.5;
	input2[3] = 0.5;
	input2[4] = -0.5 + I;
	input2[5] = 0.5;
	input2[6] = 0.5;
	input2[7] = 0.5;
	input2[8] = -0.5 + I;

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
	

	// Subtraction
	Matrix *tmp3 = minusM(tmp, tmp2);
	printM(tmp3);

	Matrix *tmp4 = minusM(id3, tmp);
	printM(tmp4);


	// Overflow checking
	int min = INT_MIN;
	Matrix *overflow_check = scaleM(id3, &min, INTEGER);
	Matrix *overflow_check2 = minusM(overflow_check, id3);
	printM(overflow_check2);

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
