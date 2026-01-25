//
//  utils.c
//  Octave-C
//
//  Created by Zach Taylor on 12/4/22
//

#define _XOPEN_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include "tgmath.h"
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "../riscv-blas/INCLUDE/cblas.h"
#include "../riscv-blas/INCLUDE/lapacke.h"
#include "../fftw-3.3.10/api/fftw3.h"

#include "../BSC-RVV/common/vector_defines.h"
#include "../BSC-RVV/common/trace_macros.h"

#include "matrix.h"

bool can_matrices_be_compared_internal(Matrix* restrict m, Matrix* restrict n)
{
	if (m->ndim != n->ndim)
	{
		fprintf(stderr, "%s: %s (%d): Matrices must have same dimensionality to be compared\n", __FILE__, __func__, __LINE__);
		return(false);
	}

	int *m_dim = m->dim;
	int *n_dim = n->dim;

	for (register int i = 0 ; i < 4 ; i++)
	{
		if (m_dim[i] != n_dim[i])
		{
			fprintf(stderr, "%s: %s (%d): Matrices must have same dimensions to be compared\n", __FILE__, __func__, __LINE__);
			return(false);
		}
	}
	return(true);
}

bool can_matrices_be_compared(Matrix *m, Matrix *n)
// Checks if matrices can be compared
{
	if (m == NULL || n == NULL)
	{
		return(false);
	}

	if (m == n)
	{
		return(true);
	}

	return can_matrices_be_compared_internal(m, n);
}

// Internal functions to check equality while accounting for rounding errors
bool almostEqual_ii(int x, int y) {return(fabs(x-y) <= EPSILON);}
bool almostEqual_id(int x, double y) {return(fabs(x-y) <= EPSILON);}
bool almostEqual_ic(int x, double complex y) {return(fabs(x-(creal(y))) <= EPSILON && fabs((cimag(y))) <= EPSILON);}
bool almostEqual_di(double x, int y) {return(almostEqual_id(y, x));}
bool almostEqual_dd(double x, double y) {return(fabs(x-y) <= EPSILON);}
bool almostEqual_dc(double x, double complex y) {return(fabs(x-(creal(y))) <= EPSILON && fabs((cimag(y))) <= EPSILON);}
bool almostEqual_ci(double complex x, int y) {return(almostEqual_ic(y, x));}
bool almostEqual_cd(double complex x, double y) {return(almostEqual_dc(y, x));}
bool almostEqual_cc(double complex x, double complex y) {return(fabs((creal(x))-(creal(y))) <= EPSILON && fabs((cimag(x))-(cimag(y))) <= EPSILON);}

// Internal functions for casting
int * i_to_i_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Load the ints
		vint32m8_t v_x = vle32_v_i32m8(&m_data_data[i], trueVL);

		// Store
		vse32_v_i32m8(&output[i], v_x, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
int * i_to_i_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (int) (m_data_data[i]);
	}

	return(output);
}
int * i_to_i(Matrix* restrict m)
{
    if (MODE == VEC) {return i_to_i_vec(m);}
    else if (MODE == REF) {return i_to_i_ref(m);}

	// Default mode
    return i_to_i_ref(m);
}
double * i_to_d_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&m_data_data[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

		// Store
		vse64_v_f64m8(&output[i], v_x, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
double * i_to_d_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double) (m_data_data[i]);
	}

	return(output);
}
double * i_to_d(Matrix* restrict m)
{
    if (MODE == VEC) {return i_to_d_vec(m);}
    else if (MODE == REF) {return i_to_d_ref(m);}

	// Default mode
    return i_to_d_ref(m);
}
double complex * i_to_c_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Initialize vectors of zeros
        vfloat64m8_t v_zero  = vfmv_v_f_f64m8 (0.0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&m_data_data[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

		// Store
        vsse64_v_f64m8(&(((double *)(output))[2*i]), 16, v_x, trueVL); 
		vsse64_v_f64m8(&(((double *)(output))[2*i+1]), 16, v_zero, trueVL); 

		i += trueVL;
	}

	FENCE();

	return(output);
}
double complex * i_to_c_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	int *m_data_data = ((int *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double complex) (m_data_data[i]);
	}

	return(output);
}
double complex * i_to_c(Matrix* restrict m)
{
    if (MODE == VEC) {return i_to_c_vec(m);}
    else if (MODE == REF) {return i_to_c_ref(m);}

	// Default mode
    return i_to_c_ref(m);
}
int * d_to_i_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the ints & cast to doubles
		vfloat64m8_t v_x = vle64_v_f64m8(&m_data_data[i], trueVL); 
        vint32m4_t v_x_rtz_int = vfncvt_rtz_x_f_w_i32m4 (v_x, trueVL);

		// Store
		vse32_v_i32m4(&output[i], v_x_rtz_int, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
int * d_to_i_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (int) (m_data_data[i]);
	}

	return(output);
}
int * d_to_i(Matrix* restrict m)
{
    if (MODE == VEC) {return d_to_i_vec(m);}
    else if (MODE == REF) {return d_to_i_ref(m);}

	// Default mode
    return d_to_i_ref(m);
}
double * d_to_d_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles
		vfloat64m8_t v_x = vle64_v_f64m8(&m_data_data[i], trueVL); 

		// Store
		vse64_v_f64m8(&output[i], v_x, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
double * d_to_d_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double) (m_data_data[i]);
	}

	return(output);
}
double * d_to_d(Matrix* restrict m)
{
    if (MODE == VEC) {return d_to_d_vec(m);}
    else if (MODE == REF) {return d_to_d_ref(m);}

	// Default mode
    return d_to_d_ref(m);
}
double complex * d_to_c_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Initialize vectors of zeros
        vfloat64m8_t v_zero  = vfmv_v_f_f64m8 (0.0, trueVL);

        // Load the doubles
		vfloat64m8_t v_x = vle64_v_f64m8(&m_data_data[i], trueVL); 

		// Store
        vsse64_v_f64m8(&(((double *)(output))[2*i]), 16, v_x, trueVL); 
		vsse64_v_f64m8(&(((double *)(output))[2*i+1]), 16, v_zero, trueVL); 

		i += trueVL;
	}

	FENCE();

	return(output);
}
double complex * d_to_c_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double *m_data_data = ((double *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double complex) (m_data_data[i]);
	}

	return(output);
}
double complex * d_to_c(Matrix* restrict m)
{
    if (MODE == VEC) {return d_to_c_vec(m);}
    else if (MODE == REF) {return d_to_c_ref(m);}

	// Default mode
    return d_to_c_ref(m);
}
int * c_to_i_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the complexes and cast to ints
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(m_data_data))[2*i]), 16, trueVL); // Stride-load the real parts
        vint32m4_t v_x_rtz_int = vfncvt_rtz_x_f_w_i32m4 (v_x, trueVL);

		// Store
		vse32_v_i32m4(&output[i], v_x_rtz_int, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
int * c_to_i_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	int *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (int) (creal(m_data_data[i]));
	}

	return(output);
}
int * c_to_i(Matrix* restrict m)
{
    if (MODE == VEC) {return c_to_i_vec(m);}
    else if (MODE == REF) {return c_to_i_ref(m);}

	// Default mode
    return c_to_i_ref(m);
}
double * c_to_d_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the complexes
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(m_data_data))[2*i]), 16, trueVL);

		// Store
		vse64_v_f64m8(&output[i], v_x, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
double * c_to_d_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	double *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double) (creal(m_data_data[i]));
	}

	return(output);
}
double * c_to_d(Matrix* restrict m)
{
    if (MODE == VEC) {return c_to_d_vec(m);}
    else if (MODE == REF) {return c_to_d_ref(m);}

	// Default mode
    return c_to_d_ref(m);
}
double complex * c_to_c_vec(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

    int n = m_data_size;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the complexes
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(m_data_data))[2*i]), 16, trueVL);
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(m_data_data))[2*i+1]), 16, trueVL);

		// Store
        vsse64_v_f64m8(&(((double *)(output))[2*i]), 16, v_x_r, trueVL); 
		vsse64_v_f64m8(&(((double *)(output))[2*i+1]), 16, v_x_i, trueVL);

		i += trueVL;
	}

	FENCE();

	return(output);
}
double complex * c_to_c_ref(Matrix* restrict m)
{
	int m_data_size = m->data->size;
	double complex *m_data_data = ((double complex *)(m->data->data));

	double complex *output = NULL;
	output = malloc((m_data_size)*sizeof(*output));

	for (register int i = 0 ; i < m_data_size ; i++)
	{
		output[i] = (double complex) (m_data_data[i]);
	}

	return(output);
}
double complex * c_to_c(Matrix* restrict m)
{
    if (MODE == VEC) {return c_to_c_vec(m);}
    else if (MODE == REF) {return c_to_c_ref(m);}

	// Default mode
    return c_to_c_ref(m);
}




// Internal functions for various comparisons
int int_comp_asc(const void *elem1, const void *elem2) 
{
    int f = *(int *)elem1;
	int s = *(int *)elem2;
    if (f > s) 
    {
    	return(1);
    }
    if (f < s)
    {
    	return(-1);
    }
    return 0;
}
int int_comp_desc(const void *elem1, const void *elem2) 
{
    return(int_comp_asc(elem2, elem1));
}
int double_comp_asc(const void *elem1, const void *elem2) 
{
	double f = *(double *)elem1;
	double s = *(double *)elem2;
    if (f > s) 
    {
    	return(1);
    }
    if (f < s)
    {
    	return(-1);
    }
    return 0;
}
int double_comp_desc(const void *elem1, const void *elem2) 
{
    return(double_comp_asc(elem2, elem1));
}
int complex_comp_asc(const void *elem1, const void *elem2) 
{
	double complex f = *(double complex *)elem1;
	double complex s = *(double complex *)elem2;

	// Complex numbers are sorted by magnitude, with ties broken by phas angle
    if (cabs(f) > cabs(s)) 
    {
    	return(1);
    }
    if (cabs(f) < cabs(s))
    {
    	return(-1);
    }
    if (carg(f) > carg(s)) 
    {
    	return(1);
    }
    if (carg(f) < carg(s)) 
    {
    	return(-1);
    }
    return 0;
}
int complex_comp_desc(const void *elem1, const void *elem2) 
{
    return(complex_comp_asc(elem2, elem1));
}

bool containsOverflow(double *x, int size)
// Returns true if the input double array x of length size contains a value that cannot be represented as an integer
{
	for (int i = 0 ; i < size ; i++)
	{
		if (x[i] > INT_MAX || x[i] < INT_MIN) 
		{
			return(true);
		}
	}
	return(false);
}

long long get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}
// Returns the number of seconds elapsed between the two specified times
float elapsed_time(long long start_time, long long end_time) {
        return (float) (end_time - start_time) / (1000 * 1000);
}

unsigned long get_cycles(void)
{
  unsigned long cycles;
  asm volatile ("rdcycle %0" : "=r" (cycles));
  return cycles;
}

// Number of cycles elapsed
double elapsed_cycles(unsigned long start_cycles, unsigned long end_cycles)
{
	return((double)(end_cycles - start_cycles));
}

