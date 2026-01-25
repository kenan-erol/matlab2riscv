//
//  eigen.c
//  Octave-C
//
//  Created by Zach Taylor on 7/19/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int main()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex z = 4.2 - 0.03*I;
	int two = 2;
	Matrix *id = idM(3);

	int totalMatrices = 15;
	Matrix **matrices = NULL;
	matrices = malloc(totalMatrices*sizeof(*matrices));

	matrices[0] = zerosM(ndim, dim);
	matrices[1] = scaleM(id, &two, INTEGER);
	matrices[2] = idM(3);
	matrices[3] = scaleM(matrices[2], &z, COMPLEX);


	matrices[4] = createM(ndim, dim, INTEGER);
	int *input4 = NULL;
	input4 = malloc(9*sizeof(*input4));
	for (int i = 0 ; i < 9 ; i++)
	{
		input4[i] = (i+1)*(i+1);
	}
	writeM(matrices[4], 9, input4);
	free(input4);

	matrices[5] = createM(ndim, dim, DOUBLE);
	double *input5 = NULL;
	input5 = malloc(9*sizeof(*input5));
	for (int i = 0 ; i < 9 ; i++)
	{
		input5[i] = (i+1)*(i+1)+0.5;
	}
	writeM(matrices[5], 9, input5);
	free(input5);

	matrices[6] = createM(ndim, dim, COMPLEX);
	double complex *input6 = NULL;
	input6 = malloc(9*sizeof(*input6));
	for (int i = 0 ; i < 9 ; i++)
	{
		input6[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(matrices[6], 9, input6);
	free(input6);

	matrices[7] = createM(ndim, dim, INTEGER);
	int *input7 = NULL;
	input7 = malloc(9*sizeof(*input7));
	for (int i = 0 ; i < 9 ; i++)
	{
		input7[i] = (i-4)*(i+1);
	}
	writeM(matrices[7], 9, input7);
	free(input7);

	matrices[8] = createM(ndim, dim, DOUBLE);
	double *input8 = NULL;
	input8 = malloc(9*sizeof(*input8));
	for (int i = 0 ; i < 9 ; i++)
	{
		input8[i] = (i-7.2)*(i+1)+0.5;
	}
	writeM(matrices[8], 9, input8);
	free(input8);

	matrices[9] = createM(ndim, dim, COMPLEX);
	double complex *input9 = NULL;
	input9 = malloc(9*sizeof(*input9));
	for (int i = 0 ; i < 9 ; i++)
	{
		input9[i] = (i-4.89)*(i+1)+((0.5)*(3-i))*I;
	}
	writeM(matrices[9], 9, input9);
	free(input9);

	matrices[10] = createM(ndim, dim, DOUBLE);
	double input10[] = {3,-2,0,   4,-1,0,   0,0,1};
	writeM(matrices[10], 9, input10);

	matrices[11] = createM(ndim, dim, DOUBLE);
	double input11[] = {
		11.25, -7.525, -1.45,     
		11, -6.9, -2.2,    
		5.5, -5.45, 2.9};
	writeM(matrices[11], 9, input11);

	matrices[12] = createM(ndim, dim, DOUBLE);
	double *input12 = NULL;
	input12 = malloc(9*sizeof(*input12));
	int one = -1;
	for (int i = 0 ; i < 9 ; i++)
	{
		input12[i] = one*(i+1)*(i+1);
		one *= -1;
	}
	writeM(matrices[12], 9, input12);
	free(input12);

	// Non-diagonalizeable matrices
	matrices[13] = createM(ndim, dim, INTEGER);
	int input13[] = {1,1,0,   0,1,0,   0,0,0};
	writeM(matrices[13], 9, input13);

	matrices[14] = createM(ndim, dim, INTEGER);
	int input14[] = {3,4,3,   -1,0,-1,   1,2,3};
	writeM(matrices[14], 9, input14);


	// Try it ALL
	double complex complex_one = 1;
	for (int index = 0 ; index < 14 ; index++)
	{
		fprintf(stdout, "Original\n");
		Matrix *complex_mat = scaleM(matrices[index], &complex_one, COMPLEX);
		printM(complex_mat);
		if (!destroyM(complex_mat))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		Matrix *evals = NULL;
		Matrix *evecs = NULL;
		eigM(matrices[index], &evals, &evecs);

		fprintf(stdout, "Eigenvalues:\n");
		Matrix *complex_evals = scaleM(evals, &complex_one, COMPLEX);
		printM(complex_evals);
		if (!destroyM(complex_evals))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		fprintf(stdout, "Eigenvectors:\n");
		Matrix *complex_evecs = scaleM(evecs, &complex_one, COMPLEX);
		printM(complex_evecs);
		if (!destroyM(complex_evecs))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		Matrix *evecs_inv = invM(evecs);
		Matrix *first = mtimesM(evecs, evals);
		Matrix *sanity_check = mtimesM(first, evecs_inv);

		if (!isEqualM(matrices[index], sanity_check) && index < 13)
		{
			fprintf(stdout, "FAILURE\n");
			flag = false;
		}

		if (!destroyM(evals))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		if (!destroyM(evecs))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		if (!destroyM(evecs_inv))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		if (!destroyM(first))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}

		if (!destroyM(sanity_check))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}			
	}

	for (int index = 0 ; index < totalMatrices ; index++)
	{
		if (!destroyM(matrices[index]))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
	}
	free(matrices);
	if (!destroyM(id))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}