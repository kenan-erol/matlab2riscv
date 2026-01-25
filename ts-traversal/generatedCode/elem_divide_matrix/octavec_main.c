//
//  elem_divide_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/25/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Elementwise division of two matrices
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
		input[i] = 1;
	}
	input[0] = -0.75 + I;
	input[4] = -0.75 + I;
	input[8] = -0.75 + I;

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
		input2[i] = 1;
	}
	input2[0] = 0.5 + I;
	input2[4] = 0.5 + I;
	input2[8] = 0.5 + I;

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


	// Right division
	Matrix *tmp3 = rdivideM(tmp, tmp2);
	printM(tmp3);

	Matrix *tmp4 = rdivideM(id3, tmp);
	printM(tmp4);

	Matrix *tmp5 = rdivideM(tmp2, id3);
	printM(tmp5);


	// Left division
	Matrix *tmp6 = ldivideM(tmp, tmp2);
	printM(tmp6);

	Matrix *tmp7 = ldivideM(id3, tmp);
	printM(tmp7);

	Matrix *tmp8 = ldivideM(tmp2, id3);
	printM(tmp8);

	if (!isEqualM(tmp3, tmp2))
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

	if (!destroyM(tmp5))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp6))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp7))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp8))
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