//
//  xcorrM.c
//  Octave-C
//
//  Created by Zach Taylor on 6/28/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

int main()
{
	bool flag = true;

	int D1 = 1;
	int dim1[1] = {20};

	int D2 = 2;
	int dim2[2] = {1,10};

	double complex z = 4.5 - 0.5*I;

	int totalMatrices = 12;
	Matrix **matrices = NULL;
	matrices = malloc(totalMatrices*sizeof(*matrices));

	matrices[0] = zerosM(D2, dim2);
	matrices[1] = onesM(D1, dim1);
	matrices[2] = onesM(D2, dim2);
	matrices[3] = scaleM(matrices[1], &z, COMPLEX);

	matrices[4] = createM(D2, dim2, INTEGER);
	int *input4 = NULL;
	input4 = malloc(10*sizeof(*input4));
	for (int i = 0 ; i < 10 ; i++)
	{
		input4[i] = (i+1)*(i+1);
	}
	writeM(matrices[4], 10, input4);
	free(input4);

	matrices[5] = createM(D2, dim2, DOUBLE);
	double *input5 = NULL;
	input5 = malloc(10*sizeof(*input5));
	for (int i = 0 ; i < 10 ; i++)
	{
		input5[i] = (i+1)*(i+1)+0.5;
	}
	writeM(matrices[5], 10, input5);
	free(input5);

	matrices[6] = createM(D1, dim1, COMPLEX);
	double complex *input6 = NULL;
	input6 = malloc(20*sizeof(*input6));
	for (int i = 0 ; i < 20 ; i++)
	{
		input6[i] = (i+1)*(i+1)+0.5*I;
	}
	writeM(matrices[6], 20, input6);
	free(input6);

	matrices[7] = createM(D1, dim1, INTEGER);
	int *input7 = NULL;
	input7 = malloc(20*sizeof(*input7));
	for (int i = 0 ; i < 20 ; i++)
	{
		input7[i] = (i-4)*(i+1);
	}
	writeM(matrices[7], 20, input7);
	free(input7);

	matrices[8] = createM(D1, dim1, DOUBLE);
	double *input8 = NULL;
	input8 = malloc(20*sizeof(*input8));
	for (int i = 0 ; i < 20 ; i++)
	{
		input8[i] = (i-7.5)*(i+1)+0.5;
	}
	writeM(matrices[8], 20, input8);
	free(input8);

	matrices[9] = createM(D2, dim2, COMPLEX);
	double complex *input9 = NULL;
	input9 = malloc(10*sizeof(*input9));
	for (int i = 0 ; i < 10 ; i++)
	{
		input9[i] = (i-4.5)*(i+1)+((0.5)*(3-i))*I;
	}
	writeM(matrices[9], 10, input9);
	free(input9);

	matrices[10] = createM(D2, dim2, DOUBLE);
	double input10[] = {3,-2,0,   4,-1,0,   0,0,1, 2.5};
	writeM(matrices[10], 10, input10);

	matrices[11] = full_duplicateM(matrices[10]);


	// Try it ALL
	double complex complex_one = 1;
	for (int index = 0 ; index < 12 ; index++)
	{
		printf("i\n");
		Matrix *complex_mat = scaleM(matrices[index], &complex_one, COMPLEX);
		printM(complex_mat);
		if (!destroyM(complex_mat))
		{
			fprintf(stderr, "Failed to destroy matrix\n");
			flag = false;
		}
		
		for (int j = 0 ; j < 12 ; j++)
		{
			printf("j\n");
			Matrix *complex_mat2 = scaleM(matrices[j], &complex_one, COMPLEX);
			printM(complex_mat2);
			if (!destroyM(complex_mat2))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}

			Matrix *tmp_none = xcorrM(matrices[index], matrices[j], 0, "none");
			Matrix *complex_mat_none = scaleM(tmp_none, &complex_one, COMPLEX);
			printM(complex_mat_none);
			if (!destroyM(tmp_none))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}
			if (!destroyM(complex_mat_none))
			{
				fprintf(stderr, "Failed to destroy matrix\n");
				flag = false;
			}

			if (index > 0 && j > 0 && getsizeM(matrices[index]) == getsizeM(matrices[j]))
			{
				Matrix *tmp_unbiased = xcorrM(matrices[index], matrices[j], 0, "unbiased");
				Matrix *complex_mat_unbiased = scaleM(tmp_unbiased, &complex_one, COMPLEX);
				printM(complex_mat_unbiased);
				if (!destroyM(tmp_unbiased))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
				if (!destroyM(complex_mat_unbiased))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}

				Matrix *tmp_biased = xcorrM(matrices[index], matrices[j], 0, "biased");
				Matrix *complex_mat_biased = scaleM(tmp_biased, &complex_one, COMPLEX);
				printM(complex_mat_biased);
				if (!destroyM(tmp_biased))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
				if (!destroyM(complex_mat_biased))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}

				Matrix *tmp_coeff = xcorrM(matrices[index], matrices[j], 0, "coeff");
				Matrix *complex_mat_coeff = scaleM(tmp_coeff, &complex_one, COMPLEX);
				printM(complex_mat_coeff);
				if (!destroyM(tmp_coeff))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
				if (!destroyM(complex_mat_coeff))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
			}
			

			for (int k = 9 ; k < 22 ; k++)
			{
				Matrix *tmp_none = xcorrM(matrices[index], matrices[j], k, "none");
				Matrix *complex_mat_none = scaleM(tmp_none, &complex_one, COMPLEX);
				printM(complex_mat_none);
				if (!destroyM(tmp_none))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}
				if (!destroyM(complex_mat_none))
				{
					fprintf(stderr, "Failed to destroy matrix\n");
					flag = false;
				}

				if (index > 0 && j > 0 && getsizeM(matrices[index]) == getsizeM(matrices[j]))
				{
					Matrix *tmp_unbiased = xcorrM(matrices[index], matrices[j], k, "unbiased");
					Matrix *complex_mat_unbiased = scaleM(tmp_unbiased, &complex_one, COMPLEX);
					printM(complex_mat_unbiased);
					if (!destroyM(tmp_unbiased))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}
					if (!destroyM(complex_mat_unbiased))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}

					Matrix *tmp_biased = xcorrM(matrices[index], matrices[j], k, "biased");
					Matrix *complex_mat_biased = scaleM(tmp_biased, &complex_one, COMPLEX);
					printM(complex_mat_biased);
					if (!destroyM(tmp_biased))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}
					if (!destroyM(complex_mat_biased))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}

					Matrix *tmp_coeff = xcorrM(matrices[index], matrices[j], k, "coeff");
					Matrix *complex_mat_coeff = scaleM(tmp_coeff, &complex_one, COMPLEX);
					printM(complex_mat_coeff);
					if (!destroyM(tmp_coeff))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}
					if (!destroyM(complex_mat_coeff))
					{
						fprintf(stderr, "Failed to destroy matrix\n");
						flag = false;
					}
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
