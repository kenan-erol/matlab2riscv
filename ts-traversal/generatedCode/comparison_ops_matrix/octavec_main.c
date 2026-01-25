//
//  comparison_ops_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Elementwise less than (<) comparison between two matrices
int lt_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);


	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	input[0] -= 1;
	input[4] -= 1;
	input[8] -= 1;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = (i+1)*(i+1);
	}
	input2[4] = 25.5;
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);




	Matrix *tmp3 = ltM(tmp, tmp2);
	printM(tmp3);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "FAILURE\n");
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Elementwise less than or equal to (<=) comparison between two matrices
int le_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);


	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1)+1;
	}
	input[0] -= 1;
	input[4] -= 1;
	input[8] -= 1;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = (i+1)*(i+1);
	}
	input2[4] = 25.5;
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the matrices
	Matrix *tmp3 = leM(tmp, tmp2);
	printM(tmp3);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "FAILURE\n");
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Elementwise greater than (>) comparison between two matrices
int gt_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);

	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	input[0] += 1;
	input[4] += 1;
	input[8] += 1;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the matrices
	Matrix *tmp3 = gtM(tmp, tmp2);
	printM(tmp3);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "FAILURE\n");
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Elementwise greater than or equal to (>=) comparison between two matrices
int ge_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);

	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1)-1;
	}
	input[0] += 1;
	input[4] += 1;
	input[8] += 1;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = (i+1)*(i+1);
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the two matrices
	Matrix *tmp3 = geM(tmp, tmp2);
	printM(tmp3);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "FAILURE\n");
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Elementwise not equal to (!=) comparison between two matrices
int ne_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *id3 = identityM(3);

	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	input[0] *= 2;
	input[8] *= 9;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = (i+1)*(i+1);
	}
	input2[4] = 25.5;
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the two matrices
	Matrix *tmp3 = neM(tmp, tmp2);
	printM(tmp3);

	if (!isEqualM(tmp3, id3))
	{
		fprintf(stdout, "FAILURE\n");
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

	if (!destroyM(id3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Elementwise max between two matrices
int max_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	input[0] *= 2;
	input[8] *= 19;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double complex *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = ((double complex)((i+1)*(i+1))) + ((double complex)(4))*I;
	}
	input2[4] = 25.5;
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the two matrices
	Matrix *tmp3 = pairwise_maxM(tmp, tmp2);
	printM(tmp3);
	
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
	
	return(flag);
}

// Elementwise min between two matrices
int min_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create the first matrix
	int *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	input[0] *= 2;
	input[8] *= 19;
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
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
	double complex *input2 = NULL;
	input2 = malloc(size*sizeof(*input2));
	for (int i = 0 ; i < size ; i++)
	{
		input2[i] = ((double complex)((i+1)*(i+1))) + ((double complex)(4))*I;
	}
	input2[4] = 25.5;
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, size, input2);
	free(input2);
	printM(tmp2);



	// Compare the two matrices
	Matrix *tmp3 = pairwise_minM(tmp, tmp2);
	printM(tmp3);
	
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
	
	return(flag);
}

// Try a bunch of different inputs
int brutal_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex z = 4.2 - 0.03*I;

	int totalMatrices = 12;
	Matrix **matrices = NULL;
	matrices = malloc(totalMatrices*sizeof(*matrices));

	matrices[0] = zerosM(ndim, dim);
	matrices[1] = onesM(ndim, dim);
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
		input6[i] = ((double complex)((i+1)*(i+1))) + ((double complex)(0.5))*I;
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
		input9[i] = ((double complex)((i-4.89)*(i+1))) + ((double complex)(((0.5)*(3-i))))*I;
	}
	writeM(matrices[9], 9, input9);
	free(input9);

	matrices[10] = createM(ndim, dim, INTEGER);
	int input10[] = {3,-2,0,   4,-1,0,   0,0,1};
	writeM(matrices[10], 9, input10);

	matrices[11] = createM(ndim, dim, DOUBLE);
	double input11[] = {
		11.25, -7.525, -1.45,     
		11, -6.9, -2.2,    
		5.5, -5.45, 2.9};
	writeM(matrices[11], 9, input11);


	// Try it ALL
	for (int i = 0 ; i < 12 ; i++)
	{
		for (int j = 0 ; j < 12 ; j++)
		{
			Matrix **matrix_comps = NULL;
			matrix_comps = malloc(8*sizeof(*matrix_comps));

			matrix_comps[0] = ltM(matrices[i], matrices[j]);
			matrix_comps[1] = leM(matrices[i], matrices[j]);
			matrix_comps[2] = gtM(matrices[i], matrices[j]);
			matrix_comps[3] = geM(matrices[i], matrices[j]);
			matrix_comps[4] = neM(matrices[i], matrices[j]);
			matrix_comps[5] = eqM(matrices[i], matrices[j]);
			matrix_comps[6] = pairwise_maxM(matrices[i], matrices[j]);
			matrix_comps[7] = pairwise_minM(matrices[i], matrices[j]);

			for (int index = 0 ; index < 8 ; index++)
			{
				printM(matrix_comps[index]);
			}

			for (int index = 0 ; index < 8 ; index++)
			{
				if (!destroyM(matrix_comps[index]))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
			}
			free(matrix_comps);
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
	
	return(flag);
}

int main()
{
	bool flag = true;

	// if(!lt_test()){flag = false;}
	// if(!le_test()){flag = false;}
	// if(!gt_test()){flag = false;}
	// if(!ge_test()){flag = false;}
	// if(!ne_test()){flag = false;}
	// if(!max_test()){flag = false;}
	// if(!min_test()){flag = false;}
	if(!brutal_test()){flag = false;}

	return(flag);
}
