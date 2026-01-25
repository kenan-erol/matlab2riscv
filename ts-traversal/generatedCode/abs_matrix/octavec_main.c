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


// Computes the absolute value of a matrix
int complexTest()
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Create an uninitialized matrix
	Matrix *tmp = createM(ndim, dim, COMPLEX);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Input data to a matrix doesn't necessarily have to be malloc'ed
	// But if it is malloc'ed, it can be free'ed as soon as it's been writeM'ed
	double complex input[] = {
		0, 10, 10*I, 
		10.102, 10.102 + 0.5*I, -12*I,
		-0.0002 - 0.1*I, -100.01*I, 81};

	writeM(tmp, size, input);
	printM(tmp);

	// Get the elementwise absolute value of the matrix
	Matrix *tmp2 = absM(tmp);
	printM(tmp2);

	// Check that the two matrices are NOT equal
	if(isEqualM(tmp, tmp2))
	{
		fprintf(stdout, "FAILURE\n");
		flag = false;
	}

	// Clean up
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

	return(flag);
}

int intTest()
{
	bool flag = true;

	// The absolute value of an identity matrix should be itself
	Matrix *tmp3 = identityM(8);
	Matrix *tmp4 = absM(tmp3);
	if(!isEqualM(tmp3, tmp4))
	{
		fprintf(stdout, "FAILURE\n");
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

	return(flag);
}

int main() 
{
	bool flag = true;
	if(!complexTest()){flag = false;}
	if(!intTest()){flag = false;}
	return(flag);
}
