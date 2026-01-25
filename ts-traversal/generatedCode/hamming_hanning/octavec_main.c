//
//  hamming_hanning.c
//  Octave-C
//
//  Created by Zach Taylor on 6/22/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <tgmath.h>
#include <matrix.h>

// Outputs hamming and hanning coefficients
int main() 
{
	bool flag = true;

	int iterations = 3;

	for (int i = 1 ; i < iterations+1 ; i++)
	{
		double *ham = hamming(i);
		for (int j = 0 ; j < i ; j++)
		{
			printf("%.4f\n", ham[j]);
		}
		free(ham);

		double *hamp = periodichamming(i);
		for (int j = 0 ; j < i ; j++)
		{
			printf("%.4f\n", hamp[j]);
		}
		free(hamp);

		double *han = hanning(i);
		for (int j = 0 ; j < i ; j++)
		{
			printf("%.4f\n", han[j]);
		}
		free(han);

		double *hanp = periodichanning(i);
		for (int j = 0 ; j < i ; j++)
		{
			printf("%.4f\n", hanp[j]);
		}
		free(hanp);
	}
	return(flag);
}
