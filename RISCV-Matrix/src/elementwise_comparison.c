//
//  elementwise_comparison.c
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


// TODO: Pairwise min/max reference functions involving complex numbers -- also very slow
// They use rudimentary masking to get around having to change their behavior based on the COMPARISON_MODE being MATLAB vs OCTAVE
// Makes them slooooooooow
// TODO: make all the double & complex ne's, ge's, and le's implement almostEqual for the "or-equal-to" part.



void gtM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // GT
        vbool4_t v_mask = vmsgt_vv_i32m8_b4 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void gtM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 
        
        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void gtM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
        
        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);

        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void gtM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_gtM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);
        
        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_gtM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_gtM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);
        
        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_gtM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
        
        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_gtM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // GT
        vbool8_t v_mask = vmfgt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * matlab_gtM_internal_vec(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					gtM_internal_vec_ii(((int*)(output_data_data)), ((int*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					gtM_internal_vec_id(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_gtM_internal_vec_ic(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					gtM_internal_vec_di(((int*)(output_data_data)), ((double*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					gtM_internal_vec_dd(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_gtM_internal_vec_dc(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					matlab_gtM_internal_vec_ci(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					matlab_gtM_internal_vec_cd(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_gtM_internal_vec_cc(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_gtM_internal_ref(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] > ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] > ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] > (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] > ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] > ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] > (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) > ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) > ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) > (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_gtM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are larger 
// m > n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(matlab_gtM_internal_vec(m, n));
}

Matrix * matlab_gtM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are larger 
// m > n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(matlab_gtM_internal_ref(m, n));
}








void octave_gtM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from x & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); 
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for gt : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for gt : if x_cabs == y_cabs, then x_carg > y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_gt = vmfgt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_gt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_gtM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from x
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for gt : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for gt : if x_cabs == y_cabs, then x_carg > y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_gt = vmfgt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_gt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_gtM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the ints from y & cast to doubles
		vint32m4_t   v_y_int = vle32_v_i32m4(&y[i], trueVL); 
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for gt : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for gt : if x_cabs == y_cabs, then x_carg > y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_gt = vmfgt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_gt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_gtM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the doubles from y
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL); 
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for gt : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for gt : if x_cabs == y_cabs, then x_carg > y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_gt = vmfgt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_gt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_gtM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for gt : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for gt : if x_cabs == y_cabs, then x_carg > y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_gt = vmfgt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_gt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
	free(x_carg);
    free(y_carg);
}



Matrix * octave_gtM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					gtM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					gtM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    octave_gtM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					gtM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					gtM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_gtM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					octave_gtM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					octave_gtM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_gtM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_gtM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] > ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] > ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((int *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((int *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg > n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] > ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] > ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg > n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{	
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((int *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((int *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg > n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg > n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg > n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_gtM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are larger 
// m > n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(octave_gtM_internal_ref(m, n));
}

Matrix * octave_gtM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are larger 
// m > n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(octave_gtM_internal_vec(m, n));
}

Matrix * gtM_ref(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_gtM_ref(m,n));
	}
	return(matlab_gtM_ref(m,n));
}

Matrix * gtM_vec(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_gtM_vec(m,n));
	}
	return(matlab_gtM_vec(m,n));
}

Matrix * gtM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are larger 
// m > n
{
    if (MODE == REF){return(gtM_ref(m, n));}
    else if (MODE == VEC){return(gtM_vec(m, n));}

    // Default case
	return(gtM_ref(m, n));
}









void geM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // GE
        vbool4_t v_mask = vmsge_vv_i32m8_b4 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void geM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 
        
        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void geM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
        
        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);

        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void geM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}







void matlab_geM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);
        
        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_geM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_geM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);
        
        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_geM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
        
        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_geM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // GE
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * matlab_geM_internal_vec(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					geM_internal_vec_ii(((int*)(output_data_data)), ((int*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					geM_internal_vec_id(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_geM_internal_vec_ic(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					geM_internal_vec_di(((int*)(output_data_data)), ((double*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					geM_internal_vec_dd(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_geM_internal_vec_dc(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					matlab_geM_internal_vec_ci(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					matlab_geM_internal_vec_cd(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_geM_internal_vec_cc(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_geM_internal_ref(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] >= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] >= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] >= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] >= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] >= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] >= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) >= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) >= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) >= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_geM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_vec(m->ndim, m->dim);
		return(output);
	}

	return(matlab_geM_internal_vec(m, n));
}

Matrix * matlab_geM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_ref(m->ndim, m->dim);
		return(output);
	}

	return(matlab_geM_internal_ref(m, n));
}





void octave_geM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from x & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); 
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for ge : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for ge : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_geM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from x
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for ge : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for ge : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_geM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the ints from y & cast to doubles
		vint32m4_t   v_y_int = vle32_v_i32m4(&y[i], trueVL); 
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for ge : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for ge : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_geM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the doubles from y
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL); 
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for ge : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for ge : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_geM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for ge : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for ge : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
	free(x_carg);
    free(y_carg);
}



Matrix * octave_geM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					geM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					geM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    octave_geM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					geM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					geM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_geM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					octave_geM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					octave_geM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_geM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_geM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] >= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] >= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((int *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((int *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg >= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] >= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] >= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg >= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{	
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((int *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((int *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg >= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg >= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs > n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg >= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_geM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_vec(m->ndim, m->dim);
		return(output);
	}

	return(octave_geM_internal_vec(m, n));
}

Matrix * octave_geM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_ref(m->ndim, m->dim);
		return(output);
	}

	return(octave_geM_internal_ref(m, n));
}

Matrix * geM_vec(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_geM_vec(m,n));
	}
	return(matlab_geM_vec(m,n));
}

Matrix * geM_ref(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_geM_ref(m,n));
	}
	return(matlab_geM_ref(m,n));
}

Matrix * geM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n
{
    if (MODE == REF){return(geM_ref(m, n));}
    else if (MODE == VEC){return(geM_vec(m, n));}

    // Default case
	return(geM_ref(m, n));
}









void ltM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // LT
        vbool4_t v_mask = vmslt_vv_i32m8_b4 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void ltM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 
        
        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void ltM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
        
        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);

        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void ltM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}







void matlab_ltM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);
        
        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_ltM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_ltM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);
        
        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_ltM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
        
        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_ltM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // LT
        vbool8_t v_mask = vmflt_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * matlab_ltM_internal_vec(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					ltM_internal_vec_ii(((int*)(output_data_data)), ((int*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					ltM_internal_vec_id(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_ltM_internal_vec_ic(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					ltM_internal_vec_di(((int*)(output_data_data)), ((double*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					ltM_internal_vec_dd(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_ltM_internal_vec_dc(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					matlab_ltM_internal_vec_ci(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					matlab_ltM_internal_vec_cd(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_ltM_internal_vec_cc(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_ltM_internal_ref(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) < ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) < ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) < (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_ltM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(matlab_ltM_internal_vec(m, n));
}

Matrix * matlab_ltM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(matlab_ltM_internal_ref(m, n));
}

void octave_ltM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from x & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); 
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for lt : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for lt : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_lt = vmflt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_lt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_ltM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from x
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for lt : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for lt : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_lt = vmflt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_lt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_ltM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the ints from y & cast to doubles
		vint32m4_t   v_y_int = vle32_v_i32m4(&y[i], trueVL); 
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for lt : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for lt : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_lt = vmflt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_lt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_ltM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the doubles from y
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL); 
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for lt : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for lt : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_lt = vmflt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_lt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_ltM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for lt : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for lt : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_lt = vmflt_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_lt, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
	free(x_carg);
    free(y_carg);
}

Matrix * octave_ltM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					ltM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					ltM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    octave_ltM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					ltM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					ltM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_ltM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					octave_ltM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					octave_ltM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_ltM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_ltM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] < ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((int *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((int *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg < n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] < ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg < n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{	
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((int *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((int *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg < n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg < n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg < n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
    }
	return(output);
}

Matrix * octave_ltM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(octave_ltM_internal_vec(m, n));
}

Matrix * octave_ltM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(octave_ltM_internal_ref(m, n));
}

Matrix * ltM_vec(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_ltM_vec(m,n));
	}
	return(matlab_ltM_vec(m,n));
}

Matrix * ltM_ref(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_ltM_ref(m,n));
	}
	return(matlab_ltM_ref(m,n));
}

Matrix * ltM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n
{
    if (MODE == REF){return(ltM_ref(m, n));}
    else if (MODE == VEC){return(ltM_vec(m, n));}

    // Default case
	return(ltM_ref(m, n));
}









void leM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); 
        
        // LE
        vbool4_t v_mask = vmsle_vv_i32m8_b4 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void leM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 
        
        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void leM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
        
        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);

        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void leM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}







void matlab_leM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);
        
        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_leM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_leM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL);
        
        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_leM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
        
        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void matlab_leM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // LE
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x, v_y, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * matlab_leM_internal_vec(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					leM_internal_vec_ii(((int*)(output_data_data)), ((int*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					leM_internal_vec_id(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_leM_internal_vec_ic(((int*)(output_data_data)), ((int*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					leM_internal_vec_di(((int*)(output_data_data)), ((double*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					leM_internal_vec_dd(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_leM_internal_vec_dc(((int*)(output_data_data)), ((double*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					matlab_leM_internal_vec_ci(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((int*)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					matlab_leM_internal_vec_cd(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double*)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					matlab_leM_internal_vec_cc(((int*)(output_data_data)), ((double complex*)(m_data_data)),  ((double complex*)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_leM_internal_ref(Matrix* restrict m, Matrix* restrict n) 
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] <= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] <= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] <= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] <= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] <= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] <= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) <= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) <= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( (creal(((double complex *)(m_data_data))[i])) <= (creal(((double complex *)(n_data_data))[i])) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * matlab_leM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal 
// m <= n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_vec(m->ndim, m->dim);
		return(output);
	}

	return(matlab_leM_internal_vec(m, n));
}

Matrix * matlab_leM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal 
// m <= n
// MATLAB method: compares only real parts of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_ref(m->ndim, m->dim);
		return(output);
	}

	return(matlab_leM_internal_ref(m, n));
}








void octave_leM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from x & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL); 
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for le : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for le : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_le = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_le, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_leM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from x
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_x_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get x_cabs
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);

		// Get x_carg
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for le : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for le : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_le = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_le, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(y_carg);
}

void octave_leM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the ints from y & cast to doubles
		vint32m4_t   v_y_int = vle32_v_i32m4(&y[i], trueVL); 
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for le : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for le : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_le = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_le, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_leM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the doubles from y
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL); 
		vfloat64m8_t v_y_i = vfmv_v_f_f64m8(0, trueVL); // Imaginary imaginaries

		// Get y_cabs
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);

		// Get y_carg
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for le : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for le : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_le = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_le, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
    free(x_carg);
}

void octave_leM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

    // 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Primary condition for le : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for le : if x_cabs == y_cabs, then x_carg < y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_le = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8(v_mask_cabs_eq, v_mask_carg_le, trueVL);

        // Put all these masks together...
        vbool8_t v_mask = vmor_mm_b8(v_mask_primary, v_mask_secondary, trueVL);

        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
	free(x_carg);
    free(y_carg);
}


Matrix * octave_leM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					leM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					leM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    octave_leM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					leM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					leM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_leM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					octave_leM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					octave_leM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    octave_leM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_leM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);
			
	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;

	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] <= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((int *)(m_data_data))[i] <= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						// Bafflingly, for some reason, the following is the case:
						// let m = (double complex)(((int *)(m_data_data))[i])  , that is, the specific value we're looking at casted to double complex
						// let n = ((double complex *)(n_data_data))[i])
						// (carg(m) <= carg(n)) -- the result of a comparison operator -- does not return a boolean, it outputs a float (when compiled with the RISC-V compiler)???? 
						// I have no explanation, but it does.
						// Also, if not wrapped in parentheses, it makes the following conditional statement fail. (Likely an issue with operator precedence).
						// Furthermore, we can't simply rely on <= in the case of equality. We have to bring in the almostEqual functions.
						// This is why we assign to variables, which avoids the problem, also for an inexplicable reason.

						double m_cabs = cabs((double complex)(((int *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((int *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg <= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] <= ((int *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( ((double *)(m_data_data))[i] <= ((double *)(n_data_data))[i] )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg <= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{	
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((int *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((int *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg <= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg <= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						double m_cabs = cabs((double complex)(((double complex *)(m_data_data))[i]));
						double n_cabs = cabs((double complex)(((double complex *)(n_data_data))[i]));
						double m_carg = carg((double complex)(((double complex *)(m_data_data))[i]));
						double n_carg = carg((double complex)(((double complex *)(n_data_data))[i]));

						if (
							(m_cabs < n_cabs) ||
							(
							 	almostEqual_dd(m_cabs, n_cabs) &&
							 		(
							 			(m_carg <= n_carg) || almostEqual_dd(m_carg, n_carg)
							 		)
							) 
						   )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * octave_leM_vec(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal 
// m <= n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_vec(m->ndim, m->dim);
		return(output);
	}

	return(octave_leM_internal_vec(m, n));
}

Matrix * octave_leM_ref(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal 
// m <= n
// Octave method: compares magnitudes, then phase angles of complex numbers
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_ref(m->ndim, m->dim);
		return(output);
	}

	return(octave_leM_internal_ref(m, n));
}

Matrix * leM_vec(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_leM_vec(m,n));
	}
	return(matlab_leM_vec(m,n));
}

Matrix * leM_ref(Matrix *m, Matrix *n)
{
	if (COMPARISON_MODE == OCTAVE)
	{
		return(octave_leM_ref(m,n));
	}
	return(matlab_leM_ref(m,n));
}

Matrix * leM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal
// m <= n
{
    if (MODE == REF){return(leM_ref(m, n));}
    else if (MODE == VEC){return(leM_vec(m, n));}

    // Default case
	return(leM_ref(m, n));
}






void equalM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL);

		// eq
        vbool4_t v_mask = vmseq_vv_i32m8_b4 (v_x, v_y, trueVL);

        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void equalM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		vfloat64m8_t v_diff = vfsub_vv_f64m8(v_x, v_y, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_abs = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) <= EPSILON
        vbool8_t v_mask = vmfle_vf_f64m8_b8 (v_diff_abs, EPSILON, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void equalM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		vfloat64m8_t v_diff_r = vfsub_vv_f64m8(v_x_r, v_y_r, trueVL);
		vfloat64m8_t v_diff_i = vfsub_vv_f64m8(v_x_i, v_y_i, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_r_abs = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		vfloat64m8_t v_diff_i_abs = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) <= EPSILON
        vbool8_t v_mask_r = vmfle_vf_f64m8_b8 (v_diff_r_abs, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfle_vf_f64m8_b8 (v_diff_i_abs, EPSILON, trueVL);
		vbool8_t v_mask = vmand_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void equalM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	equalM_internal_vec_id(z, y, x, n);
}
void equalM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		vfloat64m8_t v_diff = vfsub_vv_f64m8(v_x, v_y, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_abs = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) <= EPSILON
        vbool8_t v_mask = vmfle_vf_f64m8_b8 (v_diff_abs, EPSILON, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void equalM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries


        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);
        
        // almostEqual == difference is less than EPSILON set in matrix.h
        vbool8_t v_mask_r = vmfne_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);
        vbool8_t v_mask_i = vmfne_vv_f64m8_b8 (v_x_i, v_y_i, trueVL);
		vbool8_t v_mask = vmand_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void equalM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	equalM_internal_vec_ic(z, y, x, n);
}

void equalM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	equalM_internal_vec_dc(z, y, x, n);
}

void equalM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		vfloat64m8_t v_diff_r = vfsub_vv_f64m8(v_x_r, v_y_r, trueVL);
		vfloat64m8_t v_diff_i = vfsub_vv_f64m8(v_x_i, v_y_i, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_r_abs = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		vfloat64m8_t v_diff_i_abs = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) <= EPSILON
        vbool8_t v_mask_r = vmfle_vf_f64m8_b8 (v_diff_r_abs, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfle_vf_f64m8_b8 (v_diff_i_abs, EPSILON, trueVL);
		vbool8_t v_mask = vmand_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * equalM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					equalM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					equalM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    equalM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					equalM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					equalM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    equalM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					equalM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					equalM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    equalM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * equalM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_ii(((int *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_id(((int *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_ic(((int *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_di(((double *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_dd(((double *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_dc(((double *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_ci(((double complex *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_cd(((double complex *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( almostEqual_cc(((double complex *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * equalM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_vec(m->ndim, m->dim);
		return(output);
	}
	return(equalM_internal_vec(m, n));
}

Matrix * equalM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = onesM_ref(m->ndim, m->dim);
		return(output);
	}
	return(equalM_internal_ref(m, n));
}

Matrix * equalM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether two matrices' corresponding elements are equal (1 = equal)
// m == n
// For doubles & complexes, actually implements almostEqual
// almostEqual == difference is less than or equal to EPSILON set in matrix.h
{
    if (MODE == REF){return(equalM_ref(m, n));}
    else if (MODE == VEC){return(equalM_vec(m, n));}

    // Default case
	return(equalM_ref(m, n));
}






bool isEqualM_internal_vec_ii(int* restrict x, int* restrict y, int n)
{
    bool isEqual = true;

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Load the ints from the input & cast to doubles
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL);

		// ne
        vbool4_t v_mask = vmsne_vv_i32m8_b4 (v_x, v_y, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b4(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }

		i += trueVL;
	}

	FENCE();
    return(isEqual);
}

bool isEqualM_internal_vec_id(int* restrict x, double* restrict y, int n)
{
    bool isEqual = true;

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_diff = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		v_diff = vfsub_vv_f64m8(v_diff, v_y, trueVL);

		// Absolute value the difference
		v_diff = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff, EPSILON, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b8(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }
		i += trueVL;
	}

	FENCE();
	return(isEqual);
}

bool isEqualM_internal_vec_ic(int* restrict x, double complex* restrict y, int n)
{
    bool isEqual = true;

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_diff_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_diff_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		v_diff_r = vfsub_vv_f64m8(v_diff_r, v_y_r, trueVL);

		// Absolute value the difference
		v_diff_r = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		v_diff_i = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask_r = vmfgt_vf_f64m8_b8 (v_diff_r, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfgt_vf_f64m8_b8 (v_diff_i, EPSILON, trueVL);
		vbool8_t v_mask = vmor_mm_b8 (v_mask_r, v_mask_i, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b8(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }

		i += trueVL;
	}

	FENCE();
	return(isEqual);
}

bool isEqualM_internal_vec_di(double* restrict x, int* restrict y, int n)
{
	return(isEqualM_internal_vec_id(y, x, n));
}
bool isEqualM_internal_vec_dd(double* restrict x, double* restrict y, int n)
{
    bool isEqual = true;

 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		vfloat64m8_t v_diff = vfsub_vv_f64m8(v_x, v_y, trueVL);

		// Absolute value the difference
		v_diff = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff, EPSILON, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b8(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }

		i += trueVL;
	}

	FENCE();
	return(isEqual);
}

bool isEqualM_internal_vec_dc(double* restrict x, double complex* restrict y, int n)
{
    bool isEqual = true;

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

        // Load the doubles from the input
		vfloat64m8_t v_diff_r = vle64_v_f64m8(&x[i], trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Difference between x & y
		v_diff_r = vfsub_vv_f64m8(v_diff_r, v_y_r, trueVL);

		// Absolute value the difference
		v_diff_r = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_diff_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);
        
		// Absolute value the difference
		v_diff_i = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

        // abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff_r, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfgt_vf_f64m8_b8 (v_diff_i, EPSILON, trueVL);
        v_mask = vmor_mm_b8 (v_mask, v_mask_i, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b8(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }

		i += trueVL;
	}

	FENCE();
	return(isEqual);
}

bool isEqualM_internal_vec_ci(double complex* restrict x, int* restrict y, int n)
{
	return(isEqualM_internal_vec_ic(y, x, n));
}

bool isEqualM_internal_vec_cd(double complex* restrict x, double* restrict y, int n)
{
	return(isEqualM_internal_vec_dc(y, x, n));
}

bool isEqualM_internal_vec_cc(double complex* restrict x, double complex* restrict y, int n)
{
    bool isEqual = true;

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_diff_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_diff_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		v_diff_r = vfsub_vv_f64m8(v_diff_r, v_y_r, trueVL);
		v_diff_i = vfsub_vv_f64m8(v_diff_i, v_y_i, trueVL);

		// Absolute value the difference
		v_diff_r = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		v_diff_i = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff_r, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfgt_vf_f64m8_b8 (v_diff_i, EPSILON, trueVL);
		v_mask = vmor_mm_b8 (v_mask, v_mask_i, trueVL);

        // Count number of unequal values
        unsigned long inequalities = vpopc_m_b8(v_mask, trueVL);
        if (inequalities > 0)
        {
            isEqual = false;
            break;
        }

		i += trueVL;
	}

	FENCE();
	return(isEqual);
}

bool isEqualM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	if (m == NULL || n == NULL)
	{
		return(false);
	}

	if (m->ndim != n->ndim)
	{
		return(false);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
                    return(isEqualM_internal_vec_ii(((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size));
					break;

				case DOUBLE:
                    return(isEqualM_internal_vec_id(((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size));
					break;

				case COMPLEX:
                    return(isEqualM_internal_vec_ic(((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size));
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
                    return(isEqualM_internal_vec_di(((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size));
					break;

				case DOUBLE:
                    return(isEqualM_internal_vec_dd(((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size));
					break;

				case COMPLEX:
                    return(isEqualM_internal_vec_dc(((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size));
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
                    return(isEqualM_internal_vec_ci(((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size));
					break;

				case DOUBLE:
                    return(isEqualM_internal_vec_cd(((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size));
					break;

				case COMPLEX:
                    return(isEqualM_internal_vec_cc(((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size));
					break;
			}
			break;
	}
	return(true);
}

bool isEqualM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	if (m == NULL || n == NULL)
	{
		return(false);
	}

	if (m->ndim != n->ndim)
	{
		return(false);
	}

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ii(((int *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_id(((int *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ic(((int *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_di(((double *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_dd(((double *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_dc(((double *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ci(((double complex *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_cd(((double complex *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_cc(((double complex *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							return(false);
						}
					}
					break;
			}
			break;
	}
	return(true);
}

bool isEqualM_vec(Matrix *m, Matrix *n)
{
	if (m == n)
	{
		return(true);
	}

	return(isEqualM_internal_vec(m, n));
}

bool isEqualM_ref(Matrix *m, Matrix *n)
{
	if (m == n)
	{
		return(true);
	}

	return(isEqualM_internal_ref(m, n));
}

bool isEqualM(Matrix *m, Matrix *n)
// Returns true if two matrices are exactly equal in data & dimensions
{
    if (MODE == REF){return(isEqualM_ref(m, n));}
    else if (MODE == VEC){return(isEqualM_vec(m, n));}

    // Default case
	return(isEqualM_ref(m, n));
}

Matrix * areEqualM(int num, ...)
// Element-wise equality (==) of a list of matrices. Returns a matrix of 1's and 0's.
// num is the number of matrices that are being compared
{
	if (num < 2)
	{
		fprintf(stderr, "%s: %s (%d): Invalid num\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	va_list matrices;

	va_start(matrices, num);

	Matrix *first_matrix = va_arg(matrices, Matrix*);

	if (first_matrix == NULL)
	{
		va_end(matrices);
		return(NULL);
	}

	Matrix *second_matrix = va_arg(matrices, Matrix*);

	if (second_matrix == NULL)
	{
		va_end(matrices);
		return(NULL);
	}

	if (num == 2)
	{
		va_end(matrices);
		return(equalM(first_matrix, second_matrix));
	}

	Matrix *prev_mask = equalM(first_matrix, second_matrix);

	for (register int i = 0 ; i < num-2 ; i++)
	{
		Matrix *next_matrix = va_arg(matrices, Matrix*);

		if (next_matrix == NULL)
		{
			va_end(matrices);
			return(NULL);
		}

		Matrix *next_mask = equalM(first_matrix, next_matrix);

		if (next_mask == NULL)
		{
			va_end(matrices);
			fprintf(stderr, "%s: %s (%d): Matrices could not be compared\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}

		Matrix *updated_running_mask = andM(prev_mask, next_mask);

		if(!destroyM(next_mask))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}

		if(!destroyM(prev_mask))
		{
			fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
			return(NULL);
		}

		prev_mask = updated_running_mask;
	}
	Matrix *output = prev_mask;

	va_end(matrices);
	return(output);
}




void neM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

        // Initialize vectors of zeros
        vint32m8_t v_res  = vmv_v_x_i32m8 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); 
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL);

		// ne
        vbool4_t v_mask = vmsne_vv_i32m8_b4 (v_x, v_y, trueVL);

        v_res = vmerge_vxm_i32m8 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m8(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void neM_internal_vec_id(int* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		vfloat64m8_t v_diff = vfsub_vv_f64m8(v_x, v_y, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_abs = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff_abs, EPSILON, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void neM_internal_vec_ic(int* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the ints from the input & cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		vfloat64m8_t v_diff_r = vfsub_vv_f64m8(v_x_r, v_y_r, trueVL);
		vfloat64m8_t v_diff_i = vfsub_vv_f64m8(v_x_i, v_y_i, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_r_abs = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		vfloat64m8_t v_diff_i_abs = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask_r = vmfgt_vf_f64m8_b8 (v_diff_r_abs, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfgt_vf_f64m8_b8 (v_diff_i_abs, EPSILON, trueVL);
		vbool8_t v_mask = vmor_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void neM_internal_vec_di(int* restrict z, double* restrict x, int* restrict y, int n)
{
	neM_internal_vec_id(z, y, x, n);
}
void neM_internal_vec_dd(int* restrict z, double* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); 
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); 

		// Difference between x & y
		vfloat64m8_t v_diff = vfsub_vv_f64m8(v_x, v_y, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_abs = vfsgnjx_vv_f64m8(v_diff, v_diff, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask = vmfgt_vf_f64m8_b8 (v_diff_abs, EPSILON, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void neM_internal_vec_dc(int* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Load the doubles from the input
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries


        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);
        
        // almostEqual == difference is less than EPSILON set in matrix.h
        vbool8_t v_mask_r = vmfne_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);
        vbool8_t v_mask_i = vmfne_vv_f64m8_b8 (v_x_i, v_y_i, trueVL);
		vbool8_t v_mask = vmor_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

void neM_internal_vec_ci(int* restrict z, double complex* restrict x, int* restrict y, int n)
{
	neM_internal_vec_ic(z, y, x, n);
}

void neM_internal_vec_cd(int* restrict z, double complex* restrict x, double* restrict y, int n)
{
	neM_internal_vec_dc(z, y, x, n);
}

void neM_internal_vec_cc(int* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)

    // Actually needs LMUL = 4 to load doubles but store ints
 	long trueVL = vsetvl_e32m4(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m4(n - i);

        // Initialize vectors of zeros
        vint32m4_t v_res  = vmv_v_x_i32m4 (0, trueVL);

        // Stride-load the real parts as doubles from the input
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL);
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL);

        // Stride-load the imaginary parts as doubles from the input
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

		// Difference between x & y
		vfloat64m8_t v_diff_r = vfsub_vv_f64m8(v_x_r, v_y_r, trueVL);
		vfloat64m8_t v_diff_i = vfsub_vv_f64m8(v_x_i, v_y_i, trueVL);

		// Absolute value the difference
		vfloat64m8_t v_diff_r_abs = vfsgnjx_vv_f64m8(v_diff_r, v_diff_r, trueVL); // absolute value by sign-injection
		vfloat64m8_t v_diff_i_abs = vfsgnjx_vv_f64m8(v_diff_i, v_diff_i, trueVL); // absolute value by sign-injection

		// abs(diff) > EPSILON
        vbool8_t v_mask_r = vmfgt_vf_f64m8_b8 (v_diff_r_abs, EPSILON, trueVL);
        vbool8_t v_mask_i = vmfgt_vf_f64m8_b8 (v_diff_i_abs, EPSILON, trueVL);
		vbool8_t v_mask = vmor_mm_b8 (v_mask_r, v_mask_i, trueVL);
        v_res = vmerge_vxm_i32m4 (v_mask, v_res, 1, trueVL);

        // Store
		vse32_v_i32m4(&z[i], v_res, trueVL);

		i += trueVL;
	}

	FENCE();
}

Matrix * neM_internal_vec(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					neM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					neM_internal_vec_id(((int *)(output_data_data)), ((int *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX: ;
                    neM_internal_vec_ic(((int *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					neM_internal_vec_di(((int *)(output_data_data)), ((double *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					neM_internal_vec_dd(((int *)(output_data_data)), ((double *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    neM_internal_vec_dc(((int *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					neM_internal_vec_ci(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					neM_internal_vec_cd(((int *)(output_data_data)), ((double complex *)(m_data_data)),  ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
                    neM_internal_vec_cc(((int *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
					break;
			}
			break;
	}
	return(output);
}

Matrix * neM_internal_ref(Matrix* restrict m, Matrix* restrict n)
{
	Matrix *output = zerosM(m->ndim, m->dim);

	int m_data_size = m->data->size;
	void *m_data_data = m->data->data;
	void *n_data_data = n->data->data;
	void *output_data_data = output->data->data;
			
	switch(m->data->datatype)
	{
		case INTEGER:

			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ii(((int *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_id(((int *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ic(((int *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case DOUBLE:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_di(((double *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_dd(((double *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_dc(((double *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;

		case COMPLEX:
			switch(n->data->datatype)
			{
				case INTEGER:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_ci(((double complex *)(m_data_data))[i] , ((int *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case DOUBLE:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_cd(((double complex *)(m_data_data))[i] , ((double *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;

				case COMPLEX:
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						if ( !almostEqual_cc(((double complex *)(m_data_data))[i] , ((double complex *)(n_data_data))[i]) )
						{
							((int *)(output_data_data))[i] = 1;
						}
					}
					break;
			}
			break;
	}
	return(output);
}

Matrix * neM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(neM_internal_vec(m, n));
}
Matrix * neM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = zerosM(m->ndim, m->dim);
		return(output);
	}

	return(neM_internal_ref(m, n));
}
Matrix * neM(Matrix *m, Matrix *n)
// Returns a matrix of 0's and 1's that indicate whether m's elements are not equal 
// m != n
{
    if (MODE == REF){return(neM_ref(m, n));}
    else if (MODE == VEC){return(neM_vec(m, n));}

    // Default case
	return(neM_ref(m, n));
}







void pairwise_maxM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise max of integer vector x and integer vector y stored into integer vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); // Load the ints from the input
		vint32m8_t v_res = vmax_vv_i32m8(v_x, v_y, trueVL);
		vse32_v_i32m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_maxM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmax_vv_f64m8(v_x, v_y, trueVL); // max
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_maxM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmax_vv_f64m8(v_x, v_y, trueVL); // max
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_maxM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise max of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmax_vv_f64m8(v_x, v_y, trueVL); // max
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_maxM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_maxM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Load the doubles
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_maxM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_y_int = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_maxM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Load the doubles
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_maxM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x & y
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from x & y
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Only compare real parts
        vbool8_t v_mask = vmfge_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void octave_pairwise_maxM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get x_cabs and x_carg
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(y_carg);
	FENCE();
}

void octave_pairwise_maxM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Load the doubles
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get x_cabs and x_carg
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(y_carg);
	FENCE();
}

void octave_pairwise_maxM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_y_int = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get y_cabs and y_carg
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
	FENCE();
}

void octave_pairwise_maxM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

        // Load the doubles
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get y_cabs and y_carg
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
	FENCE();
}

void octave_pairwise_maxM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x & y
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from x & y
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get x_carg & y_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);        
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);        

        // Primary condition for comparison : x_cabs > y_cabs
        vbool8_t v_mask_primary = vmfgt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg >= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfge_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
    free(y_carg);
	FENCE();
}

Matrix * pairwise_maxM_internal_vec(Matrix* restrict m, Matrix* restrict n)
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
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					pairwise_maxM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					pairwise_maxM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_maxM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_maxM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
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
					pairwise_maxM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					pairwise_maxM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_maxM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_maxM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
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
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_maxM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_maxM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);                        
                    }
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_maxM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_maxM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);                        
                    }
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_maxM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_maxM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
                    }
					break;
			}
			break;
	}

	return(output);
}

Matrix * pairwise_maxM_internal_ref_fast(Matrix* restrict m, Matrix* restrict n)
// Not yet working
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
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((int *)(output_data_data))[i] = (int) fmax((double) (((int *)(m_data_data))[i]) , (double) (((int *)(n_data_data))[i])  );
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = (double) fmax((double) (((int *)(m_data_data))[i]) , (double) (((double *)(n_data_data))[i])  );
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if(creal((double complex)(((int *)(m_data_data))[i])) >= creal(((double complex *)(n_data_data))[i]))
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((int *)(m_data_data))[i]);
                            }
                            else
                            {
                                ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                            }
                        }
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if (almostEqual_dd(cabs((double complex)(((int *)(m_data_data))[i])) , cabs(((double complex *)(n_data_data))[i])))
                            {
                                if (carg((double complex)(((int *)(m_data_data))[i])) > carg(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((int *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                                }
                            }
                            else
                            {
                                if(cabs((double complex)(((int *)(m_data_data))[i])) >= cabs(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((int *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                                }
                            }
                        }
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
						((double *)(output_data_data))[i] = (double) fmax((double) (((double *)(m_data_data))[i]) , (double) (((int *)(n_data_data))[i])  );
					}
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					for (register int i = 0 ; i < m_data_size ; i++)
					{
						((double *)(output_data_data))[i] = (double) fmax((double) (((double *)(m_data_data))[i]) , (double) (((double *)(n_data_data))[i])  );
					}
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if(creal((double complex)(((double *)(m_data_data))[i])) >= creal(((double complex *)(n_data_data))[i]))
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double *)(m_data_data))[i]);
                            }
                            else
                            {
                                ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                            }
                        }
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if (almostEqual_dd(cabs((double complex)(((double *)(m_data_data))[i])) , cabs(((double complex *)(n_data_data))[i])))
                            {
                                if (carg((double complex)(((double *)(m_data_data))[i])) > carg(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                                }
                            }
                            else
                            {
                                if(cabs((double complex)(((double *)(m_data_data))[i])) >= cabs(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = ((double complex *)(n_data_data))[i];
                                }
                            }
                        }
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
                    if (COMPARISON_MODE == MATLAB)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if(creal((double complex)(((double complex *)(m_data_data))[i])) >= creal((double complex)(((int  *)(n_data_data))[i])))
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                            }
                            else
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((int  *)(n_data_data))[i]);
                            }
                        }
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if (almostEqual_dd(cabs((double complex)(((double complex *)(m_data_data))[i])) , cabs( (double complex)(((int  *)(n_data_data))[i]) )))
                            {
                                if (carg((double complex)(((double complex *)(m_data_data))[i])) > carg((double complex)(((int *)(n_data_data))[i])))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((int  *)(n_data_data))[i]);
                                }
                            }
                            else
                            {
                                if(cabs((double complex)(((double complex *)(m_data_data))[i])) >= cabs((double complex)(((int  *)(n_data_data))[i])))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double  *)(n_data_data))[i]);
                                }
                            }
                        }
                    }
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if(creal((double complex)(((double complex *)(m_data_data))[i])) >= creal((double complex)(((double  *)(n_data_data))[i])))
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                            }
                            else
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double  *)(n_data_data))[i]);
                            }
                        }
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if (almostEqual_dd(cabs((double complex)(((double complex *)(m_data_data))[i])) , cabs( (double complex)(((double  *)(n_data_data))[i]) )))
                            {
                                if (carg((double complex)(((double complex *)(m_data_data))[i])) > carg((double complex)(((double *)(n_data_data))[i])))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double  *)(n_data_data))[i]);
                                }
                            }
                            else
                            {
                                if(cabs((double complex)(((double complex *)(m_data_data))[i])) >= cabs((double complex)(((double *)(n_data_data))[i])))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double  *)(n_data_data))[i]);
                                }
                            }
                        }
                    }
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if(creal((double complex)(((double complex *)(m_data_data))[i])) >= creal((double complex)(((double complex *)(n_data_data))[i])))
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                            }
                            else
                            {
                                ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(n_data_data))[i]);
                            }
                        }
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
                        for (register int i = 0 ; i < m_data_size ; i++)
                        {
                            if (almostEqual_dd(cabs((double complex)(((double complex *)(m_data_data))[i])) , cabs(((double complex *)(n_data_data))[i])))
                            {
                                if (carg((double complex)(((double complex *)(m_data_data))[i])) > carg(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(n_data_data))[i]);
                                }
                            }
                            else
                            {
                                if(cabs((double complex)(((double complex *)(m_data_data))[i])) >= cabs(((double complex *)(n_data_data))[i]))
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(m_data_data))[i]);
                                }
                                else
                                {
                                    ((double complex *)(output_data_data))[i] = (double complex)(((double complex *)(n_data_data))[i]);
                                }
                            }
                        }
                    }
					break;
			}
			break;
	}

	return(output);
}

Matrix * pairwise_maxM_internal_ref(Matrix* restrict m, Matrix* restrict n)
// Slow!!! (but working)
{
	Matrix *m_mask = geM(m, n);
	Matrix *n_mask = notM(m_mask);

	Matrix *m_larger = timesM(m_mask, m);
	Matrix *n_larger = timesM(n_mask, n);

	Matrix *output = plusM(m_larger, n_larger);

	if (!destroyM(m_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(m_larger))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_larger))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	return(output);
}

Matrix * pairwise_maxM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = full_duplicateM_vec(m);
		return(output);
	}

	return(pairwise_maxM_internal_vec(m, n));
}

Matrix * pairwise_maxM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = full_duplicateM_ref(m);
		return(output);
	}

	return(pairwise_maxM_internal_ref(m, n));
}

Matrix * pairwise_maxM(Matrix *m, Matrix *n)
// Returns a matrix of the pairwise maximums of m and n
{
    if (MODE == REF){return(pairwise_maxM_ref(m, n));}
    else if (MODE == VEC){return(pairwise_maxM_vec(m, n));}

    // Default case
	return(pairwise_maxM_ref(m, n));
}








void pairwise_minM_internal_vec_ii(int* restrict z, int* restrict x, int* restrict y, int n)
// Elementwise min of integer vector x and integer vector y stored into integer vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are ints (SEW e32)
 	long trueVL = vsetvl_e32m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e32m8(n - i);

		vint32m8_t v_x = vle32_v_i32m8(&x[i], trueVL); // Load the ints from the input
		vint32m8_t v_y = vle32_v_i32m8(&y[i], trueVL); // Load the ints from the input
		vint32m8_t v_res = vmin_vv_i32m8(v_x, v_y, trueVL);
		vse32_v_i32m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_minM_internal_vec_id(double* restrict z, int* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_x_i = vle32_v_i32m4(&x[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_x = vfwcvt_f_x_v_f64m8(v_x_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmin_vv_f64m8(v_x, v_y, trueVL); // min
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_minM_internal_vec_di(double* restrict z, double* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vint32m4_t v_y_i = vle32_v_i32m4(&y[i], trueVL); // Load the ints from the input
		vfloat64m8_t v_y = vfwcvt_f_x_v_f64m8(v_y_i, trueVL); // Cast the ints to doubles

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmin_vv_f64m8(v_x, v_y, trueVL); // min
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void pairwise_minM_internal_vec_dd(double* restrict z, double* restrict x, double* restrict y, int n)
// Elementwise min of double vector x and double vector y stored into double vector z
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_y = vle64_v_f64m8(&y[i], trueVL); // Load the doubles from the input
		vfloat64m8_t v_res = vfmin_vv_f64m8(v_x, v_y, trueVL); // min
		vse64_v_f64m8(&z[i], v_res, trueVL); // Store

		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_minM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_minM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Load the doubles
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_minM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_y_int = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_minM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Load the doubles
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Only compare real parts
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void matlab_pairwise_minM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x & y
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from x & y
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Only compare real parts
        vbool8_t v_mask = vmfle_vv_f64m8_b8 (v_x_r, v_y_r, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

	FENCE();
}

void octave_pairwise_minM_internal_vec_ic(double complex* restrict z, int* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_x_int = vle32_v_i32m4(&x[i], trueVL);
		vfloat64m8_t v_x_r = vfwcvt_f_x_v_f64m8(v_x_int, trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get x_cabs and x_carg
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg <= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(y_carg);
	FENCE();
}

void octave_pairwise_minM_internal_vec_dc(double complex* restrict z, double* restrict x, double complex* restrict y, int n)
{
    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from y
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from y
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get y_carg
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);

        // Load the doubles
		vfloat64m8_t v_x_r = vle64_v_f64m8(&x[i], trueVL);
        vfloat64m8_t v_x_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get x_cabs and x_carg
		vfloat64m8_t v_x_cabs = vfsgnjx_vv_f64m8(v_x_r, v_x_r, trueVL);
        vbool8_t v_x_r_neg_mask = vmflt_vf_f64m8_b8 (v_x_r, 0.0, trueVL);
		vfloat64m8_t v_x_carg = vfmerge_vfm_f64m8(v_x_r_neg_mask, v_x_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg <= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(y_carg);
	FENCE();
}

void octave_pairwise_minM_internal_vec_ci(double complex* restrict z, double complex* restrict x, int* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

        // Load the ints and cast to doubles
		vint32m4_t v_y_int = vle32_v_i32m4(&y[i], trueVL);
		vfloat64m8_t v_y_r = vfwcvt_f_x_v_f64m8(v_y_int, trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get y_cabs and y_carg
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg <= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
	FENCE();
}

void octave_pairwise_minM_internal_vec_cd(double complex* restrict z, double complex* restrict x, double* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 

        // Load imaginary parts from x
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get x_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);

        // Load the doubles
		vfloat64m8_t v_y_r = vle64_v_f64m8(&y[i], trueVL);
        vfloat64m8_t v_y_i  = vfmv_v_f_f64m8 (0, trueVL); // Imaginary imaginaries

        // Get y_cabs and y_carg
		vfloat64m8_t v_y_cabs = vfsgnjx_vv_f64m8(v_y_r, v_y_r, trueVL);
        vbool8_t v_y_r_neg_mask = vmflt_vf_f64m8_b8 (v_y_r, 0.0, trueVL);
		vfloat64m8_t v_y_carg = vfmerge_vfm_f64m8(v_y_r_neg_mask, v_y_i, M_PI, trueVL);

        // Primary condition for comparison : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg <= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
	FENCE();
}

void octave_pairwise_minM_internal_vec_cc(double complex* restrict z, double complex* restrict x, double complex* restrict y, int n)
{
    double *x_carg = NULL;
    x_carg = malloc(n*sizeof(*x_carg));
    argM_vec_c(x_carg, x, n);

    double *y_carg = NULL;
    y_carg = malloc(n*sizeof(*y_carg));
    argM_vec_c(y_carg, y, n);

	// 4 input args; LMUL = 32/4 = 8 (m8); outputs are doubles (SEW e64)
 	long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

	int i;
 	for (i = 0; i < n;) {
		trueVL = vsetvl_e64m8(n - i);

		// Load the real parts from x & y
		vfloat64m8_t v_x_r = vlse64_v_f64m8(&(((double *)(x))[2*i]), 16, trueVL); 
		vfloat64m8_t v_y_r = vlse64_v_f64m8(&(((double *)(y))[2*i]), 16, trueVL); 

        // Load imaginary parts from x & y
		vfloat64m8_t v_x_i = vlse64_v_f64m8(&(((double *)(x))[2*i+1]), 16, trueVL);
		vfloat64m8_t v_y_i = vlse64_v_f64m8(&(((double *)(y))[2*i+1]), 16, trueVL);

        // Get x_cabs
        vfloat64m8_t v_x_r2 = vfmul_vv_f64m8(v_x_r, v_x_r, trueVL); // Square the real parts
		vfloat64m8_t v_x2 = vfmacc_vv_f64m8(v_x_r2, v_x_i, v_x_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_x_cabs = vfsqrt_v_f64m8(v_x2, trueVL); // Sqrt

        // Get y_cabs
        vfloat64m8_t v_y_r2 = vfmul_vv_f64m8(v_y_r, v_y_r, trueVL); // Square the real parts
		vfloat64m8_t v_y2 = vfmacc_vv_f64m8(v_y_r2, v_y_i, v_y_i, trueVL); // Square the imaginary parts and add them to the squared reals
		vfloat64m8_t v_y_cabs = vfsqrt_v_f64m8(v_y2, trueVL); // Sqrt

        // Get x_carg & y_carg
        vfloat64m8_t v_x_carg = vle64_v_f64m8(&x_carg[i], trueVL);        
        vfloat64m8_t v_y_carg = vle64_v_f64m8(&y_carg[i], trueVL);        

        // Primary condition for comparison : x_cabs < y_cabs
        vbool8_t v_mask_primary = vmflt_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);

        // Secondary condition for comparison : if x_cabs == y_cabs, then x_carg <= y_carg
        vbool8_t v_mask_cabs_eq = vmfeq_vv_f64m8_b8 (v_x_cabs, v_y_cabs, trueVL);
        vbool8_t v_mask_carg_ge = vmfle_vv_f64m8_b8 (v_x_carg, v_y_carg, trueVL);
        vbool8_t v_mask_secondary = vmand_mm_b8 (v_mask_cabs_eq, v_mask_carg_ge, trueVL);

        // Combine all these masks...
        vbool8_t v_mask = vmor_mm_b8 (v_mask_primary, v_mask_secondary, trueVL);

        vfloat64m8_t v_res_r = vmerge_vvm_f64m8 (v_mask, v_y_r, v_x_r, trueVL);
        vfloat64m8_t v_res_i = vmerge_vvm_f64m8 (v_mask, v_y_i, v_x_i, trueVL);

		// Storein z, stride by 16 for 2 x 8B
		// Must double i because previous iterations stored on double the indices
		vsse64_v_f64m8(&(((double *)(z))[2*i]), 16, v_res_r, trueVL);
		vsse64_v_f64m8(&(((double *)(z))[2*i+1]), 16, v_res_i, trueVL);
		i += trueVL;
	}

    free(x_carg);
    free(y_carg);
	FENCE();
}

Matrix * pairwise_minM_internal_vec(Matrix* restrict m, Matrix* restrict n)
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
					output = createM(m->ndim, m->dim, INTEGER);	
					output_data_data = output->data->data;
					pairwise_minM_internal_vec_ii(((int *)(output_data_data)), ((int *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					pairwise_minM_internal_vec_id(((double *)(output_data_data)), ((int *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_minM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_minM_internal_vec_ic(((double complex *)(output_data_data)), ((int *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
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
					pairwise_minM_internal_vec_di(((double *)(output_data_data)), ((double *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, DOUBLE);
					output_data_data = output->data->data;
					pairwise_minM_internal_vec_dd(((double *)(output_data_data)), ((double *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_minM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_minM_internal_vec_dc(((double complex *)(output_data_data)), ((double *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
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
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_minM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_minM_internal_vec_ci(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((int *)(n_data_data)), m_data_size);                        
                    }
					break;

				case DOUBLE:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_minM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_minM_internal_vec_cd(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double *)(n_data_data)), m_data_size);                        
                    }
					break;

				case COMPLEX:
					output = createM(m->ndim, m->dim, COMPLEX);
					output_data_data = output->data->data;
                    if (COMPARISON_MODE == MATLAB)
                    {
		    			matlab_pairwise_minM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);
                    }
                    else if (COMPARISON_MODE == OCTAVE)
                    {
		    			octave_pairwise_minM_internal_vec_cc(((double complex *)(output_data_data)), ((double complex *)(m_data_data)), ((double complex *)(n_data_data)), m_data_size);                        
                    }
					break;
			}
			break;
	}

	return(output);
}

Matrix * pairwise_minM_internal_vec_slow(Matrix* restrict m, Matrix* restrict n)
// Don't use this
{
	Matrix *m_mask = leM_vec(m, n);
	Matrix *n_mask = notM_vec(m_mask);

	Matrix *m_smaller = timesM_vec(m_mask, m);
	Matrix *n_smaller = timesM_vec(n_mask, n);

	Matrix *output = plusM_vec(m_smaller, n_smaller);

	if (!destroyM(m_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(m_smaller))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_smaller))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	return(output);
}

Matrix * pairwise_minM_internal_ref(Matrix* restrict m, Matrix* restrict n)
// SLOW!! (but working)
{
	Matrix *m_mask = leM(m, n);
	Matrix *n_mask = notM(m_mask);

	Matrix *m_smaller = timesM(m_mask, m);
	Matrix *n_smaller = timesM(n_mask, n);

	Matrix *output = plusM(m_smaller, n_smaller);

	if (!destroyM(m_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_mask))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(m_smaller))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	if (!destroyM(n_smaller))
	{
		fprintf(stderr, "%s: %s (%d): Failed to destroy matrix\n", __FILE__, __func__, __LINE__);
		return(NULL);
	}

	return(output);
}

Matrix * pairwise_minM_vec(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = full_duplicateM_vec(m);
		return(output);
	}

	return(pairwise_minM_internal_vec(m, n));
}

Matrix * pairwise_minM_ref(Matrix *m, Matrix *n)
{
	if (!can_matrices_be_compared(m,n)) {return(NULL);}

	if (m == n)
	{
		Matrix *output = full_duplicateM_ref(m);
		return(output);
	}

	return(pairwise_minM_internal_ref(m, n));
}

Matrix * pairwise_minM(Matrix *m, Matrix *n)
// Returns a matrix of the pairwise minimums of m and n
{
    if (MODE == REF){return(pairwise_minM_ref(m, n));}
    else if (MODE == VEC){return(pairwise_minM_vec(m, n));}

    // Default case
	return(pairwise_minM_ref(m, n));
}

