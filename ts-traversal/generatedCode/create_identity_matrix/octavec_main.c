//
//  create_identity_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 2/22/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Checks that an identity matrix can be created
int main() 
{	
	bool flag = true;

	// Create an identity matrix of side length 8
	Matrix *tmp = identityM(8);
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

	// Create an identity matrix of side length 4
	Matrix *tmp2 = identityM(4);
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

	// Create an identity matrix of side length 1
	Matrix *tmp3 = identityM(1);
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