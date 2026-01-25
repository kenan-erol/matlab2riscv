//
//  elementwise_math.c
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
#include "../riscv-blas/INCLUDE/cblas.h"
#include "../riscv-blas/INCLUDE/lapacke.h"
#include "../fftw-3.3.10/api/fftw3.h"

#include "../BSC-RVV/common/vector_defines.h"
#include "../BSC-RVV/common/trace_macros.h"

#include "matrix.h"


void argM_vec_i(double* restrict y, int* restrict x, int n)
// Return the complex arg of an int array x
// If x is positive, 0 is returned; if x is negative, pi is returned
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Initialize vectors of zeros
        vfloat64m8_t v_res  = vfmv_v_f_f64m8 (0.0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Find negatives
		vbool8_t v_mask = vmflt_vf_f64m8_b8 (v_x, 0.0, trueVL);

        // If the mask is true, then the output should be pi
        v_res = vfmerge_vfm_f64m8 (v_mask, v_res, M_PI, trueVL);

		// Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void argM_vec_d(double* restrict y, double* restrict x, int n)
// Return the complex arg of a double array x
// If x is positive, 0 is returned; if x is negative, pi is returned
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Initialize vectors of zeros
        vfloat64m8_t v_res  = vfmv_v_f_f64m8 (0.0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

        // Find negatives
		vbool8_t v_mask = vmflt_vf_f64m8_b8 (v_x, 0.0, trueVL);

        // If the mask is true, then the output should be pi
        v_res = vfmerge_vfm_f64m8 (v_mask, v_res, M_PI, trueVL);

		// Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void argM_vec_c(double* restrict y, double complex* restrict x, int n)
// Return the complex arg of a complex array x
// If an element x[i] = a + b*I, then y[i] = atan2(b, a) 
{
	for (int i = 0 ; i < n ; i++)
	{
		y[i] = carg(x[i]);
	}
}

Matrix * argM_vec(Matrix* restrict m)
// Returns the elementwise argolute value of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			argM_vec_i((double *)(output_data_data), (int *)(m_data_data), m_data_size);
			break;

		case DOUBLE: ;
			argM_vec_d((double *)(output_data_data), (double *)(m_data_data), m_data_size);
			break;

		case COMPLEX: ;
			argM_vec_c((double *)(output_data_data), (double complex *)(m_data_data), m_data_size);
			break;
	}

	return(output);
}

Matrix * argM_ref(Matrix* restrict m)
// Returns the elementwise arg value of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	Matrix *output = createM(m->ndim, m->dim, DOUBLE);
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = carg( (double complex) (((int *)(m_data_data))[i]) );
			}
			break;

		case DOUBLE: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = carg( (double complex) (((double *)(m_data_data))[i]) );
			}
			break;

		case COMPLEX: ;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = carg( (double complex) (((double complex *)(m_data_data))[i]) );
			}
			break;
	}

	return(output);
}

Matrix * argM(Matrix* restrict m)
// Returns the elementwise arg of Matrix m
{
    if (MODE == VEC) {return argM_vec(m);}
    else if (MODE == REF) {return argM_ref(m);}

	// Default mode
    return argM_vec(m);
}







void absM_vec_i(int* restrict y, int* restrict x, int n)
// Takes an input integer array x of size n and returns an integer array y of the elementwise absolute value
// abs(x) = (x XOR a) - a, where a = x >> 31 (where x are 32-bits (i.e., ints) and >> is arithmetic shift right)
// Source: https://stackoverflow.com/questions/2639173/x86-assembly-abs-implementation
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	size_t shift = 31;

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input

		vint32m8_t v_a = vsra_vx_i32m8(v_x, shift, trueVL); // a = x >> 31
		vint32m8_t v_xor = vxor_vv_i32m8(v_x, v_a, trueVL); // xor = x XOR a
		vint32m8_t v_res = vsub_vv_i32m8 (v_xor, v_a, trueVL); // result = xor - a

		vse32_v_i32m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void absM_vec_d(double* restrict y, double* restrict x, int n)
// Takes an input double array x of size n and returns an double array y of the elementwise absolute value
// https://github.com/riscv/riscv-v-spec/blob/master/v-spec.adoc#1312-vector-floating-point-sign-injection-instructions
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfsgnjx_vv_f64m8(v_x, v_x, trueVL); // absolute value by sign-injection

		vse64_v_f64m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void absM_vec_c(double* restrict y, double complex* restrict x, int n)
// Takes an input complex array x of size n and returns an double array y of the elementwise absolute value
// abs(a + bi) = sqrt(a^2 + b^2)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); // Stride-load the real parts
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL); // Stride-load the imaginary parts
		vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_res = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		v_res = vfsqrt_v_f64m8(v_res, trueVL); // Sqrt
		vse64_v_f64m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

Matrix * absM_vec(Matrix* restrict m)
// Returns the elementwise absolute value of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, INTEGER);
			output_data_data = output->data->data;
			absM_vec_i((int *)(output_data_data), (int *)(m_data_data), m_data_size);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			absM_vec_d((double *)(output_data_data), (double *)(m_data_data), m_data_size);
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			absM_vec_c((double *)(output_data_data), (double complex *)(m_data_data), m_data_size);
			break;
	}

	return(output);
}

Matrix * absM_ref(Matrix* restrict m)
// Returns the elementwise absolute value of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, INTEGER);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((int *)(output_data_data))[i] = (int)(fabs(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(fabs(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = fabs(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

Matrix * absM(Matrix* restrict m)
// Returns the elementwise absolute value of Matrix m
{
    if (MODE == VEC) {return absM_vec(m);}
    else if (MODE == REF) {return absM_ref(m);}

	// Default mode
    return absM_vec(m);
}



void floorM_vec_d(double* restrict y, double* restrict x, int n)
// Floor = x_rtz  , if x >= 0
// Floor = x_rtz  , if x < 0 and x is an integer (i.e., x == x_rtz)
// Floor = x_rtz-1, if x < 0 and x is not an integer (i.e., x != x_rtz)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

        // Find non-negatives (ie, including zero)
        vbool8_t v_x_non_neg = vmfge_vf_f64m8_b8(v_x, 0, trueVL);

        // Get the values rounded-towards-zero
        vint32m4_t v_x_rtz_int = vfncvt_rtz_x_f_w_i32m4 (v_x, trueVL);
		vfloat64m8_t v_x_rtz = vfwcvt_f_x_v_f64m8(v_x_rtz_int, trueVL);

        // Decremented rtz's
        vfloat64m8_t v_x_rtz_minus_one = vfsub_vf_f64m8(v_x_rtz, 1.0, trueVL);

        // Find the integers
        vbool8_t v_x_integers = vmfeq_vv_f64m8_b8 (v_x, v_x_rtz, trueVL);

        // Final mask
        vbool8_t v_mask = vmor_mm_b8(v_x_non_neg, v_x_integers, trueVL);

        // Floor
        vfloat64m8_t v_res = vmerge_vvm_f64m8(v_mask, v_x_rtz_minus_one, v_x_rtz, trueVL);

        // Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void floorM_vec_c(double complex* restrict y, double complex* restrict x, int n) 
{
	floorM_vec_d(((double *)(y)), ((double *)(x)), 2*n);
}

Matrix * floorM_vec(Matrix* restrict m)
// Returns the elementwise floor of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM_vec(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			floorM_vec_d(((double *)(output_data_data)), ((double *)(m_data_data)), m_data_size);
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;
			floorM_vec_c(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), m_data_size);
			break;
	}

	return(output);
}

Matrix * floorM_ref(Matrix* restrict m)
// Returns the elementwise floor of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM_ref(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(floor(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = ((double complex)(floor((creal(((double complex *)(m_data_data))[i]))))) + ((double complex)(floor((cimag(((double complex *)(m_data_data))[i])))))*I;
			}
			break;
	}

	return(output);
}

Matrix * floorM(Matrix* restrict m)
// Returns the elementwise floor of Matrix m
{
    if (MODE == VEC) {return floorM_vec(m);}
    else if (MODE == REF) {return floorM_ref(m);}

	// Default mode
    return floorM_vec(m);
}



void ceilM_vec_d(double* restrict y, double* restrict x, int n)
// Ceil = x_rtz  , if x <= 0
// Ceil = x_rtz  , if x > 0 and x is an integer (i.e., x == x_rtz)
// Ceil = x_rtz+1, if x > 0 and x is not an integer (i.e., x != x_rtz)
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

        // Find non-positives (ie, including zero)
        vbool8_t v_x_non_pos = vmfle_vf_f64m8_b8(v_x, 0, trueVL);

        // Get the values rounded-towards-zero
        vint32m4_t v_x_rtz_int = vfncvt_rtz_x_f_w_i32m4 (v_x, trueVL);
		vfloat64m8_t v_x_rtz = vfwcvt_f_x_v_f64m8(v_x_rtz_int, trueVL);

        // Incremented rtz's
        vfloat64m8_t v_x_rtz_plus_one = vfadd_vf_f64m8(v_x_rtz, 1.0, trueVL);

        // Find the integers
        vbool8_t v_x_integers = vmfeq_vv_f64m8_b8 (v_x, v_x_rtz, trueVL);

        // Final mask
        vbool8_t v_mask = vmor_mm_b8(v_x_non_pos, v_x_integers, trueVL);

        // Ceil
        vfloat64m8_t v_res = vmerge_vvm_f64m8(v_mask, v_x_rtz_plus_one, v_x_rtz, trueVL);

        // Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void ceilM_vec_c(double complex* restrict y, double complex* restrict x, int n) 
{
	ceilM_vec_d(((double *)(y)), ((double *)(x)), 2*n);
}

Matrix * ceilM_vec(Matrix* restrict m)
// Returns the elementwise floor of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM_vec(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			ceilM_vec_d(((double *)(output_data_data)), ((double *)(m_data_data)), m_data_size);
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;
			ceilM_vec_c(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), m_data_size);
			break;
	}

	return(output);
}

Matrix * ceilM_ref(Matrix* restrict m)
// Returns the elementwise ceiling of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(ceil(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = ((double complex)(ceil((creal(((double complex *)(m_data_data))[i]))))) + ((double complex)(ceil((cimag(((double complex *)(m_data_data))[i])))))*I;
			}
			break;
	}

	return(output);
}

Matrix * ceilM(Matrix* restrict m)
// Returns the elementwise ceil of Matrix m
{
    if (MODE == VEC) {return ceilM_vec(m);}
    else if (MODE == REF) {return ceilM_ref(m);}

	// Default mode
    return ceilM_vec(m);
}



// Another version of round, proposed by David McCowin.
// It runs in the exact same number of cycles as roundM_vec_d,
// and so I felt it was worth including.
void roundM_vec_d_david(double* restrict y, double* restrict x, int n)
// Round = rtz(x)  , first_decimal_digit(x) < 5
// Round = rtz(x+1), otherwise
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

        // Find negatives
        vbool8_t v_mask_neg = vmflt_vf_f64m8_b8(v_x, 0, trueVL);

        // Abs x
        vfloat64m8_t v_x_abs = vfsgnjx_vv_f64m8(v_x, v_x, trueVL); // absolute value by sign-injection

        // Get the values rounded-towards-zero
        vint32m4_t v_x_int = vfncvt_rtz_x_f_w_i32m4 (v_x_abs, trueVL);
		vfloat64m8_t v_x_rtz = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);

        // Incremented rtz's
        vfloat64m8_t v_x_rtz_plus_one = vfadd_vf_f64m8(v_x_rtz, 1.0, trueVL);

        // rtz multiplied by 10
        vfloat64m8_t v_x_rtz_times_10 = vfmul_vf_f64m8(v_x_rtz, 10.0, trueVL);

        // Get the first digit after the decimal point of the numbers
        // (e.g., 10.913 -> 9)
        // first decimal digit = rtz(10x) - 10rtz(x), where rtz() is round-towards-zero
        vfloat64m8_t v_10x = vfmul_vf_f64m8(v_x_abs, 10.0, trueVL);
        vint32m4_t v_10x_int = vfncvt_rtz_x_f_w_i32m4 (v_10x, trueVL);
		vfloat64m8_t v_10x_rtz = vfwcvt_f_x_v_f64m8(v_10x_int, trueVL);

        vfloat64m8_t v_x_first_decimal_digit_signed = vfsub_vv_f64m8(v_10x_rtz, v_x_rtz_times_10, trueVL);
        vfloat64m8_t v_x_first_decimal_digit = vfsgnjx_vv_f64m8(v_x_first_decimal_digit_signed, v_x_first_decimal_digit_signed, trueVL); // absolute value by sign-injection

        // Values which have a first_decimal_digit greater than 5 should be rounded away from zero
        vbool8_t v_mask_raz = vmfge_vf_f64m8_b8(v_x_first_decimal_digit, 5, trueVL);

        // Unsigned rounded
        vfloat64m8_t v_res_s = vmerge_vvm_f64m8(v_mask_raz, v_x_rtz, v_x_rtz_plus_one, trueVL);

        vfloat64m8_t v_res_s_neg = vfsgnjn_vv_f64m8(v_res_s, v_res_s, trueVL); // Negate by sign-injection

        vfloat64m8_t v_res = vmerge_vvm_f64m8(v_mask_neg, v_res_s, v_res_s_neg, trueVL);

        // Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void roundM_vec_d(double* restrict y, double* restrict x, int n)
// Round = rne(x)           , abs(x) != (4n + 1)*0.5, n is any integer
// Round = rne(x + sign(x)) , otherwise
{
	// 3 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

        // Abs x
        vfloat64m8_t v_x_abs = vfsgnjx_vv_f64m8(v_x, v_x, trueVL); // absolute value by sign-injection

        // 2 * abs(x) - 1
        vfloat64m8_t v_x_abs_times_2 = vfmul_vf_f64m8(v_x_abs, 2.0, trueVL);
        vfloat64m8_t v_x_abs_times_2_minus_1 = vfsub_vf_f64m8(v_x_abs_times_2, 1.0, trueVL);

        // (2 * abs(x) - 1) / 4
        vfloat64m8_t v_x_abs_times_2_minus_1_div_4 = vfdiv_vf_f64m8(v_x_abs_times_2_minus_1, 4.0, trueVL);

        // Is that an integer?
        vfloat64m8_t v_tmp = v_x_abs_times_2_minus_1_div_4;
        vint32m4_t v_tmp_int = vfncvt_rtz_x_f_w_i32m4 (v_tmp, trueVL);
		vfloat64m8_t v_tmp_rtz = vfwcvt_f_x_v_f64m8(v_tmp_int, trueVL);

        vfloat64m8_t v_tmp_sub = vfsub_vv_f64m8(v_tmp, v_tmp_rtz, trueVL);
        vfloat64m8_t v_tmp_sub_abs = vfsgnjx_vv_f64m8(v_tmp_sub, v_tmp_sub, trueVL); // absolute value by sign-injection

        vbool8_t v_tmp_is_integer = vmfle_vf_f64m8_b8(v_tmp_sub_abs, EPSILON, trueVL);


        // Round original to nearest even
        vint32m4_t v_x_int = vfncvt_x_f_w_i32m4 (v_x, trueVL);
		vfloat64m8_t v_x_rne = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);

        // Find negatives of original
        vbool8_t v_mask_neg = vmflt_vf_f64m8_b8(v_x, 0, trueVL);

        // Incremented & decremented rte
        vfloat64m8_t v_x_rne_plus_one = vfadd_vf_f64m8(v_x_rne, 1.0, trueVL);
        vfloat64m8_t v_x_rne_minus_one = vfsub_vf_f64m8(v_x_rne, 1.0, trueVL);

        // Increment/decrement rne
        vfloat64m8_t v_x_rne_scaled = vmerge_vvm_f64m8(v_mask_neg, v_x_rne_plus_one, v_x_rne_minus_one, trueVL);

        // Final
        vfloat64m8_t v_res = vmerge_vvm_f64m8(v_tmp_is_integer, v_x_rne, v_x_rne_scaled, trueVL);

        // Store
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void roundM_vec_c(double complex* restrict y, double complex* restrict x, int n) 
{
	roundM_vec_d(((double *)(y)), ((double *)(x)), 2*n);
}

Matrix * roundM_vec(Matrix* restrict m)
// Returns the elementwise round of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM_vec(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
			roundM_vec_d((double *)(output_data_data), (double *)(m_data_data), m_data_size);
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;
			roundM_vec_c((double complex *)(output_data_data), (double complex *)(m_data_data), m_data_size);
			break;
	}
	return(output);
}

Matrix * roundM_ref(Matrix* restrict m)
// Returns the elementwise round of Matrix m
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = full_duplicateM_ref(m);
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(round(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = ((double complex)(round((creal(((double complex *)(m_data_data))[i]))))) + ((double complex)(round((cimag(((double complex *)(m_data_data))[i])))))*I;
			}
			break;
	}
	return(output);
}

Matrix * roundM(Matrix* restrict m)
// Returns the elementwise round of Matrix m
{
    if (MODE == VEC) {return roundM_vec(m);}
    else if (MODE == REF) {return roundM_ref(m);}

	// Default mode
	// TODO: change once roundM_vec works
    return roundM_ref(m);
}



void scaleM_vec_dd(double* restrict y, double* restrict x, double a, int n) {	
	// Scale an input double vector x of length n by a double a and return a double vector y
	
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
	int i;

 	long trueVL = vsetvl_e64m8(n); //PLCT

 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i); //PLCT

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);
		vfloat64m8_t v_res = vfmul_vf_f64m8(v_x, a, trueVL);
		vse64_v_f64m8(&y[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void scaleM_vec_ii(int* restrict y, int* restrict x, int a, int n) {
	// Scale an input int vector x of length n by a int a and return a int vector y or NULL
	// Does not account for overflow. Only call after verifying it won't overflow

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vint32m8_t v_res = vmul_vx_i32m8(v_x, a, trueVL); // Multiply the doubles by a
		vse32_v_i32m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void scaleM_vec_id(double* restrict y, int* restrict x, double a, int n) {
	// Scale an input int vector x of length n by a double a and return a double vector y
 	
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t d_v_x = vfwcvt_f_x_v_f64m8(v_x, trueVL); // Cast the ints to doubles
		vfloat64m8_t v_res = vfmul_vf_f64m8(d_v_x, a, trueVL); // Multiply the doubles by a
		vse64_v_f64m8(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void scaleM_vec_ic(double complex* restrict y, int* restrict x, double complex a, int n) {
	// Scale an input int vector x of length n by a complex a and return a complex vector y

	double a_r = creal(a);
	double a_i = cimag(a);
 	
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are complex (SEW 2 x e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t d_v_x = vfwcvt_f_x_v_f64m8(v_x, trueVL); // Cast the ints to doubles
		vfloat64m8_t v_res_r = vfmul_vf_f64m8(d_v_x, a_r, trueVL); // Multiply the doubles by the real part of a
		vfloat64m8_t v_res_i = vfmul_vf_f64m8(d_v_x, a_i, trueVL); // Multiply the doubles by the imaginary part of a

		// Store the real parts as doubles in the complex output, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i]), 16, v_res_r, trueVL); 

		// Store the imaginary parts as doubles in the complex, offset for imaginaries, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, v_res_i, trueVL); 

		i += trueVL;
	}

	FENCE();
}

void scaleM_vec_di(double* restrict y, double* restrict x, int a, int n) {
	// Scale an input double vector x of length n by a int a and return a double vector y

	double d_a = (double) a;
	scaleM_vec_dd(y, x, d_a, n);
}

void scaleM_vec_dc(double complex* restrict y, double* restrict x, double complex a, int n) {
	// Scale an input double vector x of length n by a complex a and return a complex vector y

	double a_r = creal(a);
	double a_i = cimag(a);
 	
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are complex (SEW 2 x e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res_r = vfmul_vf_f64m8(v_x, a_r, trueVL); // Multiply the doubles by the real part of a
		vfloat64m8_t v_res_i = vfmul_vf_f64m8(v_x, a_i, trueVL); // Multiply the doubles by the imaginary part of a

		// Store the real parts as doubles in the complex output, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i]), 16, v_res_r, trueVL); 

		// Store the imaginary parts as doubles in the complex, offset for imaginaries, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, v_res_i, trueVL); 

		i += trueVL;
	}

	FENCE();
}

void scaleM_vec_ci(double complex* restrict y, double complex* restrict x, int a, int n) {
	// Scale an input complex vector x of length n by a int a and return a complex vector y

	scaleM_vec_di(((double *)(y)), ((double *)(x)), a, 2*n);
}

void scaleM_vec_cd(double complex* restrict y, double complex* restrict x, double a, int n) {
	// Scale an input complex vector x of length n by a double a and return a complex vector y

	scaleM_vec_dd(((double *)(y)), ((double *)(x)), a, 2*n);
}

void scaleM_vec_cc(double complex* restrict y, double complex* restrict x, double complex a, int n) {
	// Scale an input complex vector x of length n by a complex a and return a complex vector y

	double a_r = creal(a);
	double a_i = cimag(a);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are complex (SEW 2 x e64)
	int i;

 	long trueVL = vsetvl_e64m8(n);

 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); // Load the real parts
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL); // Load the imaginary parts
		
		// real part = a_r * v_x_r - a_i * v_x_i
		vfloat64m8_t v_res_r = vfmul_vf_f64m8(v_x_r, a_r, trueVL); // a_r * v_x_r
		v_res_r = vfnmsac_vf_f64m8(v_res_r, a_i, v_x_i, trueVL); // v_res_r -= a_i * v_x_i

		// Store the real parts as doubles in the complex output, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i]), 16, v_res_r, trueVL); 

		// imag part = a_i * v_x_r + a_r * v_x_i
		vfloat64m8_t v_res_i = vfmul_vf_f64m8(v_x_r, a_i, trueVL); // a_i * v_x_r
		v_res_i = vfmacc_vf_f64m8(v_res_i, a_r, v_x_i, trueVL); // v_res_r += a_r * v_x_i

		// Store the imaginary parts as doubles in the complex, offset for imaginaries, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, v_res_i, trueVL); 

		i += trueVL;
	}

	FENCE();
}

Matrix * scaleM_vec(Matrix* restrict m, void* restrict scalar, int type)
// Multiplies a scalar by a matrix
{
	if (m == NULL || scalar == NULL)
	{
		return(NULL);
	}

	if (type < 0 || type > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	
	switch(m->data->datatype)
	{
		case INTEGER:
			switch(type)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					scaleM_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), (double)(*(int *)scalar), m_data_size );

					// If no overflow, redo as integer
					if (!containsOverflow(((double *)(output_data_data)), m_data_size)) 
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, INTEGER);
						output_data_data = output->data->data;
						scaleM_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), (*(int *)scalar), m_data_size );
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					scaleM_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), (*(double *)scalar), m_data_size );
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					scaleM_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), (*(double complex *)scalar), m_data_size );
					break;
			}
			break;

		case DOUBLE:
			switch(type)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					scaleM_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), (*(int *)scalar), m_data_size );

					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					scaleM_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), (*(double *)scalar), m_data_size );
					
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					scaleM_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), (*(double complex *)scalar), m_data_size );
					break;
			}
			break;

		case COMPLEX:
			switch(type)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					scaleM_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), (*(int *)scalar), m_data_size );
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					scaleM_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), (*(double *)scalar), m_data_size );
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					scaleM_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), (*(double complex *)scalar), m_data_size );
					break;
			}
			break;
	}

	return(output);
}

Matrix * scaleM_ref(Matrix* restrict m, void* restrict scalar, int type)
// Multiplies a scalar by a matrix
{
	if (m == NULL || scalar == NULL)
	{
		return(NULL);
	}

	if (type < 0 || type > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(type)
			{
				case INTEGER: ; 
					bool overflow = false;
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double product = (double)(((int *)(m_data_data))[i]) * (double)(*(int *)scalar);
						if (product < INT_MAX && product > INT_MIN)
						{
							((int *)(output_data_data))[i] = (int) product;
						}
						else
						{
							overflow = true;
							break;
						}
					}

					if (overflow)
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = (double)(((int *)(m_data_data))[i]) * (double)(*(int *)scalar);
						}
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((int *)(m_data_data))[i] * (*(double *)scalar);
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((int *)(m_data_data))[i] * (*(double complex *)scalar);
					}
					break;
			}
			break;

		case DOUBLE:
			switch(type)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] * (*(int *)scalar);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;	
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] * (*(double *)scalar);
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double *)(m_data_data))[i] * (*(double complex *)scalar);
					}
					break;
			}
			break;

		case COMPLEX:
			switch(type)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * (*(int *)scalar);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * (*(double *)scalar);
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * (*(double complex *)scalar);
					}
					break;
			}
			break;
	}

	return(output);
}

Matrix * scaleM(Matrix* restrict m, void* restrict scalar, int type)
{
    if (MODE == VEC) {return scaleM_vec(m, scalar, type);}
    else if (MODE == REF) {return scaleM_ref(m, scalar, type);}

	// Default mode
    return scaleM_vec(m, scalar, type);
}

// TODO: Vectorize
Matrix * scalarpowerM(Matrix* restrict m, void* restrict exponent, int type)
// Element-wise power (m ^ n) of matrices m to the nth power, where n is a scalar.
// output[i,j] = (m[i,j])^n
{
	if (m == NULL || exponent == NULL)
	{
		return(NULL);
	}

	if (type < 0 || type > 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid type\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(type)
			{
				case INTEGER: ; 
					// Anything to 0th power is 1
					// I'm defining 0^0 = 1
					if ( almostEqual_ii(0, *(int *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
					}

					// Int to negative int power will be double
					else if (*(int *)exponent < 0)
					{
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;

						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow((double)(((int *)(m_data_data))[i]), (double)(*(int *)exponent));
						}
					}

					// Int to positve int power will be int, unless overflow
					else
					{
						bool double_overflow = false;
						
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							double num = pow((double)(((int *)(m_data_data))[i]), (double)(*(int *)exponent));
							if (num > INT_MAX)
							{
								double_overflow = true;
								break;
							}
						}

						if (double_overflow)
						{
							output = createM(m->ndim, m->dim, DOUBLE);
							output_data_data = output->data->data;

							for (register int i = 0 ; i < m_data_size ; i++)
							{								
								((double *)(output_data_data))[i] = pow((double)(((int *)(m_data_data))[i]), (double)(*(int *)exponent));
							}

						}

						else
						{
							output = createM(m->ndim, m->dim, INTEGER);
							output_data_data = output->data->data;

							for (register int i = 0 ; i < m_data_size ; i++)
							{
								((int *)(output_data_data))[i] = (int) pow((double)(((int *)(m_data_data))[i]), (double)(*(int *)exponent));
							}
						}
					}
					break;

				case DOUBLE: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_id(0, *(double *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					// Negative int to double power will be complex
					bool contains_negatives = false;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if (((int *)(m_data_data))[i] < 0)
						{
							contains_negatives = true;
							break;
						}
					}

					if (contains_negatives)
					{
						output = createM(m->ndim, m->dim, COMPLEX);
						output_data_data = output->data->data;

						for (register int i = 0 ; i < m_data_size ; i++)
						{
							if (((int *)(m_data_data))[i] < 0)
							{
								((double complex *)(output_data_data))[i] = (double complex) pow((double complex)(((int *)(m_data_data))[i]), (double complex)(*(double *)exponent));
							}
							else
							{
								((double complex *)(output_data_data))[i] = (double complex) pow((double)(((int *)(m_data_data))[i]), (*(double *)exponent));
							}
						}
					}

					else
					{
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;

						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow((double)(((int *)(m_data_data))[i]), (*(double *)exponent));
						}
					}
					
					break;

				case COMPLEX: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_ic(0, *(double complex *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow((double complex)(((int *)(m_data_data))[i]), (*(double complex *)exponent));
					}
					
					break;
			}
			break;

		case DOUBLE:
			switch(type)
			{
				case INTEGER: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_ii(0, *(int *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					// Double to int power will be double
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = pow((((double *)(m_data_data))[i]), (double)((*(int *)exponent)));
					}
					break;

				case DOUBLE: ;

					// I'm defining 0^0 = 1
					if ( almostEqual_id(0, *(double *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					// Negative double to double power will be complex
					bool contains_negatives = false;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < 0  && !almostEqual_id(0, ((double *)(m_data_data))[i]) )
						{
							contains_negatives = true;
							break;
						}
					}

					if (contains_negatives)
					{
						output = createM(m->ndim, m->dim, COMPLEX);
						output_data_data = output->data->data;

						for (register int i = 0 ; i < m_data_size ; i++)
						{
							if (((double *)(m_data_data))[i] < 0)
							{
								((double complex *)(output_data_data))[i] = pow((double complex)(((double *)(m_data_data))[i]), (double complex)(*(double *)exponent));
							}
							else
							{
								((double complex *)(output_data_data))[i] = (double complex) pow((double)(((double *)(m_data_data))[i]), (*(double *)exponent));
							}
						}
					}

					else
					{
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;

						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow((double)(((double *)(m_data_data))[i]), (*(double *)exponent));
						}
					}

					
					break;

				case COMPLEX: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_ic(0, *(double complex *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow((double complex)(((double *)(m_data_data))[i]), (*(double complex *)exponent));
					}
					
					break;
			}
			break;

		case COMPLEX:
			switch(type)
			{
				case INTEGER: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_ii(0, *(int *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow((double complex)(((double complex *)(m_data_data))[i]), (double complex)(*(int *)exponent));
					}
					
					break;

				case DOUBLE: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_id(0, *(double *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow((double complex)(((double complex *)(m_data_data))[i]), (double complex)(*(double *)exponent));
					}
					
					break;

				case COMPLEX: ;
					// I'm defining 0^0 = 1
					if ( almostEqual_ic(0, *(double complex *)exponent) )
					{
						output = onesM_ref(m->ndim, m->dim);
						break;
					}

					
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;

					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow((double complex)(((double complex *)(m_data_data))[i]), (*(double complex *)exponent));
					}
					
					break;
			}
			break;
	}

	return(output);
}

//TODO: vectorize
Matrix * sinM(Matrix* restrict m)
// Element-wise sine of matrix M
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(sin(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(sin(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = sin(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

//TODO: vectorize
Matrix * sindM(Matrix* restrict m)
// Element-wise sine of matrix M, where the elements of m are in degrees
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(sind(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(sind(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = sind(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}
/*
void cosM_vec_d(double* restrict y, double* restrict x, int n)
// Element-wise cosine, where the elements of x are in radians
{
 	long trueVL = vsetvl_e64m1(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m1(n - i);

		vfloat64m1_t v_x = vle64_v_f64m1(&x[i], trueVL); // Load the doubles from the input

		vfloat64m1_t v_res = _MM_COS_f64(v_x, trueVL); // Cosine

		vse64_v_f64m1(&y[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}
*/
//TODO: vectorize
Matrix * cosM_vec(Matrix* restrict m)
// Element-wise cosine of matrix M
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cos(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;
            for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cos(((double *)(m_data_data))[i]));
			}
            // cosM_vec_d(((double *)(output_data_data)), ((double *)(m_data_data)), m_data_size);
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = cos(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

Matrix * cosM_ref(Matrix* restrict m)
// Element-wise cosine of matrix M
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cos(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cos(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = cos(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

Matrix * cosM(Matrix* restrict m)
// Element-wise cosine of matrix M
{
    if (MODE == REF){return(cosM_ref(m));}
    else if (MODE == VEC){return(cosM_vec(m));}

    // Default case
	return(cosM_ref(m));
}

//TODO: vectorize
Matrix * cosdM(Matrix* restrict m)
// Element-wise cosine of matrix M, where the elements of m are in degrees
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cosd(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(cosd(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = cosd(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

//TODO: vectorize
Matrix * tanM(Matrix* restrict m)
// Element-wise tangent of matrix M
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(tan(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(tan(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = tan(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}

//TODO: vectorize
Matrix * tandM(Matrix* restrict m)
// Element-wise tangent of matrix M, where the elements of m are in degrees
{
	if (m == NULL)
	{
		return(NULL);
	}

	Matrix *output = NULL;
	void *output_data_data = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;

	switch(m->data->datatype)
	{
		case INTEGER: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(tand(((int *)(m_data_data))[i]));
			}
			break;

		case DOUBLE: ;
			output = createM(m->ndim, m->dim, DOUBLE);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double *)(output_data_data))[i] = (double)(tand(((double *)(m_data_data))[i]));
			}
			break;

		case COMPLEX: ;
			output = createM(m->ndim, m->dim, COMPLEX);
			output_data_data = output->data->data;

			for (register int i = 0 ; i < m_data_size ; i++)
			{
				((double complex *)(output_data_data))[i] = tand(((double complex *)(m_data_data))[i]);
			}
			break;
	}

	return(output);
}









/*
* =========================================
* 
*  Matrix-Matrix element-wise math operations
* 
* =========================================
*/

void plusM_internal_vec_dii(double* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise addition of integer vector x and integer vector y stored into double vector z
// Useful to check overflow first
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the ints from the input
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); 

        // Cast the ints to doubles
		vfloat64m8_t v_res = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); 

        // Load the ints from the input
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); 

        // Cast the ints to doubles
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); 

        // Add
		v_res = vfadd_vv_f64m8(v_res, v_y, trueVL);

        // Store
		vse64_v_f64m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise addition of integer vector x and integer vector y stored into integer vector z
// Does not account for overflow. Only call after verifying it won't overflow
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Load the ints from the input
		vint32m8_t v_res = vle32_v_i32m8(&x[i], trueVL);

        // Load the ints from the input 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 

        // Addition
		v_res = vadd_vv_i32m8(v_res, v_y, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL); 

		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise addition of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_res = vle64_v_f64m8(&x[i], trueVL); 

         // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL);

        // Addition
		v_res = vfadd_vv_f64m8(v_res, v_y, trueVL);
        
        // Store
		vse64_v_f64m8(&z[i], v_res, trueVL); 

		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
// Elementwise addition of complex vector x and complex vector y stored into complex vector z
{
	plusM_internal_vec_dd(((double *)(z)), ((double *)(x)), ((double *)(y)), 2*n);
}

void plusM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

         // Load the ints from the input
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);

        // Cast the ints to doubles
		vfloat64m8_t v_res = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); 

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // Addition
		v_res = vfadd_vv_f64m8(v_res, v_y, trueVL);

        // Store
		vse64_v_f64m8(&z[i], v_res, trueVL); 

		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL); 

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_y_i, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load the ints from x
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); 

        // Cast the ints to doubles
		vfloat64m8_t v_res = vfwcvt_f_x_v_f64m8(v_x_int, trueVL); 

        // Addition
		v_res = vfadd_vv_f64m8(v_res, v_y_r, trueVL);

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL); 

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_y_i, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load the doubles from the input
		vfloat64m8_t v_res = vle64_v_f64m8(&x[i], trueVL); 

        // Addition
		v_res = vfadd_vv_f64m8(v_res, v_y_r, trueVL); 

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

void plusM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	plusM_internal_vec_id(z, y, x, n);
}

void plusM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	plusM_internal_vec_ic(z, y, x, n);
}

void plusM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	plusM_internal_vec_dc(z, y, x, n);
}

Matrix * plusM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					plusM_internal_vec_dii(((double *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);

					// If no overflow, redo as integer
					if (!containsOverflow(((double *)(output_data_data)), m_data_size)) 
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, INTEGER);
						output_data_data = output->data->data;
						plusM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					plusM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					plusM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					plusM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					plusM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					plusM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					plusM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					plusM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					plusM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}

	return(output);
}

Matrix * plusM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					bool overflow = false;
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double sum = (double)((int *)(m_data_data))[i] + (double)((int *)(n_data_data))[i];
						if (sum < INT_MAX && sum > INT_MIN)
						{
							((int *)(output_data_data))[i] = (int) sum;
						}
						else
						{
							overflow = true;
							break;
						}
					}

					if (overflow)
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, DOUBLE);	
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = (double)((int *)(m_data_data))[i] + (double)((int *)(n_data_data))[i];
						}
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((int *)(m_data_data))[i] + ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((int *)(m_data_data))[i])) + ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] + ((int *)(n_data_data))[i];
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] + ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((double *)(m_data_data))[i])) + ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] + ((double complex)(((int *)(n_data_data))[i]));
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] + ((double complex)(((double *)(n_data_data))[i]));
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] + ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;
	}

	return(output);
}

Matrix * plusM_vec(Matrix *m, Matrix *n)
// Element-wise addition (+) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		int two = 2;
		return(scaleM_vec(m, &two, INTEGER));
	}
	return(plusM_internal_vec(m, n));
}

Matrix * plusM_ref(Matrix *m, Matrix *n)
// Element-wise addition (+) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		int two = 2;
		return(scaleM_ref(m, &two, INTEGER));
	}
	return(plusM_internal_ref(m, n));
}

Matrix * plusM(Matrix *m, Matrix *n)
// Element-wise addition (+) of matrices m and n.
{
    if (MODE == VEC) {return plusM_vec(m, n);}
    else if (MODE == REF) {return plusM_ref(m, n);}

	// Default mode
    return plusM_vec(m, n);
}



void minusM_internal_vec_dii(double* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise subtraction of integer vector x and integer vector y stored into double vector z
// Useful to check overflow first
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_res = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		v_res = vfsub_vv_f64m8(v_res, v_y, trueVL); // subtraction
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise subtraction of integer vector x and integer vector y stored into integer vector z
// Does not account for overflow. Only call after verifying it won't overflow
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); // Load the ints from the input
		vint32m8_t v_res = vsub_vv_i32m8(v_x, v_y, trueVL);
		vse32_v_i32m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise subtraction of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfsub_vv_f64m8(v_x, v_y, trueVL); // subtraction
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
// Elementwise subtraction of complex vector x and complex vector y stored into complex vector z
{
	minusM_internal_vec_dd(((double *)(z)), ((double *)(x)), ((double *)(y)), 2*n);
}

void minusM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfsub_vv_f64m8(v_x, v_y, trueVL); // subtraction
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL); 
		v_y_i = vfsgnjn_vv_f64m8(v_y_i, v_y_i, trueVL); // Negate the imaginary parts


		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_y_i, trueVL);



		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_res = vfwcvt_f_x_v_f64m8(v_x_int, trueVL); // Cast the ints to doubles
		v_res = vfsub_vv_f64m8(v_res, v_y_r, trueVL); // subtraction

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL); 
		v_y_i = vfsgnjn_vv_f64m8(v_y_i, v_y_i, trueVL); // Negate the imaginary parts

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_y_i, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

		vfloat64m8_t v_res = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		v_res = vfsub_vv_f64m8(v_res, v_y_r, trueVL); // subtraction

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfsub_vv_f64m8(v_x, v_y, trueVL); // subtraction
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL); 


		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_x_i, trueVL);



		// Load the real parts from x
		vfloat64m8_t v_res = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

		vint32m4_t v_y_int = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_int, trueVL); // Cast the ints to doubles
		v_res = vfsub_vv_f64m8(v_res, v_y, trueVL); // subtraction

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

void minusM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL); 

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_x_i, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfsub_vv_f64m8(v_x_r, v_y, trueVL); // subtraction

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res, trueVL);
		i += trueVL;
	}

	FENCE();
}

Matrix * minusM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					minusM_internal_vec_dii(((double *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);

					// If no overflow, redo as integer
					if (!containsOverflow(((double *)(output_data_data)), m_data_size)) 
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, INTEGER);
						output_data_data = output->data->data;
						minusM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					minusM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					minusM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					minusM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					minusM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					minusM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					minusM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					minusM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					minusM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}

	return(output);
}

Matrix * minusM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					bool overflow = false;
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double difference = (double)((int *)(m_data_data))[i] - (double)((int *)(n_data_data))[i];
						if (difference < INT_MAX && difference > INT_MIN)
						{
							((int *)(output_data_data))[i] = (int) difference;
						}
						else
						{
							overflow = true;
							break;
						}
					}

					if (overflow)
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;	
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = (double)((int *)(m_data_data))[i] - (double)((int *)(n_data_data))[i];
						}
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((int *)(m_data_data))[i] - ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((int *)(m_data_data))[i])) - ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] - ((int *)(n_data_data))[i];
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] - ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((double *)(m_data_data))[i])) - ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] - ((double complex)(((int *)(n_data_data))[i]));
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] - ((double complex)(((double *)(n_data_data))[i]));
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] - ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;
	}

	return(output);
}

Matrix * minusM_vec(Matrix *m, Matrix *n)
// Element-wise subtraction (-) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(zerosM(m->ndim, m->dim));
	}
	return(minusM_internal_vec(m, n));
}

Matrix * minusM_ref(Matrix *m, Matrix *n)
// Element-wise subtraction (-) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(zerosM(m->ndim, m->dim));
	}
	return(minusM_internal_ref(m, n));
}

Matrix * minusM(Matrix *m, Matrix *n)
// Element-wise subtraction (-) of matrices m and n.
{
    if (MODE == VEC) {return minusM_vec(m, n);}
    else if (MODE == REF) {return minusM_ref(m, n);}

	// Default mode
    return minusM_vec(m, n);
}







void timesM_internal_vec_dii(double* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise multiplication of integer vector x and integer vector y stored into double vector z
// Useful to check overflow first
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_res = vfmul_vv_f64m8(v_x, v_y, trueVL); // multiplication
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise multiplication of integer vector x and integer vector y stored into integer vector z
// Does not account for overflow. Only call after verifying it won't overflow
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); // Load the ints from the input
		vint32m8_t v_res = vmul_vv_i32m8(v_x, v_y, trueVL);
		vse32_v_i32m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise multiplication of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmul_vv_f64m8(v_x, v_y, trueVL); // multiplication
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
// Elementwise multiplication of complex vector x and complex vector y stored into complex vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// // Real part multiplication
		// vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x_r, v_y_r, trueVL);
		// v_res_r = vfnmsac_vv_f64m8(v_res_r, v_x_i, v_y_i, trueVL);

        // NOTE / KNOWN BUG:
        // The above real-part-multiplication code calculates the real part of the 
        // complex-complex multiply output by first multiplying the real parts together, 
        // then using vfnmsac_vv_f64m8, which mutiplies the imaginary parts together, 
        // negates them, and adds them to the multiplied real parts.
        // The code below first multiplies the imaginary parts together, negates them,
        // then multiplies the real parts together and adds them to the negated, multiplied
        // imaginary parts.
        // Mathematically, these two are equivalent. However, the first method gives slight 
        // rounding differences when compared with complex number multiplication as defined in tgmath.h 
        // The code below does the operation exactly as tgmath.h does, and so it is preferred
        // despite calling one more operation.
        
        // Real part multiplication
		vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x_i, v_y_i, trueVL);
        v_res_r = vfsgnjn_vv_f64m8(v_res_r, v_res_r, trueVL); // Negate the imaginary parts
		v_res_r = vfmacc_vv_f64m8(v_res_r, v_x_r, v_y_r, trueVL);

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Imaginary part multiplication
		vfloat64m8_t v_res_i = vfmul_vv_f64m8(v_x_i, v_y_r, trueVL); 
		v_res_i = vfmacc_vv_f64m8(v_res_i, v_x_r, v_y_i, trueVL);
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmul_vv_f64m8(v_x, v_y, trueVL); // multiplication
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_int, trueVL); // Cast the ints to doubles

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x, v_y_r, trueVL); // real part multiplication

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		vfloat64m8_t v_res_i = vfmul_vv_f64m8(v_x, v_y_i, trueVL); // imaginary part multiplication
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x, v_y_r, trueVL); // real part multiplication

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		vfloat64m8_t v_res_i = vfmul_vv_f64m8(v_x, v_y_i, trueVL); // imaginary part multiplication
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void timesM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	timesM_internal_vec_id(z, y, x, n);
}

void timesM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	timesM_internal_vec_ic(z, y, x, n);
}

void timesM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	timesM_internal_vec_dc(z, y, x, n);
}

Matrix * timesM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					timesM_internal_vec_dii(((double *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);

					// If no overflow, redo as integer
					if (!containsOverflow(((double *)(output_data_data)), m_data_size)) 
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, INTEGER);
						output_data_data = output->data->data;
						timesM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					timesM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					timesM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					timesM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					timesM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					timesM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					timesM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					timesM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					timesM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}

	return(output);
}



Matrix * timesM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					bool overflow = false;
					output = createM(m->ndim, m->dim, INTEGER);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double product = (double)((int *)(m_data_data))[i] * (double)((int *)(n_data_data))[i];
						if (product < INT_MAX && product > INT_MIN)
						{
							((int *)(output_data_data))[i] = (int) product;
						}
						else
						{
							overflow = true;
							break;
						}
					}

					if (overflow)
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = (double)((int *)(m_data_data))[i] * (double)((int *)(n_data_data))[i];
						}
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((int *)(m_data_data))[i] * ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((int *)(m_data_data))[i])) * ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] * ((int *)(n_data_data))[i];
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] * ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((double *)(m_data_data))[i])) * ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * ((double complex)(((int *)(n_data_data))[i]));
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * ((double complex)(((double *)(n_data_data))[i]));
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] * ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;
	}

	return(output);
}

Matrix * timesM_vec(Matrix *m, Matrix *n)
// Element-wise multiplication (*) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		int two = 2;
		// TODO: change to scalarpowerM_vec once written
		return(scalarpowerM(m, &two, INTEGER));
	}
	return(timesM_internal_vec(m, n));
}

Matrix * timesM_ref(Matrix *m, Matrix *n)
// Element-wise multiplication (*) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		int two = 2;
		// TODO: change to scalarpowerM_ref once written
		return(scalarpowerM(m, &two, INTEGER));
	}
	return(timesM_internal_ref(m, n));
}

Matrix * timesM(Matrix *m, Matrix *n)
// Element-wise multiplication (*) of matrices m and n.
{
    if (MODE == VEC) {return timesM_vec(m, n);}
    else if (MODE == REF) {return timesM_ref(m, n);}

	// Default mode
    return timesM_vec(m, n);
}





void rdivideM_internal_vec_ii(double* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise right division of integer vector x and integer vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_res = vfdiv_vv_f64m8(v_x, v_y, trueVL); // right division
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise right division of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfdiv_vv_f64m8(v_x, v_y, trueVL); // right division
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
// Elementwise right division of complex vector x and complex vector y stored into complex vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		
		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Magnitude of y
		vfloat64m8_t v_y_mag = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL);
		v_y_mag = vfmacc_vv_f64m8(v_y_mag, v_y_i, v_y_i, trueVL);
		

		// Real part calculation
		vfloat64m8_t v_res_r_numerator = vfmul_vv_f64m8(v_x_r, v_y_r, trueVL);
		v_res_r_numerator = vfmacc_vv_f64m8(v_res_r_numerator, v_x_i, v_y_i, trueVL);
		vfloat64m8_t v_res_r = vfdiv_vv_f64m8(v_res_r_numerator, v_y_mag, trueVL);

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Imaginary part calculation
		vfloat64m8_t v_res_i_numerator = vfmul_vv_f64m8(v_x_i, v_y_r, trueVL); 
		v_res_i_numerator = vfnmsac_vv_f64m8(v_res_i_numerator, v_x_r, v_y_i, trueVL);
		vfloat64m8_t v_res_i = vfdiv_vv_f64m8(v_res_i_numerator, v_y_mag, trueVL);
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfdiv_vv_f64m8(v_x, v_y, trueVL); // right division
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the x ints and cast to double
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Magnitude of y
		vfloat64m8_t v_y_mag = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL);
		v_y_mag = vfmacc_vv_f64m8(v_y_mag, v_y_i, v_y_i, trueVL);
		
		// Negate the imaginary parts
		vfloat64m8_t v_y_i_neg = vfsgnjn_vv_f64m8(v_y_i, v_y_i, trueVL); // Negate the imaginary parts

		// Divide out the magnitude
		vfloat64m8_t v_y_r_final = vfdiv_vv_f64m8(v_y_r, v_y_mag, trueVL);
		vfloat64m8_t v_y_i_final = vfdiv_vv_f64m8(v_y_i_neg, v_y_mag, trueVL);
		
		// Calculate the real part
		vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x, v_y_r_final, trueVL);

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Calculate the imaginary part
		vfloat64m8_t v_res_i = vfmul_vv_f64m8(v_x, v_y_i_final, trueVL);

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the x doubles
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

		// Load the imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Magnitude of y
		vfloat64m8_t v_y_mag = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL);
		v_y_mag = vfmacc_vv_f64m8(v_y_mag, v_y_i, v_y_i, trueVL);
		
		// Negate the imaginary parts
		vfloat64m8_t v_y_i_neg = vfsgnjn_vv_f64m8(v_y_i, v_y_i, trueVL);

		// Divide out the magnitude
		vfloat64m8_t v_y_r_final = vfdiv_vv_f64m8(v_y_r, v_y_mag, trueVL);
		vfloat64m8_t v_y_i_final = vfdiv_vv_f64m8(v_y_i_neg, v_y_mag, trueVL);
		
		// Calculate the real part
		vfloat64m8_t v_res_r = vfmul_vv_f64m8(v_x, v_y_r_final, trueVL);

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Calculate the imaginary part
		vfloat64m8_t v_res_i = vfmul_vv_f64m8(v_x, v_y_i_final, trueVL);

		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_res = vfdiv_vv_f64m8(v_x, v_y, trueVL); // right division
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_res_r = vfdiv_vv_f64m8(v_x_r, v_y, trueVL); // real part right division

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

		vfloat64m8_t v_res_i = vfdiv_vv_f64m8(v_x_i, v_y, trueVL); // imaginary part right division
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

void rdivideM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input

		vfloat64m8_t v_res_r = vfdiv_vv_f64m8(v_x_r, v_y, trueVL); // real part right division

		// Store the real parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);

		// Load the imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

		vfloat64m8_t v_res_i = vfdiv_vv_f64m8(v_x_i, v_y, trueVL); // imaginary part right division
 
		// Store the imaginary parts in z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * rdivideM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					rdivideM_internal_vec_ii(((double *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					rdivideM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					rdivideM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					rdivideM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					rdivideM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					rdivideM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					rdivideM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					rdivideM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					rdivideM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}

	return(output);
}

Matrix * rdivideM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = (double)(((int *)(m_data_data))[i]) / (double)(((int *)(n_data_data))[i]);
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((int *)(m_data_data))[i] / ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((int *)(m_data_data))[i])) / ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] / ((int *)(n_data_data))[i];
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = ((double *)(m_data_data))[i] / ((double *)(n_data_data))[i];
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex)(((double *)(m_data_data))[i])) / ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] / ((double complex)(((int *)(n_data_data))[i]));
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] / ((double complex)(((double *)(n_data_data))[i]));
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = ((double complex *)(m_data_data))[i] / ((double complex *)(n_data_data))[i];
					}
					break;
			}
			break;
	}

	return(output);
}



Matrix * rdivideM_vec(Matrix *m, Matrix *n)
// Element-wise right division (/) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(onesM_vec(m->ndim, m->dim));
	}
	return(rdivideM_internal_vec(m, n));
}

Matrix * rdivideM_ref(Matrix *m, Matrix *n)
// Element-wise right division (/) of matrices m and n.
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		return(onesM_ref(m->ndim, m->dim));
	}
	return(rdivideM_internal_ref(m, n));
}

Matrix * rdivideM(Matrix *m, Matrix *n)
// Element-wise right division (/) of matrices m and n.
{
    if (MODE == VEC) {return rdivideM_vec(m, n);}
    else if (MODE == REF) {return rdivideM_ref(m, n);}

	// Default mode
    return rdivideM_vec(m, n);
}

Matrix * ldivideM_vec(Matrix *m, Matrix *n)
// Element-wise left division (\) of matrices m and n.
{
	return(rdivideM_vec(n,m));
}

Matrix * ldivideM_ref(Matrix *m, Matrix *n)
// Element-wise left division (\) of matrices m and n.
{
	return(rdivideM_ref(n,m));
}

Matrix * ldivideM(Matrix *m, Matrix *n)
// Element-wise left division (\) of matrices m and n.
{
	return(rdivideM(n,m));
}

Matrix * powerM_internal(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = NULL;

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = NULL;

	switch(m->data->datatype)
	{
		case INTEGER:
			switch(n->data->datatype)
			{
				case INTEGER: ; 
					bool overflow = false;
					output = createM(m->ndim, m->dim, INTEGER);
					output_data_data = output->data->data;	
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double product = pow(((int *)(m_data_data))[i] , ((int *)(n_data_data))[i]);
						if (product < INT_MAX && product > INT_MIN)
						{
							((int *)(output_data_data))[i] = (int) product;
						}
						else
						{
							overflow = true;
							break;
						}
					}

					if (overflow)
					{
						destroyM(output);
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow(((int *)(m_data_data))[i] , ((int *)(n_data_data))[i]);
						}
					}
					break;

				case DOUBLE: ;
					bool complex_overflow = false;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < 0 && !almostEqual_ii(0, ((int *)(m_data_data))[i]) && ((double *)(n_data_data))[i] != floor(((double *)(n_data_data))[i]))
						{
							complex_overflow = true;
							break;
						}
					}

					if (complex_overflow)
					{
						output = createM(m->ndim, m->dim, COMPLEX);
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double complex *)(output_data_data))[i] = pow((double complex)(((int *)(m_data_data))[i]) , (double complex)(((double *)(n_data_data))[i]));
						}
					}
					else
					{
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;	
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow(((int *)(m_data_data))[i] , ((double *)(n_data_data))[i]);
						}
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] =  pow((double complex)(((int *)(m_data_data))[i]) , ((double complex *)(n_data_data))[i]);
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;	
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = pow(((double *)(m_data_data))[i] , ((int *)(n_data_data))[i]);
					}
					break;

				case DOUBLE: ;
					bool complex_overflow = false;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < 0 && !almostEqual_id(0, ((double *)(m_data_data))[i]) && ((double *)(m_data_data))[i] != floor(((double *)(m_data_data))[i]) && ((double *)(n_data_data))[i] != floor(((double *)(n_data_data))[i]))
						{
							complex_overflow = true;
							break;
						}
					}

					if (complex_overflow)
					{
						output = createM(m->ndim, m->dim, COMPLEX);
						output_data_data = output->data->data;	
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double complex *)(output_data_data))[i] = pow((double complex)(((double *)(m_data_data))[i]) , (double complex)(((double *)(n_data_data))[i]));
						}
					}
					else
					{
						output = createM(m->ndim, m->dim, DOUBLE);
						output_data_data = output->data->data;
						for (register int i = 0 ; i < m_data_size ; i++)
						{
							((double *)(output_data_data))[i] = pow(((double *)(m_data_data))[i] , ((double *)(n_data_data))[i]);
						}
					}

					
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;	
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] =  pow((double complex)(((double *)(m_data_data))[i]) , ((double complex *)(n_data_data))[i]);
					}
					break;
			}
			break;

		case COMPLEX:
			output = createM(m->ndim, m->dim, COMPLEX);	
			output_data_data = output->data->data;

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow(((double complex *)(m_data_data))[i] , (double complex)(((int *)(n_data_data))[i]));
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow(((double complex *)(m_data_data))[i] , (double complex)(((double *)(n_data_data))[i]));
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double complex *)(output_data_data))[i] = pow(((double complex *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]);
					}
					break;
			}
			break;
	}

	return(output);
}

// TODO: vectorize
Matrix * powerM(Matrix *m, Matrix *n)
// Element-wise raise m to the nth power.
// output[i,j] = (m[i,j])^(n[i,j])
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	Matrix *n_restricted = n;
	bool free_n = false;
	if (m == n)
	{
		n_restricted = full_duplicateM(m);
		free_n = true;
	}

	Matrix *output = powerM_internal(m, n_restricted);

	// Cleanup
	if (free_n && !destroyM(n_restricted))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
	}

	return(output);
}