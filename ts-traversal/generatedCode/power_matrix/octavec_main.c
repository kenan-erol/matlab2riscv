//
//  power_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 6/3/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Int matrix to 0 power
int i_zero()
{
	bool flag = true;

	Matrix *id = identityM(3);

	if(!id)
	{
		fprintf(stderr, "Failed to create identity matrix\n");
		return(false);
	}

	printM(id);

	int exponent = 0;
	Matrix *power = mpowerM(id, &exponent, INTEGER);
	printM(power);

	if (!isEqualM(id, power))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(id))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Double matrix to 0 power
int d_zero()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *id3 = idM(3);
	if(!id3)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double exponent = 0;
	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!isEqualM(power, id3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
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

// Complex matrix to 0 power
int c_zero()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *id3 = idM(3);
	if(!id3)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	Matrix *tmp = createM(ndim, dim, COMPLEX);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double complex exponent = 0;
	Matrix *power = mpowerM(tmp, &exponent, COMPLEX);
	printM(power);

	if (!isEqualM(power, id3))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
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

// Int matrix to 1 power
int i_one()
{
	bool flag = true;

	Matrix *id = identityM(3);

	if(!id)
	{
		fprintf(stderr, "Failed to create identity matrix\n");
		return(false);
	}

	printM(id);

	int exponent = 1;
	Matrix *power = mpowerM(id, &exponent, INTEGER);
	printM(power);

	if (!isEqualM(id, power))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(id))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Double matrix to 1 power
int d_one()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double exponent = 1;
	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!isEqualM(tmp, power))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Complex matrix to 1 power
int c_one()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	Matrix *tmp = createM(ndim, dim, COMPLEX);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double complex exponent = 1;
	Matrix *power = mpowerM(tmp, &exponent, COMPLEX);
	printM(power);

	if (!isEqualM(tmp, power))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Int matrix to 20 power
int i_large()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	int exponent = 20;
	Matrix *power = mpowerM(tmp, &exponent, INTEGER);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Int matrix to -20 power
int i_negative()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, INTEGER);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	int exponent = -20;
	Matrix *power = mpowerM(tmp, &exponent, INTEGER);
	Matrix *power_floor = floorM(power);
	printM(power_floor);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power_floor))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Double matrix to 0.05 power
int d_small()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double exponent = 0.05;
	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Double matrix to -1.1 power
int d_negative()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(9*sizeof(*input));
	int one = -1;
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = one*(i+1)*(i+1);
		one *= -1;
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double exponent = -1.1;
	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Complex matrix to -10 + 7.8*I power
int c_large()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	Matrix *tmp = createM(ndim, dim, COMPLEX);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double complex exponent = -10 + 7.8*I;
	Matrix *power = mpowerM(tmp, &exponent, COMPLEX);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Complex matrix to -0.8*I power
int c_small()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	Matrix *tmp = createM(ndim, dim, COMPLEX);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(tmp, 9, input);
	free(input);

	printM(tmp);

	double complex exponent = -0.8*I;
	Matrix *power = mpowerM(tmp, &exponent, COMPLEX);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(power))
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


	// Try it ALL
	double complex complex_one = 1;
	for (int index = 2 ; index < 12 ; index++)
	{
		printf("Original\n");
		Matrix *complex_mat = scaleM(matrices[index], &complex_one, COMPLEX);
		printM(complex_mat);
		if (!destroyM(complex_mat))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
		

		printf("Integer exponents\n");
		for (int i = -4 ; i <= 4 ; i++)
		{
			Matrix *power = mpowerM(matrices[index], &i, INTEGER);
			Matrix *complex_power = scaleM(power, &complex_one, COMPLEX);
			printM(complex_power);
			if (!destroyM(power))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}
			if (!destroyM(complex_power))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}
		}

		printf("Double exponents\n");
		for (double i = -3 ; i <= 1.9 ; i+=0.2)
		{
			fprintf(stdout, "Exponent: %.4f\n", i);
			Matrix *power = mpowerM(matrices[index], &i, DOUBLE);
			Matrix *complex_power = scaleM(power, &complex_one, COMPLEX);
			printM(complex_power);
			if (!destroyM(power))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}
			if (!destroyM(complex_power))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}
		}

		printf("Complex exponents\n");
		for (double i = -3 ; i <= 3.1 ; i+=0.2)
		{
			for (double j = -3 ; j <= 3.1 ; j+=0.2)
			{
				if (fabs(j) <= 100*DBL_EPSILON)
				{
					continue;
				}
				double complex num = i + j*I;
				fprintf(stdout, "Exponent: %.4f + %.4fi\n", i, j);
				Matrix *power = mpowerM(matrices[index], &num, COMPLEX);
				Matrix *complex_power = scaleM(power, &complex_one, COMPLEX);
				printM(complex_power);
				if (!destroyM(power))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
				if (!destroyM(complex_power))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
			}
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


// Non-diagonalizeable matrix to double power
// Note: behavior is undefined
int non_diag1()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};

	Matrix *tmp = createM(ndim, dim, INTEGER);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	int *input = NULL;
	input = malloc(4*sizeof(*input));
	for (int i = 0 ; i < 4 ; i++)
	{
		input[i] = 1;
	}
	input[2] = 0;
	writeM(tmp, 4, input);
	free(input);

	printM(tmp);

	double exponent = 1.5;
	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Non-diagonalizeable matrix to double power
// Note: behavior is undefined
int non_diag2()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	Matrix *tmp = createM(ndim, dim, INTEGER);

	if(!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	int *input = NULL;
	input = malloc(9*sizeof(*input));
	
	input[0] = 3;
	input[1] = 4;
	input[2] = 3;

	input[3] = -1;
	input[4] = 0;
	input[5] = -1;

	input[6] = 1;
	input[7] = 2;
	input[8] = 3;

	writeM(tmp, 9, input);
	free(input);
	printM(tmp);

	double exponent = -4.25;

	Matrix *power = mpowerM(tmp, &exponent, DOUBLE);
	printM(power);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}

	if (!destroyM(power))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
		
	return(flag);
}

int main()
{
	bool flag = true;

	if(!i_zero()){flag = false;}
	if(!d_zero()){flag = false;}
	if(!c_zero()){flag = false;}

	if(!i_one()){flag = false;}
	if(!d_one()){flag = false;}
	if(!c_one()){flag = false;}

	if(!i_large()){flag = false;}
	if(!i_negative()){flag = false;}

	if(!d_small()){flag = false;}
	if(!d_negative()){flag = false;}

	if(!c_large()){flag = false;}
	if(!c_small()){flag = false;}

	if(!brutal_test()){flag = false;}

	// if(!non_diag1()){flag = false;}
	// if(!non_diag2()){flag = false;}

	return(flag);
}