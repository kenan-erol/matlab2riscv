//
//  logic_ops_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Checks the elementwise logic operations (and, or, not)
int main()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};


	// Create a matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

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

	writeM(tmp, size, input);
	free(input);
	printM(tmp);




	// NOT
	Matrix *tmp_n = notM(tmp);
	printM(tmp_n);

	Matrix *tmp1 = zeroesM(ndim, dim);
	if (tmp1 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	Matrix *tmp1_n = notM(tmp1);
	printM(tmp1_n);

	Matrix *tmp2 = onesM(ndim, dim);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	Matrix *tmp2_n = notM(tmp2);
	printM(tmp2_n);

	// Not of ones == zeros
	if(!isEqualM(tmp1, tmp2_n))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	// Not of zeros == ones
	if(!isEqualM(tmp1_n, tmp2))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	Matrix *tmp3 = identityM(3);
	Matrix *tmp3_n = notM(tmp3);

	printM(tmp3_n);


	// AND
	Matrix *and_01 = andM(tmp, tmp1);
	printM(and_01);

	Matrix *and_03n = andM(tmp, tmp3_n);
	printM(and_03n);



	// OR
	Matrix *or_12n = orM(tmp1, tmp2_n);
	printM(or_12n);

	Matrix *or_2n3 = orM(tmp2_n, tmp3);
	printM(or_2n3);


	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp_n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp1))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp1_n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp2_n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp3_n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(and_01))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(and_03n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(or_12n))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(or_2n3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}
