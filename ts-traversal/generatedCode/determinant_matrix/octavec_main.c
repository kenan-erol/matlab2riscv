//
//  determinant_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 6/2/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Calculates the determinant of a 1x1 matrix
int test1()
{
	bool flag = true;

	Matrix *id = identityM(1);

	if(!id)
	{
		fprintf(stderr, "Failed to create identity matrix\n");
		return(false);
	}

	printM(id);

	// Create a holder for the determinant value
	int det_id = 0;

	// Calculate the determinant
	detM(id, &det_id);
	printf("%d\n", det_id);

		if (det_id != 1)
	{
		fprintf(stdout, "Incorrect determinant: %d\n", det_id);
		flag = false;
	}

	if (!destroyM(id))
	{
		fprintf(stderr, "Failed to destroy identity matrix\n");
		flag = false;
	}
	
	return(flag);
}

// Calculates the determinant of a 2x2 matrix
int test2()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {2,2};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex input[] = {
		26 + 1*I, 3 - 8*I,
		20*I, 1 + 25*I};
	writeM(tmp, size, input);
	printM(tmp);
	
	// Create a holder for the determinant
	double complex num = 0;

	// Calculate the determinant
	detM(tmp, &num);
	printf("%.5f + %.5fi\n", creal(num), cimag(num));

	if (num != -159+591*I)
	{
		fprintf(stdout, "Incorrect determinant: %f+%fi\n", creal(num), cimag(num));
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Calculates the determinant of a 3x3 matrix
int test3()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	// Create the matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if (!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}


	// Write the matrix data
	double *input = NULL;
	input = malloc(9*sizeof(*input));
	for (int i = 0 ; i < 9 ; i++)
	{
		input[i] = (i+1)*(i+1);
	}
	writeM(tmp, 9, input);
	free(input);
	printM(tmp);

	// Create a holder for the determinant
	double num = 0;

	// Calculate the determinant
	detM(tmp, &num);

	printf("%.5f\n", num);


	// if (num != -216) // Comparing doubles like this is bad.
	if (!(fabs(num - (-216)) < 10000*DBL_EPSILON)) // Do this instead
	{
		fprintf(stdout, "Incorrect determinant: %f\n", num);
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Calculates the determinant of a 5x5 matrix
int testn()
{
	bool flag = true;
	int ndim = 2;
	int dim[2] = {5,5};
	Matrix *tmp = createM(ndim, dim, DOUBLE);

	if (!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double *input = NULL;
	input = malloc(25*sizeof(*input));

	// Matrix with determinant 3101
	int counter = 0;
	for (int i = 0 ; i < 5 ; i++)
	{
		for (int j = 0 ; j < 5 ; j++)
		{
			counter += 1;
			if ((counter-1)%2 == 0)
			{
				input[counter-1] = (counter + i)%7;
			}
			else
			{
				input[counter-1] = -1*((counter + j)%7);
			}
		}
	}
	writeM(tmp, 25, input);
	free(input);
	printM(tmp);

	double num = 0;
	detM(tmp, &num);
	printf("%.5f\n", num);

	if ((int) round(num) != 3101)
	{
		fprintf(stdout, "Incorrect determinant: %.5f\n", num);
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

// Non-square matrices will get a determinant of zero
int non_square()
{
	bool flag = true;
	int ndim = 2;
	int dim[2] = {2,3};

	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (!tmp)
	{
		fprintf(stderr, "Failed to create matrix\n");
		return(false);
	}

	double complex *input = NULL;
	input = malloc(6*sizeof(*input));
	input[0] = 26 + 1*I;
	input[1] = 3 - 8*I;
	input[2] = 20*I;
	input[3] = 1 + 25*I;
	input[4] = 0;
	input[5] = 1;
	writeM(tmp, 6, input);
	free(input);
	printM(tmp);
	

	double complex num = 0;
	detM(tmp, &num);

	if (num != 0)
	{
		fprintf(stdout, "Incorrect determinant: %f+%fi\n", creal(num), cimag(num));
		flag = false;
	}

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}

int main()
{
	bool flag = true;
	if(!test1()){flag = false;}
	if(!test2()){flag = false;}
	if(!test3()){flag = false;}
	if(!testn()){flag = false;}

	if(!non_square()){flag = false;}
	return(flag);
}