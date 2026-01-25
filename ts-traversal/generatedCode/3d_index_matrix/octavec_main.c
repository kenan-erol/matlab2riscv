//
//  3d_index_matrix.c
//  Octave-C
//
//  Created by Zach Taylor on 4/27/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Checks that a 3D matrix is can be indexed properly
int rowMajor() 
{
	bool flag = true;

	int ndim = 3;
	int dim[3] = {2,3,5};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}

	// Initialize the data for a 3D matrix
	double *input = NULL;
	input = malloc(size*sizeof(*input));
	for (int i = 0 ; i < size ; i++)
	{
		input[i] = i*i+0.5;
	}

	// Create the matrix
	Matrix *tmp = createM(ndim, dim, DOUBLE);
	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		free(input);
		return(false);
	}
	writeM(tmp, size, input);		
	printM(tmp);
	
	// Print out all the matrix values via flat indexing
	for (int i = 1 ; i <= size ; i++)
	{
		double num;
		indexM(tmp, &num, 1, i);
		if(num != input[i-1])
		{
			fprintf(stdout, "Indexing failure %d: [%f]; expected: [%f]\n", i, num, input[i-1]);
			flag = false;
		}
		fprintf(stdout, "%f\n", num);
	}
	fprintf(stdout, "\n");

	// Print out all the matrix values via normal indexing
	for (int k = 1 ; k <= dim[2] ; k++)
	{
		for (int j = 1 ; j <= dim[1] ; j++)
		{
			for (int i = 1 ; i <= dim[0] ; i++)
			{
				double num;
				indexM(tmp, &num, getnDimM(tmp), i, j, k);
				if(num != input[(j-1)+(i-1)*dim[1]+(k-1)*dim[0]*dim[1]])
				{
					fprintf(stdout, "Indexing failure: tmp[%d][%d][%d]: [%f]; expected: [%f]\n", i, j, k, num, input[(j-1)+(i-1)*dim[1]+(k-1)*dim[0]*dim[1]]);
					flag = false;
				}
				fprintf(stdout, "%f\n", num);
			}
		}
	}
	fprintf(stdout, "\n");

	// Normal indexing in C is equivalent to flat indexing in Octave
	for (int k = 1 ; k <= dim[2] ; k++)
	{
		for (int j = 1 ; j <= dim[1] ; j++)
		{
			for (int i = 1 ; i <= dim[0] ; i++)
			{
				double num;
				indexM(tmp, &num, getnDimM(tmp), i, j, k);
				if(num != input[(j-1)+(i-1)*dim[1]+(k-1)*dim[0]*dim[1]])
				{
					fprintf(stdout, "Indexing failure: tmp[%d][%d][%d]: [%f]; expected: [%f]\n", i, j, k, num, input[(j-1)+(i-1)*dim[1]+(k-1)*dim[0]*dim[1]]);
					flag = false;
				}
				fprintf(stdout, "%f\n", num);
			}
		}
	}


	// Clean up
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	free(input);
	return(flag);
}

int columnMajor() 
{
	bool flag = true;

	int ndim = 3;
	int dim[3] = {2,3,5};
	int size = 1;
	for (int i = 0 ; i < ndim ; i++)
	{
		size *= dim[i];
	}		

	// Create the matrix
	Matrix *tmp_int = zerosM(ndim, dim);
	if ( tmp_int == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	double double_one = 1.0;
	Matrix *tmp = scaleM(tmp_int, &double_one, DOUBLE);

	if ( tmp == NULL )
	{
		fprintf(stderr, "Could not create matrix\n");
		return(false);
	}

	// Directly assign values to the matrix
	// Using this method is ill-advised, as the writeM method is preferred for 
	// assigning data to a matrix.
	// There is no built-in function to assign a value to a matrix by index.

	// Note that it iterates over k then j then i, equivalent to the Octave column major
	int counter = 0;
	for (int k = 0 ; k < dim[2] ; k++)
	{
		for (int j = 0 ; j < dim[1] ; j++)
		{
			for (int i = 0 ; i < dim[0] ; i++)
			{
				double *tmp_data_data = getdataM(tmp);
				int flat_index = j + i*dim[1] + k*dim[0]*dim[1];
				tmp_data_data[flat_index] = counter*counter + 0.5;
				counter++;
			}
		}
	}
	printM(tmp);

	// Print out all the matrix values via flat indexing
	for (int i = 1 ; i <= size ; i++)
	{
		double num;
		indexM(tmp, &num, 1, i);
		fprintf(stdout, "%f\n", num);
	}
	fprintf(stdout, "\n");

	// Print out all the matrix values via normal indexing
	counter = 0;
	for (int k = 1 ; k <= dim[2] ; k++)
	{
		for (int j = 1 ; j <= dim[1] ; j++)
		{
			for (int i = 1 ; i <= dim[0] ; i++)
			{
				double num;
				indexM(tmp, &num, getnDimM(tmp), i, j, k);
				if(num != counter*counter+0.5)
				{
					fprintf(stdout, "Indexing failure: tmp[%d][%d][%d]: [%f]; expected: [%f]\n", i, j, k, num,counter*counter+0.5);
					flag = false;
				}
				counter++;
				fprintf(stdout, "%f\n", num);
			}
		}
	}
	fprintf(stdout, "\n");
	
	// Normal indexing in C is equivalent to flat indexing in Octave
	counter = 0;
	for (int k = 1 ; k <= dim[2] ; k++)
	{
		for (int j = 1 ; j <= dim[1] ; j++)
		{
			for (int i = 1 ; i <= dim[0] ; i++)
			{
				double num;
				indexM(tmp, &num, getnDimM(tmp), i, j, k);
				if(num != counter*counter+0.5)
				{
					fprintf(stdout, "Indexing failure: tmp[%d][%d][%d]: [%f]; expected: [%f]\n", i, j, k, num,counter*counter+0.5);
					flag = false;
				}
				counter++;
				fprintf(stdout, "%f\n", num);
			}
		}
	}

	// Clean up
	if (!destroyM(tmp))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}

	if (!destroyM(tmp_int))
	{
		fprintf(stderr, "Failed to destroy matrix\n");
		flag = false;
	}
	
	return(flag);
}

int main() 
{
	bool flag = true;
	if(!rowMajor()){flag = false;}
	if(!columnMajor()){flag = false;}
	if(flag){exit(EXIT_SUCCESS);}else{exit(EXIT_FAILURE);}
}
