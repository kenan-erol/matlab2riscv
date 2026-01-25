//
//  elem_power_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/26/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Raises an integer matrix to an integer matrix, elementwise
int ii_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};


	// Create an integer matrix
	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	// Pre-compute the result for error checking
	int *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1), (i+1));
	}
	
	Matrix *tmp2 = createM(ndim, dim, INTEGER);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);
	printM(tmp);


	// Raise the matrix
	Matrix *tmp3 = powerM(tmp, tmp);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises an integer matrix to a double matrix, elementwise
int id_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	double *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4;
	}

	Matrix *tmp1 = createM(ndim, dim, DOUBLE);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);



	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1), (i+1)+0.4);
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);

	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises an integer matrix to a double matrix (with negatives), elementwise
int neg_id_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = -(i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	double *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4;
	}

	Matrix *tmp1 = createM(ndim, dim, DOUBLE);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);



	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow(-(i+1), (i+1)+0.4);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);

	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises an integer matrix to a complex matrix, elementwise
int ic_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1);
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	double complex *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4*I;
	}

	Matrix *tmp1 = createM(ndim, dim, COMPLEX);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);



	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1), (i+1)+0.4*I);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);

	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a double matrix to an integer matrix, elementwise
int di_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.4;
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	int *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1);
	}

	Matrix *tmp1 = createM(ndim, dim, INTEGER);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+0.4, (i+1));
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);


	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a double matrix to a double matrix, elementwise
int dd_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.4;
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);
	printM(tmp);


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+0.4, (i+1)+0.4);
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);


	Matrix *tmp3 = powerM(tmp, tmp);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a double matrix to a double matrix (with negatives), elementwise
int neg_dd_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = -((i+1)+0.4);
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);
	printM(tmp);


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow(-((i+1)+0.4), (i+1)+0.4);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);


	Matrix *tmp3 = powerM(tmp, tmp);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a double matrix to a complex matrix, elementwise
int dc_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.4;
	}
	
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	double complex *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4*I;
	}

	Matrix *tmp1 = createM(ndim, dim, COMPLEX);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);



	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+0.4, (i+1)+0.4*I);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);

	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a complex matrix to an integer matrix, elementwise
int ci_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+I;
	}
	
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


	int *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1);
	}
	
	Matrix *tmp1 = createM(ndim, dim, INTEGER);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+I, (i+1));
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);


	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a complex matrix to a double matrix, elementwise
int cd_test() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.5*I;
	}
	
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


	double *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4;
	}
	
	Matrix *tmp1 = createM(ndim, dim, DOUBLE);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+0.5*I, (i+1)+0.4);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);


	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises a complex matrix to a complex matrix, elementwise
int cc_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)+0.4*I;
	}
	
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


	double complex *input1 = NULL;
	input1 = malloc(9*sizeof(*input1));
	for (int i = 0 ; i < 9 ; i++)
	{
		input1[i] = (i+1)+0.4*I;
	}

	Matrix *tmp1 = createM(ndim, dim, COMPLEX);
	if ( tmp1 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input1);
		return(false);
	}
	writeM(tmp1, 9, input1);
	free(input1);
	printM(tmp1);



	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1)+0.4*I, (i+1)+0.4*I);
	}
	
	Matrix *tmp2 = createM(ndim, dim, COMPLEX);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);

	Matrix *tmp3 = powerM(tmp, tmp1);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp1))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

// Raises an int matrix to an int matrix, with overflow
int overflow_test()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = INT_MAX;
	}
	
	Matrix *tmp = createM(ndim, dim, INTEGER);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow(INT_MAX, 2);
	}
	
	Matrix *tmp2 = createM(ndim, dim, DOUBLE);
	if ( tmp2 == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input2);
		return(false);
	}
	writeM(tmp2, 9, input2);
	free(input2);
	printM(tmp);

	int two = 2;
	Matrix *ones = onesM(ndim, dim);
	Matrix *twos = scaleM(ones, &two, INTEGER);

	Matrix *tmp3 = powerM(tmp, twos);
	printM(tmp3);

	if (!isEqualM(tmp2, tmp3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp2))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(tmp3))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(ones))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	if (!destroyM(twos))
	{
		flag = false;
		fprintf(stderr, "Failed to destroy matrix\n");
	}

	return(flag);
}

int main()
{
	bool flag = true;

	if(!ii_test()){flag = false;}
	if(!id_test()){flag = false;}
	if(!neg_id_test()){flag = false;}
	if(!ic_test()){flag = false;}

	if(!di_test()){flag = false;}
	if(!dd_test()){flag = false;}
	if(!neg_dd_test()){flag = false;}
	if(!dc_test()){flag = false;}

	if(!ci_test()){flag = false;}
	if(!cd_test()){flag = false;}
	if(!cc_test()){flag = false;}

	if(!overflow_test()){flag = false;}

	return(flag);
}