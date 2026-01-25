//
//  create_one_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 5/20/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Creates a matrix of ones
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,3};

	// Create a 3x3 matrix of ones
	Matrix *tmp = onesM(ndim, dim);
	if (tmp == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	printM(tmp);

	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	// Create a 1x6 matrix of ones
	int dim2[2] = {1,6};

	Matrix *tmp2 = onesM(ndim, dim2);
	if (tmp2 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	printM(tmp2);

	if (!destroyM(tmp2))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	// Create a 4x1 matrix of ones
	int dim3[2] = {4,1};

	Matrix *tmp3 = onesM(ndim, dim3);
	if (tmp3 == NULL)
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	printM(tmp3);

	if (!destroyM(tmp3))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	return(flag);
}
