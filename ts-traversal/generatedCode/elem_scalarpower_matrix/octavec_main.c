//
//  elem_scalarpower_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/26/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Raises an int matrix to an integer, elementwise
int ii_test()
{
	int exponent = 3;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (int)(pow(-1, i))*(i+1);
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


	int *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((pow(-1, i))*(i+1), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, INTEGER);
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

// Raises an int matrix to a double, elementwise
int id_test()
{
	double exponent = 1.2;

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


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, DOUBLE);
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

// Raises an int matrix to a negative double, elementwise
int neg_id_test()
{
	double exponent = 1.2;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (pow(-1, i))*(i+1);
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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((pow(-1, i))*(i+1), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, DOUBLE);
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

// Raises an int matrix to a complex, elementwise
int ic_test()
{
	double complex exponent = 4+0.3*I;

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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((i+1), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, COMPLEX);
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

// Raises a double matrix to an integer, elementwise
int di_test()
{
	int exponent = 5;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (pow(-1, i))*(i+1.4);
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


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((pow(-1, i))*(i+1.4), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, INTEGER);
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

// Raises a double matrix to a double, elementwise
int dd_test()
{
	double exponent = 1.4;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1.4);
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


	double *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = pow((i+1.4), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, DOUBLE);
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

// Raises a double matrix to a negative double, elementwise
int neg_dd_test()
{
	double exponent = 1.4;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (pow(-1, i))*(i+1.4);
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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((pow(-1, i))*(i+1.4), exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, DOUBLE);
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

// Raises a double matrix to a complex, elementwise
int dc_test()
{
	double complex exponent = -0.5*I;

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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((i+1)+0.4, exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, COMPLEX);
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

// Raises a complex matrix to an int, elementwise
int ci_test() 
{
	int exponent = 3;

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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((i+1)+0.5*I, exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, INTEGER);
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

// Raises a complex matrix to a double, elementwise
int cd_test() 
{
	double exponent = -0.9;

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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((i+1)+0.5*I, exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, DOUBLE);
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

// Raises a complex matrix to a complex, elementwise
int cc_test() 
{
	double complex exponent = 2-2*I;

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


	double complex *input2 = NULL;
	input2 = malloc(9*sizeof(*input2));
	for (int i = 0 ; i < 9 ; i++)
	{
		input2[i] = cpow((i+1)+0.5*I, exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, COMPLEX);
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

// Raises an int matrix to an integer, elementwise, outputting a double
int overflow_test()
{
	int exponent = 2;

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
		input2[i] = pow((double) INT_MAX, (double) exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, INTEGER);
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

// Raises an int matrix to an integer, elementwise, outputting a double
int underflow_test()
{
	int exponent = 2;

	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = INT_MIN;
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
		input2[i] = pow((double) INT_MIN, (double) exponent);
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
	printM(tmp2);


	Matrix *tmp3 = scalarpowerM(tmp, &exponent, INTEGER);
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
	if(!underflow_test()){flag = false;}

	return(flag);
}