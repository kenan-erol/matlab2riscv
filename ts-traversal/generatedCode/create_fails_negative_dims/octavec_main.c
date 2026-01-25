//
//  create_fails_negative_dims.c
//  Octave-C
//
//  Created by Zach Taylor on 5/24/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// createM should fail if given a negative dimension
int main() 
{
	bool flag = true;

	int ndim = 2;
	int dim[2] = {3,-3};

	Matrix *tmp = zerosM(ndim, dim);
	if (tmp == NULL)
	{
		fprintf(stdout, "Success\n");
	}

	else
	{
		fprintf(stdout, "Failure\n");
	}

	return(flag);
}
