//
//  matrix.h
//  Octave-C
//
//  Created by Zach Taylor on 2/26/21.
//

#ifndef matrix_h
#define matrix_h

#define _XOPEN_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "tgmath.h"
#include "../riscv-blas/INCLUDE/cblas.h"
#include "../riscv-blas/INCLUDE/lapacke.h"
#include "../fftw-3.3.10/api/fftw3.h"

#include "../BSC-RVV/common/vector_defines.h"
#include "../BSC-RVV/common/trace_macros.h"

#define INTEGER 0
#define DOUBLE 1
#define COMPLEX 2

#define ASCENDING 0
#define DESCENDING 1

#define OCTAVE 0
#define MATLAB 1
#define COMPARISON_MODE OCTAVE

#define REF 0
#define VEC 1
#define DEFAULT 2
#define MODE DEFAULT

#define HANNING 1
#define HAMMING 2
#define RECTANGLE 3

#define RESET_SEED 0

//#define M_PI 3.141592653589793

#define EPSILON 10000000*DBL_EPSILON

// Timing macros
#ifndef __TRACE_MACROS
#define __TRACE_MACROS

#ifndef START_TRACE
#ifdef __riscv
#define START_TRACE   asm("xor x0, x0, x0");
#else
#define START_TRACE
#endif
#endif

#ifndef STOP_TRACE
#ifdef __riscv
#define STOP_TRACE   asm("xor x0, x1, x1");
#else
#define STOP_TRACE
#endif
#endif

#endif







/*
* =========================================
* 
*  Definition of the matrix
* 
* =========================================
*/
struct vector {
	void *data;   // The actual data
	int datatype; // The type of data in the vector (INTEGER, DOUBLE, or COMPLEX)
	int size;     // The number of elements in the vector
	int readOnly; // The number of different matrices using this data. >1 indicates readOnly. 0 indicates read/write.
};
typedef struct vector Vector;

struct matrix {
	Vector *data; // Points to a vector containing data
	int ndim;     // The dimensionality of a Matrix, anywhere from 1-4
	int dim[4];   // The actual dimensions of the Matrix
};
typedef struct matrix Matrix;


/*
* =========================================
* 
*  Functions to create & destroy matrices
*  Source code in matrix.c
* 
* =========================================
*/

#define allocM createM
#define mallocM createM
Matrix * createM(int ndim, int dim[ndim], int type);
// Creates a matrix
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// type = the datatype (INTEGER, DOUBLE, COMPLEX)
// Returns: a Matrix* object

Matrix * duplicateM(Matrix *m);
// Duplicates a matrix (NOT including underlying data)
// m = the matrix to duplicate
// Notes: increments the number of users (readOnly) of the underlying data, making it readOnly
// Returns: a Matrix* object that views the same data as m

Matrix * full_duplicateM_ref(Matrix *m);
Matrix * full_duplicateM_vec(Matrix *m);
Matrix * full_duplicateM(Matrix *m);
// Duplicates a matrix (including the underlying data)
// m = the matrix to duplicate
// Returns: a Matrix* object that views the same data as m

#define idM identityM
#define eyeM identityM
Matrix * identityM(int size);
// Creates a 2D square identity matrix
// size = the side length of the square matrix
// Returns: a Matrix* object

#define callocM zerosM
#define zeroesM zerosM
#define zeroM zerosM
Matrix * zerosM(int ndim, int dim[ndim]);
// Creates an int matrix of zeros
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// Returns: a Matrix* object

Matrix * onesM_vec(int ndim, int dim[ndim]);
Matrix * onesM_ref(int ndim, int dim[ndim]);
Matrix * onesM(int ndim, int dim[ndim]);
// Creates an int matrix of ones
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// Returns: a Matrix* object

#define newSeed randomSeed
#define randSeed randomSeed
void randomSeed();
// Randomizes the seed

void resetSeed();
// Resets the seed to the default

Matrix * randM(int ndim, int dim[ndim]);
// Creates a double matrix of random elements uniformly distributed on the interval (0, 1)
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// Returns: a Matrix* object

Matrix * randiM(int ndim, int dim[ndim], int type, int imax);
// Creates a matrix of random elements uniformly distributed on the interval [0, max(imax, INT_MAX))
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// type = the datatype (INTEGER, DOUBLE, COMPLEX)
// Returns: a Matrix* object


Matrix * randnM(int ndim, int dim[ndim]);
// Creates a double matrix of random elements normally distributed on the interval (-1, 1) with zero mean and one variance
// Uses Box-Muller to transform from uniform to normal ditribution
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// Returns: a Matrix* object

Matrix * reindexM(Matrix *m, int size, ...);
// Creates a matrix out of pieces of another matrix
// m = the matrix to extract values from
// size = the size of the output matrix
// ... = the flat-indices (one-based) of the elements in m to include in the output
// Returns a Matrix* object, that is a column vector of length 'size'

#define freeM destroyM
bool destroyM(Matrix *m);
// Destroys a matrix
// m = the matrix to destroy
// Notes: if the matrix is readOnly, the underlying data is not destroyed, and the readOnly counter is decremented
// Returns: true if successful; false otherwise










/*
* =========================================
* 
*  Functions to read & write to matrices
*  Source code in matrix.c
* 
* =========================================
*/

bool writeM_ref(Matrix *m, int size, void *input);
bool writeM_vec(Matrix *m, int size, void *input);
bool writeM(Matrix *m, int size, void *input);
// Assigns data to a matrix
// m = the matrix to which to assign data
// size = the length of the input. This MUST be the same as the value at m->data->size
// input = the data to be assigned
// Note: It is the user's responsibility to ensure that the type of the input data matches that of the initialized matrix
// Note: If a matrix is readOnly, data will not be written
// Note: typically, the input data will have been malloc'ed before calling this function. After calling writeM(), that input data can be immediately free'd
// Returns: true (if successful); false (on mismatch size or if the matrix is readOnly)

void indexM(Matrix *m, void *output, int ndim, ...);
// Gets a data point from a matrix
// m = the matrix to be read
// output = a pointer to an output of a predetermined type (int, double, double complex)
// Note: indexM is *** One-indexed *** , to match Octave
// Takes a variable number of inputs for different dimensionalities.
// Example: 
// 		double foo;
// 		indexM(m, &foo, m->ndim, row, column[, index3[, index4]])
// 		// foo is now equal to the value of the specified index.

// Inputting ndim=1 and only one dimension will be interpreted as a flat index (i.e., the 1D index)
// This is especially useful when iterating through a whole matrix
// Example: 
// 		double foo;
// 		indexM(m, &foo, 1, 1); 
// 		// foo is now equal to the value of row 1, column 1(, dimension-three 1(, dimension-four 1)) of matrix m.

void printM(Matrix *m);
void fprintM(FILE *fptr, Matrix *m);
// Prints out a matrix m
// Note: will only print out the first and second dims of a 3-D or 4-D matrix

#define getndimM getnDimM
#define getNdimM getnDimM
#define getNDimM getnDimM
int getnDimM(Matrix *m);
// Returns the dimensionality of a matrix m

#define getdimsM getDimsM
#define getdimM getDimsM
#define getDimM getDimsM
int * getDimsM(Matrix *m);
// Returns an int array of the dimensions of a matrix m
// This array will always be of size 4 (since matrices may be up to 4-D)
// For a 2-D, 3x2 matrix, the output would be {3,2,1,1}

#define getDataM getdataM
void * getdataM(Matrix *m);
// Returns a void pointer to a matrix's data
// Must be casted to be used effectively

#define getTypeM gettypeM
int gettypeM(Matrix *m);
// Returns the datatype of matrix m's data

#define getSizeM getsizeM
int getsizeM(Matrix *m);
// Returns the size of a matrix m

#define getReadonlyM getreadonlyM
#define getreadOnlyM getreadonlyM
#define getReadOnlyM getreadonlyM
int getreadonlyM(Matrix *m);
// Returns the number of users viewing a matrix m

#define resizeM reshapeM
bool reshapeM (Matrix* m, int ndim, int dim[ndim]);
// Changes the dimensions of a matrix m, retaining the existing data
// m = the matrix to transform
// ndim = the number of dimensions in the matrix (for a 2D matrix, ndim == 2)
// dim[ndim] = an int array of the matrix dimensions (for a 2x3 matrix, dim == {2,3})
// Note: reshapeM will fail if the size of the output (i.e., the product of dim[]) does not match the original matrix
// Returns: true (if successful); false otherwise










/*
* =========================================
* 
*  Internal functions
*  Source code in utils.c
* 
* =========================================
*/

bool can_matrices_be_compared_internal(Matrix* restrict m, Matrix* restrict n);
// Returns true if two matrices can be compared (i.e., they have the same dimensions)
// Undefined behavior if m == n

bool can_matrices_be_compared(Matrix *m, Matrix *n);
// Returns true if two matrices can be compared (i.e., they have the same dimensions)
// Returns true if m == n

bool almostEqual_ii(int x, int y);
bool almostEqual_id(int x, double y);
bool almostEqual_ic(int x, double complex y);
bool almostEqual_di(double x, int y);
bool almostEqual_dd(double x, double y);
bool almostEqual_dc(double x, double complex y);
bool almostEqual_ci(double complex x, int y);
bool almostEqual_cd(double complex x, double y);
bool almostEqual_cc(double complex x, double complex y);
// Functions to check near-equality if numbers (usually doubles) are not stored in a precise enough manner for actual equality

// Casting functions
// func x_to_y(m) takes a matrix m of type x and returns its data cast to type y
// i = int
// d = double
// c = double complex
int * i_to_i_vec(Matrix* restrict m);
int * i_to_i_ref(Matrix* restrict m);
int * i_to_i(Matrix* restrict m);
double * i_to_d_vec(Matrix* restrict m);
double * i_to_d_ref(Matrix* restrict m);
double * i_to_d(Matrix* restrict m);
double complex * i_to_c_vec(Matrix* restrict m);
double complex * i_to_c_ref(Matrix* restrict m);
double complex * i_to_c(Matrix* restrict m);
int * d_to_i_vec(Matrix* restrict m);
int * d_to_i_ref(Matrix* restrict m);
int * d_to_i(Matrix* restrict m);
double * d_to_d_vec(Matrix* restrict m);
double * d_to_d_ref(Matrix* restrict m);
double * d_to_d(Matrix* restrict m);
double complex * d_to_c_vec(Matrix* restrict m);
double complex * d_to_c_ref(Matrix* restrict m);
double complex * d_to_c(Matrix* restrict m);
int * c_to_i_vec(Matrix* restrict m);
int * c_to_i_ref(Matrix* restrict m);
int * c_to_i(Matrix* restrict m);
double * c_to_d_vec(Matrix* restrict m);
double * c_to_d_ref(Matrix* restrict m);
double * c_to_d(Matrix* restrict m);
double complex * c_to_c_vec(Matrix* restrict m);
double complex * c_to_c_ref(Matrix* restrict m);
double complex * c_to_c(Matrix* restrict m);

// Functions for comparison when sorting
int int_comp_asc(const void *elem1, const void *elem2);
int int_comp_desc(const void *elem1, const void *elem2);
int double_comp_asc(const void *elem1, const void *elem2);
int double_comp_desc(const void *elem1, const void *elem2);
int complex_comp_asc(const void *elem1, const void *elem2);
int complex_comp_desc(const void *elem1, const void *elem2);

bool containsOverflow(double *x, int size);
// Returns true if the input double array x of length size contains a value that cannot be represented as an integer

long long get_time();
// Gets the current time
// Unexpected behavior when running with spike
// Prefer get_cycles instead

float elapsed_time(long long start_time, long long end_time);
// Returns the number of seconds elapsed between the two specified times
// Unexpected behavior when running with spike
// Prefer elapsed_cycles instead

unsigned long get_cycles(void);
// Gets the current number of cycles

double elapsed_cycles(unsigned long start_cycles, unsigned long end_cycles);
// Difference in number of cycles





/*
* =========================================
* 
*  Functions for single-matrix math operations
*  Source code in matrix_math.c
* 
* =========================================
*/

Matrix * transposeM(Matrix *m);
// Transposes matrix m
// Returns a Matrix*, or NULL if it fails

Matrix * ctransposeM(Matrix *m);
// Complex-conjugate transposes matrix m
// Returns a Matrix*, or NULL if it fails

#define detM determinantM
void determinantM(Matrix *m, void *output);
// Computes the determinant of a matrix
// Since the determinant could be an integer, double, or double complex, you must pass a pointer to the output

#define invM invertM
#define inverseM invertM
Matrix * invertM(Matrix *m);
// Inverts a matrix m
// Returns a Matrix*, or NULL if it fails

void eigM(Matrix* restrict m, Matrix* restrict *evals, Matrix* restrict *evecs);
// Eigendecomposition of a matrix m
// m = The matrix to decompose, size n x n
// evals = A square matrix of size n x n where the eigenvalues will be placed (along the diagonal)
// evecs = A square matrix of size n x n where the (right, normalized) eigenvectors will be placed in columns
// Has the property that evecs * evals * inv(evecs) == m



/*
* =========================================
* 
*  Matrix element-wise logic operations
*  Source code in elementwise_math.c
* 
* =========================================
*/


Matrix * trueM_vec(Matrix *m);
Matrix * trueM_ref(Matrix *m);
Matrix * trueM(Matrix *m);
// Does element-wise logical TRUE of matrix m
// essentially NOT(NOT(m))
// Returns: an int Matrix* of 0's and 1's

Matrix * andM_vec(Matrix *m, Matrix *n);
Matrix * andM_ref(Matrix *m, Matrix *n);
Matrix * andM(Matrix *m, Matrix *n);
// Does element-wise logical AND of matrices m and n
// Returns: an int Matrix* of 0's and 1's

Matrix * orM_vec(Matrix *m, Matrix *n);
Matrix * orM_ref(Matrix *m, Matrix *n);
Matrix * orM(Matrix *m, Matrix *n);
// Does element-wise logical OR of matrices m and n
// Returns: an int Matrix* of 0's and 1's

Matrix * notM_vec(Matrix *m);
Matrix * notM_ref(Matrix *m);
Matrix * notM(Matrix *m);
// Does element-wise logical NOT of matrix m
// Returns: an int Matrix* of 0's and 1's










/*
* =========================================
* 
*  Matrix-Matrix element-wise comparison operations
*  Source code in elementwise_comparison.c
* 
* =========================================
*/

bool isEqualM_vec(Matrix *m, Matrix *n);
bool isEqualM_ref(Matrix *m, Matrix *n);
bool isEqualM(Matrix *m, Matrix *n);
// Returns true if two matrices are exactly equal in data & dimensions

#define eqM equalM
Matrix * equalM_vec(Matrix *m, Matrix *n);
Matrix * equalM_ref(Matrix *m, Matrix *n);
Matrix * equalM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether two matrices' corresponding elements are equal (1 = equal)
// m == n

Matrix * areEqualM(int num, ...);
// Returns a matrix of 0's and 1's that indicate whether num matrices' corresponding elements are all equal (1 = equal)
// num = the number of matrices to be compared

Matrix * matlab_gtM_ref(Matrix *m, Matrix *n);
Matrix * matlab_gtM_vec(Matrix *m, Matrix *n);
Matrix * octave_gtM_vec(Matrix *m, Matrix *n);
Matrix * octave_gtM_ref(Matrix *m, Matrix *n);
Matrix * gtM_vec(Matrix *m, Matrix *n);
Matrix * gtM_ref(Matrix *m, Matrix *n);
Matrix * gtM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater 
// m > n

Matrix * matlab_geM_ref(Matrix *m, Matrix *n);
Matrix * matlab_geM_vec(Matrix *m, Matrix *n);
Matrix * octave_geM_vec(Matrix *m, Matrix *n);
Matrix * octave_geM_ref(Matrix *m, Matrix *n);
Matrix * geM_vec(Matrix *m, Matrix *n);
Matrix * geM_ref(Matrix *m, Matrix *n);
Matrix * geM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether m's elements are greater or equal
// m >= n

Matrix * matlab_ltM_ref(Matrix *m, Matrix *n);
Matrix * matlab_ltM_vec(Matrix *m, Matrix *n);
Matrix * octave_ltM_vec(Matrix *m, Matrix *n);
Matrix * octave_ltM_ref(Matrix *m, Matrix *n);
Matrix * ltM_vec(Matrix *m, Matrix *n);
Matrix * ltM_ref(Matrix *m, Matrix *n);
Matrix * ltM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether m's elements are less 
// m < n

Matrix * matlab_leM_ref(Matrix *m, Matrix *n);
Matrix * matlab_leM_vec(Matrix *m, Matrix *n);
Matrix * octave_leM_vec(Matrix *m, Matrix *n);
Matrix * octave_leM_ref(Matrix *m, Matrix *n);
Matrix * leM_vec(Matrix *m, Matrix *n);
Matrix * leM_ref(Matrix *m, Matrix *n);
Matrix * leM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether m's elements are less or equal 
// m <= n

#define neqM neM
Matrix * neM_vec(Matrix *m, Matrix *n);
Matrix * neM_ref(Matrix *m, Matrix *n);
Matrix * neM(Matrix *m, Matrix *n);
// Returns a matrix of 0's and 1's that indicate whether m's elements are not equal 
// m != n

#define elementwise_maxM pairwise_maxM
#define elementwise_maximumM pairwise_maxM
#define pairwise_maximumM pairwise_maxM
Matrix * pairwise_maxM_vec(Matrix *m, Matrix *n);
Matrix * pairwise_maxM_ref(Matrix *m, Matrix *n);
Matrix * pairwise_maxM(Matrix *m, Matrix *n);
// Returns a matrix of the pairwise maximums of m and n

#define elementwise_minM pairwise_minM
#define elementwise_minimumM pairwise_minM
#define pairwise_minimumM pairwise_minM
Matrix * pairwise_minM_vec(Matrix *m, Matrix *n);
Matrix * pairwise_minM_ref(Matrix *m, Matrix *n);
Matrix * pairwise_minM(Matrix *m, Matrix *n);
// Returns a matrix of the pairwise minimums of m and n










/*
* =========================================
* 
*  Single-Matrix element-wise math operations
*  Source code in elementwise_math.c
* 
* =========================================
*/

void argM_vec_i(double* restrict y, int* restrict x, int n);
void argM_vec_d(double* restrict y, double* restrict x, int n);
void argM_vec_c(double* restrict y, double complex* restrict x, int n);
Matrix * argM_ref(Matrix *m);
Matrix * argM_vec(Matrix *m);
Matrix * argM(Matrix *m);
// Element-wise complex arg value of a matrix m
// Returns a Matrix*

void absM_vec_i(int* restrict y, int* restrict x, int n);
void absM_vec_d(double* restrict y, double* restrict x, int n);
void absM_vec_c(double* restrict y, double complex* restrict x, int n);
Matrix * absM_ref(Matrix *m);
Matrix * absM_vec(Matrix *m);
Matrix * absM(Matrix *m);
// Element-wise absolute value of a matrix m
// Returns a Matrix*

Matrix * floorM_vec(Matrix *m);
Matrix * floorM_ref(Matrix *m);
Matrix * floorM(Matrix *m);
// Element-wise floor of a matrix m
// Returns a Matrix*

Matrix * ceilM_vec(Matrix *m);
Matrix * ceilM_ref(Matrix *m);
Matrix * ceilM(Matrix *m);
// Element-wise ceiling of a matrix m
// Returns a Matrix*

Matrix * roundM_vec(Matrix *m);
Matrix * roundM_ref(Matrix *m);
Matrix * roundM(Matrix *m);
// Element-wise round-to-nearest-int of a matrix m
// Returns a Matrix*

#define scalartimesM scaleM
#define scalarmultiplyM scaleM
Matrix * scaleM_ref(Matrix *m, void *scalar, int type);
Matrix * scaleM_vec(Matrix *m, void *scalar, int type);
Matrix * scaleM(Matrix *m, void *scalar, int type);
// Multiplies a scalar by a matrix
// m = the input matrix
// scalar = a POINTER to a scalar (either int, double, or double complex)
// type = the type of the scalar (either INTEGER, DOUBLE, or, COMPLEX)
// Note: It is the user's responsibility to ensure that the type correctly matches the scalar
// Returns: an output Matrix*

#define spowM scalarpowerM
#define scalarpowM scalarpowerM
Matrix * scalarpowerM(Matrix *m, void *scalar, int type);
// Elementwise raise a matrix to the power n
// Returns a Matrix*

Matrix * sinM(Matrix *m);
// Element-wise sine of matrix M, where the elements of m are in radians
// Returns a Matrix*

#define sind(X) sin(X * M_PI / (double) 180.0)
Matrix * sindM(Matrix *m);
// Element-wise sine of matrix M, where the elements of m are in degrees
// Returns a Matrix*

Matrix * cosM_vec(Matrix *m);
Matrix * cosM_ref(Matrix *m);
Matrix * cosM(Matrix *m);
// Element-wise cosine of matrix M, where the elements of m are in radians
// Returns a Matrix*

#define cosd(X) cos(X * M_PI / (double) 180.0)
Matrix * cosdM(Matrix *m);
// Element-wise cosine of matrix M, where the elements of m are in degrees
// Returns a Matrix*

Matrix * tanM(Matrix *m);
// Element-wise tangent of matrix M, where the elements of m are in radians
// Returns a Matrix*

#define tand(X) tan(X * M_PI / (double) 180.0)
Matrix * tandM(Matrix *m);
// Element-wise tangent of matrix M, where the elements of m are in degrees
// Returns a Matrix*










/*
* =========================================
* 
*  Matrix-Matrix element-wise math operations
*  Source code in elementwise_math.c
* 
* =========================================
*/

#define addM plusM
Matrix * plusM_vec(Matrix *m, Matrix *n);
Matrix * plusM_ref(Matrix *m, Matrix *n);
Matrix * plusM(Matrix *m, Matrix *n);
// Elementwise addition of two matrices
// Returns a Matrix*

#define subtractM minusM
Matrix * minusM_vec(Matrix *m, Matrix *n);
Matrix * minusM_ref(Matrix *m, Matrix *n);
Matrix * minusM(Matrix *m, Matrix *n);
// Elementwise subtraction of two matrices
// Returns a Matrix*

#define multiplyM timesM
Matrix * timesM_vec(Matrix *m, Matrix *n);
Matrix * timesM_ref(Matrix *m, Matrix *n);
Matrix * timesM(Matrix *m, Matrix *n);
// Elementwise multiplication of two matrices
// Returns a Matrix*

Matrix * rdivideM_vec(Matrix *m, Matrix *n);
Matrix * rdivideM_ref(Matrix *m, Matrix *n);
Matrix * rdivideM(Matrix *m, Matrix *n);
// Elementwise right division of two matrices
// Returns a Matrix*

Matrix * ldivideM_vec(Matrix *m, Matrix *n);
Matrix * ldivideM_ref(Matrix *m, Matrix *n);
Matrix * ldivideM(Matrix *m, Matrix *n);
// Elementwise left division of two matrices
// Returns a Matrix*

#define powM powerM
Matrix * powerM(Matrix *m, Matrix *n);
// Element-wise raise m to the nth power
// Returns a Matrix*










/*
* =========================================
* 
*  Matrix mathematical operations
*  i.e., NOT element-wise
*  Source code in matrix_math.c
* 
* =========================================
*/

#define mmultiplyM mtimesM
Matrix * mtimesM(Matrix *m, Matrix *n);
// Matrix product of two matrices m and n
// m and n are the two ordered matrix inputs
// Note: Uses BLAS
// Returns: a Matrix* output (or NULL, if unsuccessful)

Matrix * naive_mtimesM_vec(Matrix *m, Matrix *n);
Matrix * naive_mtimesM_ref(Matrix *m, Matrix *n);
// Matrix product of two matrices m and n
// m and n are the two ordered matrix inputs
// Note: Does NOT use BLAS. Naive implementation.
// Returns: a Matrix* output (or NULL, if unsuccessful)

Matrix * mrdivideM(Matrix *m, Matrix *n);
// Matrix right division
// output = transpose(inv(transpose(m)) * transpose(n))
// Note: Uses BLAS 
// Returns: a Matrix* output (or NULL, if unsuccessful)

Matrix * mldivideM(Matrix *m, Matrix *n);
// Matrix left division
// output = inv(m)*n
// Note: Uses BLAS 
// Returns: a Matrix* output (or NULL, if unsuccessful)

#define mpowM mpowerM
Matrix * mpowerM(Matrix *m, void *scalar, int type);
// Matrix to a power
// output = m ^ scalar
// Note: Uses BLAS 
// Note: Behavior is unpredictable if m is defective (non diagonalizable), and the scalar is a non-integer
// Returns: a Matrix* output (or NULL, if unsuccessful)










/*
* =========================================
* 
*  Matrix statistical operations
*  Source code in stats.c
* 
* =========================================
*/

void sort(void * data, int N, int type, int direction);
// Sorts a list of numbers in place
// type = the type (INTEGER, DOUBLE, or COMPLEX) of the data
// N = the length of the list to sort
// data = the data
// direction. 0 = ascending; 1 = descending

Matrix * sortM(Matrix *m, int direction);
// If m is a row or column vector, returns a row or column vector with sorted elements
// If m is a matrix, returns a matrix with each column sorted
// direction. 0 = ascending; 1 = descending

#define average mean
#define avg mean
void mean(void * output, int type, int N, void * data);
// Returns the average of a list of numbers
// Output = the address of the output (a void pointer since it may be int, double, or complex)
// type = the type (INTEGER, DOUBLE, or COMPLEX) of the data
// N = the number of numbers to average
// ... = the list of numbers to sum

#define averageM meanM
#define avgM meanM
Matrix * meanM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the average of the values
// If m is a matrix, returns a row vector with the mean value of each column

Matrix * medianM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the median of the values
// If m is a matrix, returns a row vector with the median value of each column

#define minimumM minM
Matrix * minM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the minimum of the values
// If m is a matrix, returns a row vector with the minimum value of each column

#define maximumM maxM
Matrix * maxM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the maximum of the values
// If m is a matrix, returns a row vector with the maximum value of each column

#define minimumV minV
Matrix * minV(Matrix *m, int *index);
// If m is a row or column vector, returns a 1x1 matrix with the minimum of the values
// Returns the address of the flat index of the minimum value in index
// Note: *** the index is ONE-BASED *** to match Octave

#define maximumV maxV
Matrix * maxV(Matrix *m, int *index);
// If m is a row or column vector, returns a 1x1 matrix with the maximum of the values
// Returns the address of the flat index of the maximum value in index
// Note: *** the index is ONE-BASED *** to match Octave

#define samplevarM varM
#define sampleVarM varM
#define samplevarianceM varM
#define sampleVarianceM varM
Matrix * varM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the unbiased sample variance of the values
// If m is a matrix, returns a row vector with the unbiased sample variance of each column

#define populationVarM popvarM
#define populationVarianceM popvarM
#define popVarM popvarM
#define popVarianceM popvarM
#define populationvarM popvarM
#define populationvarianceM popvarM
#define normVarM popvarM
#define normVarianceM popvarM
#define normvarM popvarM
#define normvarianceM popvarM
Matrix * popvarM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the population variance of the values
// If m is a matrix, returns a row vector with the population variance of each column

#define samplestdM stdM
#define sampleStdM stdM
#define samplestandarddeviationM stdM
#define sampleStandarddeviationM stdM
#define samplestandardDeviationM stdM
#define sampleStandardDeviationM stdM
#define samplestdevM stdM
#define stdevM stdM
Matrix * stdM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the unbiased sample variance of the values
// If m is a matrix, returns a row vector with the unbiased sample variance of each column

#define popStdM popstdM
#define popstandarddeviationM popstdM
#define popStandarddeviationM popstdM
#define popstandardDeviationM popstdM
#define popStandardDeviationM popstdM
#define popstdevM popstdM
#define populationstdM popstdM
#define populationStdM popstdM
#define populationstandarddeviationM popstdM
#define populationStandarddeviationM popstdM
#define populationstandardDeviationM popstdM
#define populationStandardDeviationM popstdM
#define populationstdevM popstdM
Matrix * popstdM(Matrix *m);
// If m is a row or column vector, returns a 1x1 matrix with the population standard deviation of the values
// If m is a matrix, returns a row vector with the population standard deviation of each column

double int_quantile(int *vec, int N, double q);
// Finds the quantile q of an integer vector vec of length N
// vec : an integer vector. Does not need to be sorted.
// N : the length of vector vec
// q : the desired quantile
// Returns a double

double double_quantile(double *vec, int N, double q);
// Finds the quantile q of a double vector vec of length N
// vec : a double vector. Does not need to be sorted.
// N : the length of vector vec
// q : the desired quantile
// Returns a double

double complex complex_quantile(double complex *vec, int N, double q);
// Finds the quantile q of a complex vector vec of length N
// vec : a complex vector. Does not need to be sorted.
// N : the length of vector vec
// q : the desired quantile
// Returns a double complex

#define quantilesM quantileM_vect
Matrix * quantileM_vect(Matrix *m, int N, double *quantiles);
// If m is a row or column vector, returns a 1xN matrix with the specified  quantiles
// If m is a matrix of size rxc, returns a Nxc matrix with the specified quantiles of each column 

Matrix * quantileM(Matrix *m);
// If m is a row or column vector, returns a 1x5 matrix with the [0, 0.25, 0.5, 0.75, 1] quantiles
// If m is a matrix of size rxc, returns a 5xc matrix with the [0, 0.25, 0.5, 0.75, 1] quantiles of each column 

Matrix * betapdfM(Matrix *m, double a, double b);
// Computes the beta pdf at each value in m using the parameters a and b
// Returns a Matrix *

Matrix * exppdfM(Matrix *m, double lambda);
// Computes the exponential pdf at each value in m using the mean lambda
// Returns a Matrix *

Matrix * chi2pdfM(Matrix *m, double n);
// Computes the chi-square pdf at each value in m using the n degrees of freedom
// Returns a Matrix *

Matrix * gampdfM(Matrix *m, double a, double b);
// Computes the gamma pdf at each value in m using shape parameter a and rate parameter b
// Returns a Matrix *

Matrix * lognpdfM(Matrix *m, double mu, double sigma);
// Computes the lognormal pdf at each value in m using mean mu and standard deviation sigma
// Returns a Matrix *

Matrix * normpdfM(Matrix *m, double mu, double sigma);
// Computes the normal pdf at each value in m using mean mu and standard deviation sigma
// Returns a Matrix *

Matrix * unidpdfM(Matrix *m, int n);
// Computes the uniform discrete pdf at each value in m using the maximum observable value n
// Returns a Matrix *

double gammadiv(double x);
// Returns the gamma function fraction used by the hypergeometric function
// Returns gamma((x+1)/2) / gamma(x/2)

double tcdf (double x, double df);
// Get the t-distribution CDF at x with df degrees of freedom

#define t_testM ttestM
void ttestM(Matrix* restrict m, double mu, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict tstat, double* restrict df, double* restrict sd);
// For a sample m from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (m) == mu'
// Uses alpha = 0.05
// m = an integer or double row or column vector
// mu = the mean to test
// h = a boolean pointer where the result of the t-test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
// pval = a double pointer where the pval will be placed.
// ci = a double array (length 2) pointer where the confidence interval will be placed
// tstat = a double pointer where the tstat will be placed.
// df = a double pointer where the degrees of freedom will be placed.
// sd = a double pointer where the sample standard deviation will be placed.

#define t_testM_xy ttestM_xy
void ttestM_xy(Matrix *x, Matrix *y, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict tstat, double* restrict df, double* restrict sd);
// For a sample x from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (x) - mean (y) == 0'
// Uses alpha = 0.05
// m = an integer or double row or column vector
// n = an integer or double row or column vector
// h = a boolean pointer where the result of the t-test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
// pval = a double pointer where the pval will be placed.
// ci = a double array (length 2) pointer where the confidence interval will be placed
// tstat = a double pointer where the tstat will be placed.
// df = a double pointer where the degrees of freedom will be placed.
// sd = a double pointer where the sample standard deviation will be placed.

double normcdf (double x, double mu, double s);
// Get the normal distribution CDF at x with mean mu and standard deviation s

#define z_testM ztestM
void ztestM(Matrix* restrict m, double mu, double s, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict z, double* restrict zcrit);
// For a sample m from a normal distribution with unknown mean and known standard deviation, perform a z-test of the null hypothesis 'mean (m) == mu'
// Uses alpha = 0.05
// m = an integer or double row or column vector
// mu = the mean to test
// s = the standard deviation
// h = a boolean pointer where the result of the z-test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
// pval = a double pointer where the pval will be placed.
// ci = a double array (length 2) pointer where the confidence interval will be placed
// z = a double pointer where the z statistic will be placed.
// zcrit = a double pointer where the z critical value will be placed.

double chi2cdf(double chisqstat, double df);
// Estimates the Chi-square CDF at chisqstat with df degrees of freedom

void vartestM(Matrix* restrict m, double v, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict chisqstat, double* restrict df);
// For a sample m from a perform a variance test of the null hypothesis 'var (m) == v'
// Uses alpha = 0.05
// m = an integer or double row or column vector
// v = the variance to test
// h = a boolean pointer where the result of the z-test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
// pval = a double pointer where the pval will be placed.
// ci = a double array (length 2) pointer where the confidence interval will be placed
// chisqstat = a double pointer where the chi-squared statistic will be placed.


/* Attempt at complex ttest
	double tcdf_d (double x, double df);
	// Get the t-distribution CDF at x with df degrees of freedom

	double tcdf_c (double complex x, double df);
	// Get the t-distribution CDF at x with df degrees of freedom

	#define t_testM ttestM
	void ttestM(Matrix *m, void *mu, bool *h, double *pval, void *ci, void *tstat, double *df, double *sd);
	// For a sample m from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (m) == mu'
	// m = an integer or double row or column vector
	// mu = the mean to test
	// h = a boolean pointer where the result of the t_test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
	// pval = a double pointer where the pval will be placed.
	// ci = a void array (length 2) pointer where the confidence interval will be placed
	// tstat = a void pointer where the tstat will be placed.
	// df = a double pointer where the degrees of freedom will be placed.
	// sd = a double pointer where the sample standard deviation will be placed.

	#define t_testM_xy ttestM_xy
	void ttestM_xy(Matrix *x, Matrix *y, bool *h, double *pval, void *ci, void *tstat, double *df, double *sd);
	// For a sample x from a normal distribution with unknown mean and variance, perform a t-test of the null hypothesis 'mean (x) - mean (y) == 0'
	// m = an integer or double row or column vector
	// n = an integer or double row or column vector
	// h = a boolean pointer where the result of the t_test will be placed. 0 = null hypothesis is accepted; 1 = null hypothesis is rejected
	// pval = a double pointer where the pval will be placed.
	// ci = a void array (length 2) pointer where the confidence interval will be placed
	// tstat = a void pointer where the tstat will be placed.
	// df = a double pointer where the degrees of freedom will be placed.
	// sd = a double pointer where the sample standard deviation will be placed.
*/

void normfitM(Matrix *m, void *mu, void *sigma);
// Returns estimations of normal distribution parameters. mu is set to the mean, sigma is set to the standard deviation
// m = an integer or double row or column vector
// mu = a void pointer where the mean value will be placed
// sigma = a void pointer where the standard deviation will be placed

void unifitM(Matrix *m, void *a, void *b);
// Returns estimations of uniform distribution parameters. a is set to the minimum, b is set to the maximum
// m = an integer or double row or column vector
// a = a void pointer where the min value will be placed
// b = a void pointer where the max value will be placed










/*
* =========================================
* 
*  Matrix signal processing operations
*  Source code in signal_processing.c
* 
* =========================================
*/

double * hamming(int n);
// Return the coefficients of a hamming window of size n

#define hammingperiodic periodichamming
#define hammingPeriodic periodichamming
#define periodicHamming periodichamming
double * periodichamming(int n);
// Return the coefficients of a hamming window of size n with last coefficient removed

double * hanning(int n);
// Return the coefficients of a hanning window of size n

#define hanningperiodic periodichanning
#define hanningPeriodic periodichanning
#define periodicHanning periodichanning
double * periodichanning(int n);
// Return the coefficients of a hanning window of size n with last coefficient removed

Matrix * dftM(Matrix *m);
// If m is a row vector, returns a row vector with the discrete fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are discrete fourier transformed

Matrix * idftM(Matrix* restrict m);
// If m is a row vector, returns a row vector with the inverse discrete fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse discrete fourier transformed

#define fourierM fftM
Matrix * fftM(Matrix *m);
// If m is a row vector, returns a row vector with the fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are fast fourier transformed

#define inversefourierM ifftM
#define inverseFourierM ifftM
#define fourierinverseM ifftM
#define fourierInverseM ifftM
Matrix * ifftM(Matrix *m);
// If m is a row vector, returns a row vector with the inverse fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse fast fourier transformed

#define fourierV fftV
Matrix * fftV(Matrix *m, int n);
// m = a row or column vector
// n = a positive integer
// Returns = a row or column vector of the fast fourier transform using the first n elements of m. If n > m, m is padded with zeros.

#define inversefourierV ifftV
#define inverseFourierV ifftV
#define fourierinverseV ifftV
#define fourierInverseV ifftV
Matrix * ifftV(Matrix *m, int n);
// If m is a row vector, returns a row vector with the inverse fast fourier transform
// If m is a column vector or matrix, returns a matrix where the columns are inverse fast fourier transformed
// Returns = a row or column vector of the inverse fast fourier transform using the first n elements of m. If n > m, m is padded with zeros.

Matrix * stftV(Matrix *m, int win_size, int inc, int num_coef, int win_type);
// Compute the short-time Fourier transform of the vector m with num_coef coefficients by applying a window of win_size data points and an increment of inc points
// Octave uses the following defaults: win_size = 80, inc = 24, num_coef = 64, and win_type = 1

Matrix * filterM(Matrix *b, Matrix *a, Matrix *x, Matrix **state);
// Apply a 1-D digital filter to the data x
// b, a, and x are all vectors
// si is the initial state vector. If si is not NULL, on return it will be set to sf
// To use the default si and get sf returned in its place, input a vector of zeros of length max(len(a), len(b))-1
// Returns solutions to the following equation:
// length(a) - 1         length(b) - 1
// SUM a[k+1] * y[n-k] = SUM b[k+1] * x[n-k]    for 1 <= n <= length(x)
// k=0                   k=0

double internal_xcorrM_ii(int m, int * x, int x_l, int * y, int y_l, int mode);
double internal_xcorrM_id(int m, int * x, int x_l, double * y, int y_l, int mode);
double complex internal_xcorrM_ic(int m, int * x, int x_l, double complex * y, int y_l, int mode);
double internal_xcorrM_di(int m, double * x, int x_l, int * y, int y_l, int mode);
double internal_xcorrM_dd(int m, double * x, int x_l, double * y, int y_l, int mode);
double complex internal_xcorrM_dc(int m, double * x, int x_l, double complex * y, int y_l, int mode);
double complex internal_xcorrM_ci(int m, double complex * x, int x_l, int * y, int y_l, int mode);
double complex internal_xcorrM_cd(int m, double complex * x, int x_l, double * y, int y_l, int mode);
double complex internal_xcorrM_cc(int m, double complex * x, int x_l, double complex * y, int y_l, int mode);
// These functions do cross correlation between vectors x and y at a given lag m

Matrix * xcorrM(Matrix *x, Matrix *y, int maxlag, char *scale);
// Estimates cross correlation
// x, y = vectors
// maxlag = an integer for maxlag. Input 0 to use the default of max(length(x), length(y))-1.
// Returns a vector
// The cross correlation estimate between x and y for lag k is:
//           length(x)
// R_xy(k) = SUM x[i+k]*conj(y[i])
//           i=1
// Treat invalid indices as zero

#endif /* matrix_h */
