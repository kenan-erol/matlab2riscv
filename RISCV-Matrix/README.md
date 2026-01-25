# RISCV-Matrix <!-- omit in toc -->

A library of functions implemented in RISC-V Vectorized C for advanced matrix manipulation in the HALO Project.
A senior project by Zach Taylor, Yale University
21 December 2022

### Contributors

- Zach Taylor ([@ZTaylor39](https://github.com/ZTaylor39))
- Xiayuan Wen ([@XiayuanW](https://github.com/XiayuanW))

## Table of Contents <!-- omit in toc -->
- [Motivation](#motivation)
- [Environment](#environment)
  - [Octave](#octave)
    - [Differences between Octave and MATLAB](#differences-between-octave-and-matlab)
      - [Complex Number Ordering](#complex-number-ordering)
    - [Differences between C and Octave](#differences-between-c-and-octave)
      - [Storage](#storage)
      - [Indexing](#indexing)
  - [RISC-V GNU Compiler Toolchain](#risc-v-gnu-compiler-toolchain)
  - [Spike Simulator](#spike-simulator)
  - [External Libraries](#external-libraries)
  - [Setting up Your Environment](#setting-up-your-environment)
- [Configuring and Building the matrix library](#configuring-and-building-the-matrix-library)
  - [Matrix Library Configuration Options](#matrix-library-configuration-options)
    - [COMPARISON\_MODE](#comparison_mode)
    - [EPSILON](#epsilon)
    - [MODE](#mode)
  - [Building the matrix library](#building-the-matrix-library)
- [Code Design](#code-design)
  - [The Matrix](#the-matrix)
  - [Data Types](#data-types)
  - [The Matrix Library](#the-matrix-library)
  - [Naming Conventions](#naming-conventions)
    - [RVV Vectorization](#rvv-vectorization)
    - [Vectorizing More Functions](#vectorizing-more-functions)
    - [Comments on Function Vectorization](#comments-on-function-vectorization)
  - [Memory Allocation](#memory-allocation)
  - [Basic Anatomy of a Reference Matrix Function](#basic-anatomy-of-a-reference-matrix-function)
  - [Basic Anatomy of a Vectorized Matrix Function](#basic-anatomy-of-a-vectorized-matrix-function)
  - [Vector Processor Architecture](#vector-processor-architecture)
  - [Table of Vectorization Status](#table-of-vectorization-status)
  - [Proposed Functions](#proposed-functions)
- [Testing](#testing)
  - [tests\_C\_Octave](#tests_c_octave)
    - [tests\_C\_Octave Makefile command reference](#tests_c_octave-makefile-command-reference)
  - [tests\_Vec\_C](#tests_vec_c)
    - [Speed \& Accuracy Reports](#speed--accuracy-reports)
    - [tests\_Vec\_C Makefile command reference](#tests_vec_c-makefile-command-reference)
- [Preliminary Results](#preliminary-results)
  - [C vs Octave](#c-vs-octave)
  - [RVV C vs Reference C](#rvv-c-vs-reference-c)
    - [Elementwise Absolute Value: absM](#elementwise-absolute-value-absm)
    - [Elementwise Addition: plusM](#elementwise-addition-plusm)
    - [Matrix Multiplication: mtimesM](#matrix-multiplication-mtimesm)
- [Future Directions](#future-directions)
- [Known Bugs](#known-bugs)
- [Acknowledgements](#acknowledgements)
- [Sources](#sources)

## Motivation

This project aims to lay the groundwork for software to be used by the [HALO](https://ieeexplore.ieee.org/document/9138938)[^1] (Hardware Architecture for LO-power BCIs) project. HALO uses a RISC-V micro-controller to achieve low-power operation of an implantable Brain-Computer Interface (BCI) chip. These BCI chips aim to record and analyze neural activity linked to seizures and other disorders. This design has two equally important constraints that must be balanced:
- The device must run on low power to maximize the longevity of studies. Since BCIs are implanted on the surface of the brain, we don't want to have to change their batteries often.
- BCIs must be capable of real-time, complex data processing, in the form of matrix data analysis.

These two constraints demand a software design that places a premium on power efficiency. [The RISC-V "V" Vector Extension](https://github.com/riscv/riscv-v-spec/blob/master/v-spec.adoc)[^2] (hereinafter referred to as "RVV") is an extension of the RISC-V architecture that includes assembly instructions capable of performing elementwise operations on entire vectors in parallel. 
Rather than adding two vectors by iterating through each element of their elements, RVV is capable of loading in entire vectors (or, at least, large segments of them), and executing the operation on the entire vector at once. 
RVV vector assembly instructions have also been wrapped in C-language [functions](https://github.com/riscv-non-isa/rvv-intrinsic-doc/blob/master/rvv-intrinsic-api.md)[^3], allowing for C code to seamlessly incorporate RVV instructions when compiled using the RISC-V GNU compiler toolchain.

Taking advantage of RVV instructions greatly accelerates data processing on vectors (and, by extension, matrices). Using these instructions, though, requires that researchers write programs in RVV Assembly (or at least in C). However, in the field of neuroscience, most of the researchers program and analyze data in MATLAB, which is a much higher-level language than C or Assembly.

This project aims to write a C-language library using RVV Assembly instructions that will enable highly efficient matrix manipulation. We anticipate that RISCV-MATRIX will be able to mimic MATLAB matrix manipulation functions over 100x faster. (Of course, the exact performance increase is highly situational due to a multitude of factors -- more on that below).


## Environment

### Octave

[GNU Octave](https://octave.org/) is a powerful high-level language geared towards numerical operations. It operates almost identically to MATLAB, with the main difference being that it is open-source. More differences are listed below.
As a programming language, Octave is a natural choice for working with matrices. However, it's high-level interface introduces undesirable performance inefficiencies.

#### Differences between Octave and MATLAB

Octave and MATLAB are the same in almost every way. A page outlining many (but not all) of their differences can be found [here](https://en.wikibooks.org/wiki/MATLAB_Programming/Differences_between_Octave_and_MATLAB). 

##### Complex Number Ordering

One salient difference between Octave and MATLAB is how they order complex numbers. 
[MATLAB](https://www.mathworks.com/help/matlab/matlab_prog/array-comparison-with-relational-operators.html) orders complex numbers only by their real parts. 

$z_1 < z_2 \Longleftrightarrow \Re(z_1) < \Re(z_2)$

[Octave](https://docs.octave.org/v4.2.0/Comparison-Ops.html), on the other hand, orders complex numbers by their magnitudes, breaking ties by their arguments. 

$z_1 < z_2 \Longleftrightarrow |z_1| < |z_2| \lor (|z_1| = |z_2| \land \text{arg}(z_1) < \text{arg}(z_2))$

RISCV-MATRIX supports both comparison modes as the configurable option `COMPARISON_MODE` in `src/matrix.h`.

#### Differences between C and Octave

##### Storage

The first main difference between C and Octave is how matrices are stored. Matrices in C are stored row-major, where two successive elements in the same *row* are stored successively in memory. Octave (like MATLAB) is column-major, where two successive elements in the same *column* are stored successively in memory. 
This difference may be observed in the tests_C_Octave (which also serve as examples of equivalent C and Octave matrix code). 

##### Indexing

The second main difference between C and Octave is how they are indexed. By nature, C is zero-indexed. The first element of an array in C is given by `arr[0]`. However, Octave is one-indexed. The first element of an array is given by `arr[1]`.
Since the goal of this project is for C to resemble Octave as closely as possible, I decided to implement one-indexing for the interface of RISCV-MATRIX. The only functions where is relevant are `indexM` and `reindexM`.

### RISC-V GNU Compiler Toolchain

The [RISC-V GNU Compiler Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) is the RISC-V C/C++ compiler. It is used to compile the matrix library and programs that use the matrix library. Follow the installation instructions on their README.

Another toolchain, the [LLVM Compiler](https://github.com/llvm/llvm-project), was also considered for this project, but we ultimately decided to use the RISC-V GNU Compiler Toolchain.

### Spike Simulator

In order to simulate running the vectorized code on a vector hardware processor, we used the [Spike simulator](https://github.com/riscv-software-src/riscv-isa-sim). Spike is the RISC-V ISA Simulator.

Once both the RISC-V GNU Compiler Toolchain and the Spike simulator are installed, you should be able to compile & run a simple program:

```
riscv64-unknown-elf-gcc -DUSE_RISCV_VECTOR  -march=rv64gcv1p0 -g3 -pedantic -Wall -O2 -o main.riscv main.c

spike --isa=rv64gcv --varch=vlen:1024,elen:64 pk main.riscv
```
N.B.: the `vlen` option (a.k.a., VLMAX) passed to spike is set to the max value of 1024, but that setting will ultimately depend on the parameters of the hardware.

The `elen` option (set to 64) determines the maximum number of bits that any single element of a vector may occupy.

### External Libraries

- fftw-3.3.10 - A Fast-Fourier Transform library.
  - The "Fastest Fourier Transform in the West", [fftw](https://www.fftw.org/) is a C library for calculating Fourier Transformations.
  - N.B.: The functions in fftw-3.3.10 do not run using RVV Assembly instructions, the library is just compiled with the RISC-V GNU Compiler Toolchain.
- riscv-blas - A CBLAS and LAPACKE implementation compiled with the RISC-V GNU Toolchain.
  - [BLAS](https://netlib.org/blas/) is a library of routines vital for performing simple vector and matrix operations (e.g., matrix multiplication). Originally designed in Fortran, it is converted to C in `riscv-blas/CBLAS`.
  - [LAPACKE](https://netlib.org/lapack/lapacke.html) is the C-language interface to LAPACK (Linear Algebra Package), which provides higher-level routines for matrix operations (e.g., matrix eigendecomposition).
  - N.B.: The functions in riscv-blas do not run using RVV Assembly instructions, the library is just compiled with the RISC-V GNU Compiler Toolchain.
- BSC-RVV - Example code that provides examples of and benchmarks RVV vectorized C code.
  - Pulled from [BSC-RVV](https://github.com/XiayuanW/BSC-RVV).
- tgmath.h - My slightly-modified implementation of the standard built-in C library.
  - [tgmath](https://en.cppreference.com/w/c/numeric/tgmath) stands for "type-generic math". It is a C/C++ built-in library that enables the same numerical functions to be called on different input types.
  - N.B.: I did not change any functionality of tgmath.h, I merely added extra parentheses around some function calls so that consistency in operator precedence is guaranteed when switching between the standard gcc compiler and the RISC-V GNU compiler toolchain.

### Setting up Your Environment

1. Install [GNU Octave](https://www.geeksforgeeks.org/how-to-install-gnu-octave-in-linux/)
2. Install the [RISC-V GNU Compiler Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain).
3. Install the [Spike simulator](https://github.com/riscv-software-src/riscv-isa-sim).
4. If you're working on a Grace node, run the command `setup stdtools` to setup the environment variables for standard tools.
   - `setup stdtools` must be run on each new Grace node you use.


## Configuring and Building the matrix library

### Matrix Library Configuration Options

Before compiling the matrix library, it is important to check that you have configured it properly in `src/matrix.h`.

#### COMPARISON_MODE

As discussed above, Octave and MATLAB order complex numbers differently. RISCV-MATRIX supports both comparison modes. Set the comparison mode by setting the `COMPARISON_MODE` variable in `src/matrix.h` to either `OCTAVE` or `MATLAB`.

#### EPSILON

Comparing floating-point numbers (such as doubles or double complexes) is inherently tricky. Two floating-point numbers may be equal to *some* precision, but not to *infinite* precision due to inherent storage limitations and accumulated rounding errors. 
Instead of checking that two doubles are exactly equal, we determine if the magnitude of the difference between two floating-point numbers is smaller than some epsilon value.
The variable `EPSILON` sets the threshold for equality between two floating-point numbers.

#### MODE

The `MODE` variable allows the user to declare whether functions should use their "vectorized" version (i.e., using RVV Assembly instructions) or their "reference" version (i.e., using standard C-language instructions). There are three options for `MODE`:
- `REF` - calls to a function will always call the non-vectorized version.
- `VEC` - calls to a function will always call the vectorized version (if it exists).
- `DEFAULT` - calls to a function will use the mode that is considered default for that function. The default version of a function is defined in the source code for that function.

### Building the matrix library

Running `make matrix` from root , `src/`, `tests_C_Octave/` , or `tests_Vec_C/` will compile the matrix library.

## Code Design

My initial work on RISCV-MATRIX began in February 2021. I have tried to maintain a consistent design philosophy over that time, while also doing my best to incorporate lessons learned along the way. There are some aspects of the library at which I would like to redesign, but that is beyond the scope of my involvement in this project. I have included ideas for refactoring in a later section.

### The Matrix

```
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
```
A struct matrix (or, simply, Matrix) is a struct containing a Vector and information about how to interpret that Vector as a Matrix (a number of dimensions, and a list of those dimensions).

Matrices from 1D to 4D are technically supported, though many individual functions will only accept 1D and 2D Matrix inputs.

A Vector contains an array of data, an indicator for the type of that data, and the size (number of elements) contained in the array.

Vectors also have a field `readOnly`. The idea here is that two matrices &mdash; possibly of different dimensions though containing the same total number of elements &mdash; could contain the same values. Rather than storing those values in multiple locations, two matrices could reference a single copy of shared data. The integer value of `readOnly` indicates how many matrices are sharing the same data. Usually, this value is 0. When it is not, however, it indicates the number of matrices (minus 1) that are sharing the data, preventing any of them from modifying the data via `writeM`.

### Data Types

Matrices may only contain data in one of three data types:
- Integers (size 32), referred to as INTEGER.
- Doubles (size 64), referred to as DOUBLE.
- Double complexes, referred to as COMPLEX.
  - Double complexes consist of a pair of size-64 doubles stored adjacently in memory. 
  - The first double contains the real part of the complex number, and the second double contains the imaginary part.

All elements within a matrix must have the same data type.

### The Matrix Library

The matrix library, contained within the `src/` directory, is broken up over several source files. All these source files share a single `src/matrix.h` header file. Any code wanting to use the matrix library should be linked with `src/libmatrix.a` and include `src/matrix.h`.
- `src/matrix.h` contains all function headers with documentation about each function's inputs, outputs, and usage.
- `src/matrix.c` contains the most basic functions of the matrix library. It contains matrix constructors (`createM`, `onesM`, `zerosM`, etc.), destructors (`destroyM`), readers (`getdataM`, `getsizeM`, etc.), writers (`writeM`), printers (`printM`, `fprintM`), and accessors (`indexM`, `reindexM`).
- `src/elementwise_comparison.c` defines functions for elementwise comparison operators between two matrices (e.g., `equalsM`).
- `src/elementwise_logic.c` defines functions for elementwise logic operations on matrices (e.g., `orM`).
- `src/elementwise_math.c` defines functions for elementwise math operations between one or two matrices (e.g., `absM` or `plusM`).
- `src/matrix_math.c` defines functions for matrix math operations (e.g., `mtimesM`).
- `src/signal_processing.c` defines functions for matrix signal processing (e.g., `fftM`).
- `src/stats.c` defines functions for matrix statistical functions (e.g., `meanM`).
- `src/utils.c` defines various utility functions, such as typecasting.

### Naming Conventions

Functions that take a matrix as an input, as well as matrix creators, are named `functionM`. Where possible, I have tried to mimic Octave function names, and added alternate spellings/calls for several functions (e.g., `detM` and `determinantM` call the same function).

Typecasting is an exception to this convention. These functions, of the form `X_to_Y (Matrix* restrict m)` (where X and Y are each one of "i", "d", or "c", corresponding to INTEGER, DOUBLE, and COMPLEX, respectively). X is the type of the input matrix, and Y is the type of the output **ARRAY**. 
- N.B.: the typecasting functions DO NOT return matrices; they only return the array of matrix data properly casted and in a newly-allocated chunk of memory. The outputs of these typecasting functions must later be free'd.

#### RVV Vectorization

Many of the matrix functions have been "vectorized", meaning that they have been written in such a way as to make use of the RVV Assembly functions. (An exhaustive list of vectorized functions is below).
For all vectorized functions, there are three flavors. I'll use `absM` (the elementwise absolute value function) as an example:
- `absM_vec` - The RVV vectorized version.
- `absM_ref` - The standard, non-vectorized version.
- `absM` - A wrapper function that calls the version of `absM_XXX` as specified by the configurable variable `MODE`, or the default version.

For the sake of testing, `_ref` and `_vec` versions (if they exist) are always directly callable. Functions that have not yet been vectorized do not have `_ref` and `_vec` versions (but they should get them whenever they are vectorized).

The `foo_vec` versions of functions do not typically themselves contain direct calls to RVV vectorized functions. Instead, they will call a function `foo_vec_X[Y]`, where X (and Y, for functions with two input matrices) is one of "i", "d", or "c", corresponding to INTEGER, DOUBLE, and COMPLEX, respectively, indicating the type of the input matrix. `foo_vec_X[Y]` will handle the actual RVV Assembly segment of the matrix operation, while `foo_vec` will do all necessary setup & teardown. (This is different from `foo_ref` functions, which do their own dirty work.)

N.B.: An exception to this rule is `naive_mtimesM_vec`, which, as currently written, directly calls RVV Assembly functions.

#### Vectorizing More Functions

A big next step of this project is to continue vectorizing functions until as much of RISCV-MATRIX uses vectorized functions as possible.

For vectorizing functions, the following are invaluable sources of information:

1. [The Official RISC-V "V" Extension Spec](https://github.com/riscv/riscv-v-spec/blob/master/v-spec.adoc)
   - The most up-to-date specification document for RVV.
2. [UC Berkeley's Copy of the RISC-V "V" Extension Spec](https://inst.eecs.berkeley.edu/~cs152/sp20/handouts/sp20/riscv-v-spec.html).
   - A slightly older version of the RVV official spec that includes some tables and other auxiliary information.
   - You can generate the same document from the most up-to-date spec [here](https://github.com/riscv/riscv-v-spec/releases/tag/v1.0).
3. [C-Language RVV functions](https://github.com/riscv-non-isa/rvv-intrinsic-doc)

#### Comments on Function Vectorization

Within the context of RVV Vectorization, the instructions that take the longest amount of time to execute are load & store. I have written most of the RVV code to minimize the number of loads & stores that are called. Frequently, this means that vectorized functions will contain duplicate code. Rather than calling other vectorized functions for pre-processing and/or post-processing (which would add to the number of loads & stores), I have opted to repeat some code. 
This certainly causes some bloating in the codebase (`src/elementwise_comparison.c` is a prime example), but I do believe it makes the code (marginally) more efficient. 

Whether this marginal improvement in efficiency is worth the loss in codebase maintainability is an open question. This principle of the RVV Vectorization process is certainly one to take another look at.

### Memory Allocation

Matrix operations are not done in-place. Input matrix data is safe from being modified. Only the `writeM` function will modify the data of an existing matrix. Any function that returns a matrix will allocate that matrix itself. 

**This means that ALL matrices returned by function calls must be `destroyM`-ed to avoid memory leaks.** Furthermore, this means that the outputs of all matrix operations must be directly assigned to a variable so that they can later be `destroyM`-ed. Matrix operations cannot be chained. Otherwise, the space allocated by the Matrix operation will never be free'd.

See the example below:
```
int ndim = 2;        // 2D matrix
int dim[2] = {3,3};  // 3x3 dimensions

Matrix *zero = zerosM(ndim, dim); // Create a 3x3 INTEGER matrix of zeros
Matrix *one = onesM(ndim, dim);   // Create a 3x3 INTEGER matrix of ones
Matrix *id = idM(ndim, dim);      // Create a 3x3 INTEGER identity matrix

// This is correct:
Matrix *first_sum = plusM(one, id);
Matrix *second_sum = plusM(first_sum, zero);

// This is incorrect and will cause memory leaks:
Matrix *bad_sum = plusM(plusM(one, id), zero);

destroyM(zero);
destroyM(one);
destroyM(id);
destroyM(first_sum);
destroyM(second_sum);
destroyM(bad_sum);
```

### Basic Anatomy of a Reference Matrix Function

Below is an annotated example of the anatomy of a typical Reference Matrix Function:

```
// Function header ending in _ref, indicating the existence of absM_vec
Matrix * absM_ref(Matrix* restrict m) 
// Description of the operation
// Returns the elementwise absolute value of Matrix m
{
  // Error checking
  if (m == NULL)                      
  {
    return(NULL);
  }
  
  // Output declaration
  Matrix *output = NULL;              

  // Temporary variables for things we'll be using frequently and don't want to have to access each time.
  int m_data_size = m->data->size;
  void *m_data_data = m->data->data;
  void *output_data_data = NULL;

  // Because the matrix data could be any of three types, each of a different
  // size, we must have three distinct ways of loading and storing the 
  // data as different types.
  // This is an unfortunate result of the lack of polymorphism in C. 
  // For two-input functions, each case has a nested switch statement.
  switch(m->data->datatype)
  {
    case INTEGER: ;

      // Construct the INTEGER-type output
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, INTEGER);      
      output_data_data = output->data->data;

      // Iterate over the input and execute the operation
      for (register int i = 0 ; i < m_data_size ; i++) 
      {
        // Notice that we have to properly cast the input and output types
        ((int *)(output_data_data))[i] = (int)(fabs(((int *)(m_data_data))[i]));
      }
      break;

    case DOUBLE: ;
      // Construct the DOUBLE-type output
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, DOUBLE);
      output_data_data = output->data->data;

      // Iterate over the input and execute the operation
      for (register int i = 0 ; i < m_data_size ; i++)
      {
        // Notice that we have to properly cast the input and output types
        ((double *)(output_data_data))[i] = (double)(fabs(((double *)(m_data_data))[i]));
      }
      break;

    case COMPLEX: ;
      // Construct the COMPLEX-type output
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, DOUBLE);
      output_data_data = output->data->data;

      // Iterate over the input and execute the operation
      for (register int i = 0 ; i < m_data_size ; i++)
      {
        // Notice that we have to properly cast the input and output types
        ((double *)(output_data_data))[i] = fabs(((double complex *)(m_data_data))[i]);
      }
      break;
  }

  // Return
  return(output);
}
```

### Basic Anatomy of a Vectorized Matrix Function

Below is an annotated example of the anatomy of a typical Vectorized Matrix Function:

```
// Function header ending in _vec_d, indicating that the 
// function takes one input (x) of type DOUBLE.
// Here, y points to the already-initialized output.
// n is the size (number of elements) of the input/output
void absM_vec_d(double* restrict y, double* restrict x, int n)
// Description of the function
{
  // Reasoning for LMUL & SEW settings, which change from function to function.
  // SEW (e64) is the size of the elements
  // LMUL (m8) is the number of vector registers in a single group.
  long trueVL = vsetvl_e64m8(n); // num elements in a vector that can be simultaneously operated on

  // Begin iteration
  // Each iteration loads in as many elements as it can based on 
  // LMUL & SEW settings
  int i;
  for (i = 0; i < n;) {

    // Recalculate how many elements to operate on in the next iteration of the loop
    trueVL = vsetvl_e64m8(n - i);

    // Load the doubles from the input
    vfloat64m8_t v_x = vle64_v_f64m8(&x[i], trueVL);

    // Vectorized operation
    vfloat64m8_t v_res = vfsgnjx_vv_f64m8(v_x, v_x, trueVL); // absolute value by sign-injection

    // Store
    vse64_v_f64m8(&y[i], v_res, trueVL);

    // Increment by the number of elements operated on
    i += trueVL;
  }

  FENCE();
}

// Notice that the _vec version looks very similar to the _ref version
// The only difference is that where the _ref function enters a for-loop,
// the _vec version calls a vectorized helper.
Matrix * absM_vec(Matrix* restrict m)
// Returns the elementwise absolute value of Matrix m
{
  // Error checking
  if (m == NULL)
  {
    return(NULL);
  }

  // Output declaration
  Matrix *output = NULL;

  // Temporary variables for things we'll be using frequently and don't want to have to access each time.
  int m_data_size = m->data->size;
  void *m_data_data = m->data->data;
  void *output_data_data = NULL;

  switch(m->data->datatype)
  {
    case INTEGER: ;
      // Construct the INTEGER-type output
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, INTEGER);
      output_data_data = output->data->data;

      // Notice that we still have to cast the input & output types, so we have a _vec_i version of the function.
      absM_vec_i((int *)(output_data_data), (int *)(m_data_data), m_data_size);
      break;

    case DOUBLE: ;
      // Construct the INTEGER-type output
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, DOUBLE);
      output_data_data = output->data->data;

      // Notice that we still have to cast the input & output types, so we have a _vec_d version of the function.
      absM_vec_d((double *)(output_data_data), (double *)(m_data_data), m_data_size);
      break;

    case COMPLEX: ;
      // Construct the DOUBLE-type output (the absolute value of a complex number is a double)
      // Note that this allocates a new matrix object that
      // must later be free'd by the user.
      output = createM(m->ndim, m->dim, DOUBLE);
      output_data_data = output->data->data;

      // Notice that we still have to cast the input & output types, so we have a _vec_c version of the function.
      absM_vec_c((double *)(output_data_data), (double complex *)(m_data_data), m_data_size);
      break;
  }

  // Return
  return(output);
}
```

### Vector Processor Architecture

See the slides in `RISCV-v-spec_slides.pdf`, as well as the [RISCV-V Spec](https://github.com/riscv/riscv-v-spec/blob/master/v-spec.adoc#mapping-of-vector-elements-to-vector-register-state) for a detailed look into the low-level workings of vector programming architecture.
Here are some main points to give a rough idea:
- A RISCV-V vector architecture has 32 vector registers.
- VLEN is the number of bits in a single vector register. It must be a power of 2. **This is a hardware constant**, set when running a program with `spike`. All our simulations used VLEN=1024.
- We can group adjacent vector registers to store longer or shorter vectors. The more vector registers in a single group, the greater the number of elements that we can operate on at once. LMUL is the number of registers in a single group.
  - LMUL must be a power of 2 (possibly fractional, for splitting one register into multiple groups, but we never actually do this).
  - The max LMUL is 8.
  - When it comes to LMUL, bigger is better, as bigger LMUL corresponds to more elements that can be operated on at once.
  - However, you don't want LMUL to be larger than the number of temporary variables used in the function, or else you risk having implicit loads & stores, decreasing performance.
  - LMUL can (and does) change from function to function.
- SEW = Single-Element Width. This is the number of bits in a single slot of a vector register. Notably, SEW is dynamic, even within a single function. (This is useful when it's necessary to cast between integers and doubles).
  - For integers, SEW = 32
  - For doubles, SEW = 64
  - For complexes, SEW = 64x2
    - It is convenient to treat complexes as just doubles, if an operation behaves the same on the real & imaginary parts of a complex number (e.g., addition).
    - Otherwise, we will typically load in the real & imaginary parts of complex array separately (using strided loads) and go from there. 
- VLMAX, the maximum number of elements we can operate on at a time, is given by: $\text{VLMAX} = (\frac{\text{VLEN}}{\text{SEW}})(\text{LMUL})$
- When selecting LMUL and SEW for a vectorized function,
  - SEW should be set to the number corresponding to the datatype of the output.
  - In most cases, LMUL should be 32 divided by the smallest power of two greater than or equal to the number of vector variables used in the function (including those used to load in the variables).
    - LMUL has a maximum value of 8.
    - This is something that I was initially confused about, and so several functions need to be checked to ensure optimal LMUL choices (listed below).
    - Setting LMUL too high will cause extra loads and stores to be called implicitly, slowing down the program.

### Table of Vectorization Status

Functions are listed in the order in which their headers appear in `src/matrix.h`.
If a function has `_ref` and `_vec` versions, they are not shown.


|Function|Vectorizable?/Worth vectorizing?|Vectorized?|Default Mode|Notes|
|--|--|--|--|--|
|createM|No|*n/a*|*n/a*|*n/a*|
|duplicateM|No|*n/a*|*n/a*|*n/a*|
|full_duplicateM|Yes|Yes|Ref|*n/a*|
|identityM|No|*n/a*|*n/a*|*n/a*|
|zerosM|No|*n/a*|*n/a*|*n/a*|
|onesM|Yes|Yes|Ref|*n/a*|
|randomSeed|No|*n/a*|*n/a*|*n/a*|
|resetSeed|No|*n/a*|*n/a*|*n/a*|
|randM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random floats|
|randiM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random ints|
|randnM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random floats|
|reindexM|Possibly|No|*n/a*|*n/a*|
|destroyM|No|*n/a*|*n/a*|*n/a*|
|writeM|Yes|Yes|Ref|LMUL settings need checking|
|indexM|No|*n/a*|*n/a*|*n/a*|
|printM|No|*n/a*|*n/a*|*n/a*|
|fprintM|No|*n/a*|*n/a*|*n/a*|
|getnDimM|No|*n/a*|*n/a*|*n/a*|
|getDimsM|No|*n/a*|*n/a*|*n/a*|
|getdataM|No|*n/a*|*n/a*|*n/a*|
|gettypeM|No|*n/a*|*n/a*|*n/a*|
|getsizeM|No|*n/a*|*n/a*|*n/a*|
|getreadonlyM|No|*n/a*|*n/a*|*n/a*|
|reshapeM |No|*n/a*|*n/a*|*n/a*|
|can_matrices_be_compared|No|*n/a*|*n/a*|*n/a*|
|almostEqual_ii|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_id|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_ic|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_di|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_dd|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_dc|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_ci|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_cd|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|almostEqual_cc|Yes|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. <br>- Vectorization of equalM_internal_vec is a starting point.|
|i_to_i|Yes|Yes|Ref|LMUL settings need checking|
|i_to_d|Yes|Yes|Ref|LMUL settings need checking|
|i_to_c|Yes|Yes|Ref|LMUL settings need checking|
|d_to_i|Yes|Yes|Ref|LMUL settings need checking|
|d_to_d|Yes|Yes|Ref|LMUL settings need checking|
|d_to_c|Yes|Yes|Ref|LMUL settings need checking|
|c_to_i|Yes|Yes|Ref|LMUL settings need checking|
|c_to_d|Yes|Yes|Ref|LMUL settings need checking|
|c_to_c|Yes|Yes|Ref|LMUL settings need checking|
|int_comp_asc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|int_comp_desc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|double_comp_asc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|double_comp_desc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|complex_comp_asc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|complex_comp_desc|Maybe|No|*n/a*|- Currently written to compare two individual elements<br>- Could be vectorized if it took in vector arguments.<br>- Honestly probably a good idea to do.<br>- Would greatly simplify all the elementwise comparisons. |
|containsOverflow|Yes|No|*n/a*|*n/a*|
|get_time|No|*n/a*|*n/a*|*n/a*|
|elapsed_time|No|*n/a*|*n/a*|*n/a*|
|get_cycles|No|*n/a*|*n/a*|*n/a*|
|elapsed_cycles|No|*n/a*|*n/a*|*n/a*|
|transposeM|Yes|No|*n/a*|*n/a*|
|ctransposeM|Yes|No|*n/a*|*n/a*|
|determinantM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|invertM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|eigM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|trueM|Yes|Yes|Vec|Needs testing against Octave|
|andM|Yes|Yes|Ref|*n/a*|
|orM|Yes|Yes|Ref|*n/a*|
|notM|Yes|Yes|Ref|*n/a*|
|isEqualM|Yes|Yes|Ref|*n/a*|
|equalM|Yes|Yes|Ref|LMUL settings need checking|
|areEqualM|Maybe|No|*n/a*|*n/a*|
|gtM|Yes|Yes|Ref|LMUL settings need checking|
|geM|Yes|Yes|Ref|- Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes<br>- LMUL settings need checking|
|ltM|Yes|Yes|Ref|LMUL settings need checking|
|leM|Yes|Yes|Ref|- Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes<br>- LMUL settings need checking|
|neM|Yes|Yes|Ref|- Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes<br>- LMUL settings need checking|
|pairwise_maxM|Yes|Yes|Ref|- Reference version is slow because it calls several other functions to avoid different behaviors for COMPARISON_MODE. Reference version needs improvement<br>- LMUL settings need checking|
|pairwise_minM|Yes|Yes|Ref|- Reference version is slow because it calls several other functions to avoid different behaviors for COMPARISON_MODE. Reference version needs improvement<br>- LMUL settings need checking|
|argM|Yes|Yes|Vec|Needs testing against Octave|
|absM|Yes|Yes|Vec|*n/a*|
|floorM|Yes|Yes|Vec|LMUL settings need checking|
|ceilM|Yes|Yes|Vec|LMUL settings need checking|
|roundM|Yes|Yes|Vec|LMUL settings need checking|
|scaleM|Yes|Yes|Vec|*n/a*|
|scalarpowerM|Yes|No|*n/a*|*n/a*|
|sinM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|sindM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|cosM|Yes|No|*n/a*|- Depends on ability (or lack thereof) of doing trig functions in RVV<br>- Might be able to use `BSC-RVV/common/__cos.h`|
|cosdM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|tanM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|tandM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|plusM|Yes|Yes|Vec|*n/a*|
|minusM|Yes|Yes|Vec|*n/a*|
|timesM|Yes|Yes|Vec|LMUL settings need checking|
|rdivideM|Yes|Yes|Vec|LMUL settings need checking|
|ldivideM|Yes|Yes|Vec|LMUL settings need checking|
|powerM|Yes|No|*n/a*|This function differs from Octave by some amount of rounding error.|
|mtimesM|Maybe|- Naive version vectorized<br>- BLAS version not vectorized|BLAS implementation|- Depends on vectorization of BLAS<br>- Vectorized implementation differs from BLAS/naive implementation by some rounding error|
|mrdivideM|Maybe|No|*n/a*|Depends on vectorization of BLAS (for mtimesM) & LAPACKE (invertM)|
|mldivideM|Maybe|No|*n/a*|Depends on vectorization of BLAS (for mtimesM) & LAPACKE (invertM)|
|mpowerM|Yes|No|*n/a*|Unable to raise non-diagonalizable matrices to non-integer powers|
|sort|Maybe|No|*n/a*|*n/a*|
|sortM|Maybe|No|*n/a*|*n/a*|
|mean|Yes|No|*n/a*|*n/a*|
|meanM|Yes|No|*n/a*|*n/a*|
|medianM|Maybe|No|*n/a*|*n/a*|
|minM|Maybe|No|*n/a*|*n/a*|
|maxM|Maybe|No|*n/a*|*n/a*|
|minV|Maybe|No|*n/a*|*n/a*|
|maxV|Maybe|No|*n/a*|*n/a*|
|varM|Maybe|No|*n/a*|*n/a*|
|popvarM|Maybe|No|*n/a*|*n/a*|
|stdM|Maybe|No|*n/a*|*n/a*|
|popstdM|Maybe|No|*n/a*|*n/a*|
|int_quantile|Maybe|No|*n/a*|*n/a*|
|double_quantile|Maybe|No|*n/a*|*n/a*|
|complex_quantile|Maybe|No|*n/a*|*n/a*|
|quantileM_vect|Maybe|No|*n/a*|*n/a*|
|quantileM|Maybe|No|*n/a*|*n/a*|
|betapdfM|Maybe|No|*n/a*|*n/a*|
|exppdfM|Maybe|No|*n/a*|*n/a*|
|chi2pdfM|Maybe|No|*n/a*|*n/a*|
|gampdfM|Maybe|No|*n/a*|*n/a*|
|lognpdfM|Maybe|No|*n/a*|*n/a*|
|normpdfM|Maybe|No|*n/a*|*n/a*|
|unidpdfM|Maybe|No|*n/a*|*n/a*|
|gammadiv|Maybe|No|*n/a*|- Currently written to operate on a single element<br>- Maybe could be vectorized if it took in vector arguments.|
|tcdf |Maybe|No|*n/a*|- Currently written to operate on a single element<br>- Maybe could be vectorized if it took in vector arguments.|
|ttestM|Maybe|No|*n/a*|*n/a*|
|ttestM_xy|Maybe|No|*n/a*|*n/a*|
|normcdf |Maybe|No|*n/a*|- Currently written to operate on a single element<br>- Maybe could be vectorized if it took in vector arguments.|
|ztestM|Maybe|No|*n/a*|*n/a*|
|chi2cdf|Maybe|No|*n/a*|- Currently written to operate on a single element<br>- Maybe could be vectorized if it took in vector arguments.|
|vartestM|Maybe|No|*n/a*|*n/a*|
|normfitM|Maybe|No|*n/a*|*n/a*|
|unifitM|Maybe|No|*n/a*|*n/a*|
|hamming|Maybe|No|*n/a*|*n/a*|
|periodichamming|Maybe|No|*n/a*|*n/a*|
|hanning|Maybe|No|*n/a*|*n/a*|
|periodichanning|Maybe|No|*n/a*|*n/a*|
|dftM|Maybe|No|*n/a*|Needs testing against Octave|
|idftM|Maybe|No|*n/a*|Needs testing against Octave|
|fftM|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|ifftM|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|fftV|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|ifftV|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|stftV|Maybe|No|*n/a*|Needs testing against Octave|
|filterM|Maybe|No|*n/a*|Needs testing against Octave|
|xcorrM|Maybe|No|*n/a*|*n/a*|


### Proposed Functions

Some functions that don't currently exist in RISCV-MATRIX, but may be useful:
- `Matrix * reciprocalM(Matrix * m)` - elementwise $1/x$ (could either be a shortcut for `scalarpowerM`, or a call `scalarpowerM`)
- `Matrix * expM(Matrix * m)` - elementwise $e^x$
- `Matrix * logM(Matrix * m)` / `Matrix * lnM(Matrix * m)` - elementwise $\ln(x)$
- `Matrix * log10M(Matrix * m)` - elementwise $\log_{10}(x)$
- `Matrix * log2M(Matrix * m)` - elementwise $\log_2(x)$
- `Matrix * logbM(Matrix * m, void * b, int type)` - elementwise $\log_b(x)$, where `int type` is the numerical type (INTEGER, DOUBLE, or COMPLEX) of `*b`.
- `Matrix * mexpM(Matrix * m)` - $e$ to the power of a matrix.
- `Matrix * mlogM(Matrix * m)` / `Matrix * mlnM(Matrix * m)` - natural logarithm of a matrix.
- `Matrix * mlog10M(Matrix * m)` - log base 10 of a matrix.
- `Matrix * mlog2M(Matrix * m)` - log base 2 of a matrix.
- `Matrix * mlogbM(Matrix * m, void * b, int type)` - log base `*b` of a matrix., where `int type` is the numerical type (INTEGER, DOUBLE, or COMPLEX) of `*b`.


## Testing

Of course, we can't just assume that all the code works. Probably not all of it does. So, we have tests. The tests are divided into two directories:

### tests_C_Octave

Tests in `tests_C_Octave/` are meant to verify output accuracy between various C functions and their equivalent programs in Octave.

N.B.: In each of these tests, the output of the C programs and the Octave outputs should be exactly the same. These tests, therefore, also serve as examples of how to write Octave code using RISCV-MATRIX and demonstrate the usage of the different RISCV-MATRIX functions.

N.B.: These test cases provide basic, quick checks for consistency between C and Octave. They are not exhaustive in covering all edge cases.

For the sake of runtime efficiency, the Octave "reference" outputs of each program are stored in `tests_C_Octave/outputs/zoo/` (so that way the Octave functions need not be run every time to compare the outputs). When running one of the C test programs, I recommend redirecting the output to the appropriately-named file in `tests_C_Octave/outputs/grace/grace_TEST_output.txt`. 

#### tests_C_Octave Makefile command reference

The `tests_C_Octave/Makefile` contains many recipes useful for executing the various test cases.

|Command|Behavior|Output|
|:-|:-|:-|
|`make -s`|Compiles all C tests|Reports the status of each test as it is compiled.|
|`make -s TEST`|Compiles the specific test TEST (where TEST is of the form `abs_matrix/abs_matrix`)|Reports the status of the test as it is compiled.|
|`make -s matrix`|Recompiles the Matrix Library|Outputs that it is compiling the Matrix Library|
|`make -s runC`|Runs all the C test cases|The outputs of each test are printed to their corresponding `outputs/grace/grace_TEST_output.txt` file.|
|`make -s runOctave`|Runs all the Octave test cases|The outputs of each test are printed to their corresponding `outputs/zoo/zoo_TEST_output.txt` file.|
|`make -s compare_outputs`|diffs each test output in `outputs/grace/grace_TEST_output.txt` with the corresponding `outputs/zoo/zoo_TEST_output.txt`|The diff results are printed to STDOUT.|
|`make -s timeit`|Runs all the C & Octave test cases and times the output of each|Nothing is printed to STDOUT; the runtimes of each test are reported in `tests_C_Octave/speed_result.txt`|

### tests_Vec_C

In order to verify that "vectorized" C functions (i.e., those using RVV Assembly instructions) accurately mimic the behavior of their equivalent "reference" (non-vectorized) C functions, and also to measure performance differences, the tests in `tests_Vec_C` need to be able to explicitly call the vectorized and reference versions of any function, and so for any function that has been vectorized, `foo(...)`, `foo_vec(...)`, and `foo_ref(...)` are directly callable, regardless of `MODE`.

Each test in `tests_Vec_C/` is in its own directory. Each directory contains the C program of the test, as well as its accuracy & speed reports.

The `tests_Vec_C/common/` directory contains a `utils.c` file that runs the code within each test file and formats the output in a consistent way. The advantage of this approach is that we can run all the tests on a specific, united set of matrix sizes and have a common output format.

Nicely formatted versions of the speed results of each test can be found in the `tests_Vec_C/time_results/` directory.

#### Speed & Accuracy Reports

When each test case is run, it is run on several matrices of a variety of sizes (defined in `common/utils.c`). Each test case also runs all combinations of input data types.

#### tests_Vec_C Makefile command reference

The `tests_Vec_C/Makefile` contains many recipes useful for executing the various test cases.

|Command|Behavior    |Output               |
|:-------|:------|:------------------------|
|`make -s`|Recompiles the matrix library, `common/utils.o`, and time-runs test program.|Reports its progress to STDOUT.|
|`make -s TEST/TEST`|Compiles the specific test TEST (where TEST is of the form `abs_vec_test`)|Reports the status of the test as it is compiled.|
|`make -s TEST`|Compiles and time-runs the specific test TEST (where TEST is of the form `abs_vec_test`)|Reports the status of the test as it is compiled. The speed report is saved to `TEST/TEST_speed_output.txt`|
|`make -s compile`|Recompiles the matrix library, `common/utils.o`, and each test program.|Reports its progress to STDOUT.|
|`make -s check`|Runs each test case, specifically checking for that the `_vec` and `_ref` versions of a particular function have the same results.|Reports its progress to STDOUT. A full accuracy report is saved to `TEST/TEST_accuracy_output.txt`|
|`make -s run` or `make -s time`|Runs each test case, timing the `_vec` and `_ref` versions of a particular functions.|Reports its progress to STDOUT. A full speed report is saved to `TEST/TEST_speed_output.txt`. Each speed report is nicely formatted and saved to `time_results/TEST.txt`|
|`make -s matrix`|Recompiles the Matrix Library|Outputs that it is compiling the Matrix Library|
|`make -s common/utils.o`|Recompiles the utilities module|Outputs that it is compiling the utilities module.|

## Preliminary Results

### C vs Octave

Below is a table comparing the runtimes (in seconds) of 5 runs each of the C and Octave tests in `tests_C_Octave`.

N.B.: When these results were collected, the only way of measuring Octave runtime was using the built-in Octave `tic` and `toc` commands (see `tests_C_Octave/octaveTimer.m`), which measures runtime in seconds instead of cycles. A more thorough analysis would incorporate cycles counts for Octave programs as well.

N.B.: These tests compare Reference versions of the C functions with their Octave equivalents. At the time this data was collected, no functions had yet been vectorized.

N.B.: These tests are not exhaustive and do not compare a wide range of size inputs or a variety of input datatypes. Take these numbers only as rough estimates.


|Test|Octave Runtime (seconds)|C Runtime (seconds)|Runtime Ratio (C/Octave)|
|:-|-:|-:|-:|
|3d_index_matrix|0.03656912|0.016|0.437527619|
|4d_index_matrix|0.07201982|0.018|0.249931199|
|abs_matrix|0.01387095|0.017|1.225582963|
|add_matrix|0.02108121|0.017|0.806405325|
|advanced_stats|298.880326|4.23|0.014152822|
|areEqual_matrix|0.02558517|0.017|0.664447412|
|basic_stats|0.65158081|0.071|0.108965763|
|cc_matrix_multiplication|0.01452899|0.017|1.170074451|
|cd_matrix_multiplication|0.01404309|0.015|1.068140986|
|ceil_matrix|0.01469111|0.018|1.225230769|
|ci_matrix_multiplication|0.01331091|0.017|1.277147843|
|comparison_ops_matrix|1.50591779|0.038|0.025233781|
|create_1D_matrix|0.01094103|0.016|1.462385168|
|create_2D_matrix|0.01243687|0.016|1.286497326|
|create_fails_negative_dims|0.00920415|0.015|1.629699646|
|create_identity_matrix|0.01806211|0.015|0.830467758|
|create_one_matrix|0.01231408|0.016|1.29932565|
|create_zero_matrix|0.01319718|0.015|1.136606457|
|dc_matrix_multiplication|0.01344681|0.018|1.338607447|
|dd_matrix_multiplication|0.01188922|0.017|1.42986672|
|determinant_matrix|0.02224517|0.018|0.809164416|
|di_matrix_multiplication|0.01247501|0.017|1.362724359|
|duplicate_matrix|0.01740384|0.016|0.919337342|
|eigen|0.13018799|0.019|0.145942802|
|elem_divide_matrix|0.03045821|0.016|0.525309925|
|elem_multiply_matrix|0.02117419|0.016|0.755636933|
|elem_power_matrix|0.07511282|0.018|0.239639518|
|elem_scalarpower_matrix|0.08502388|0.018|0.211705229|
|elem_trig_matrix|0.0472641|0.017|0.359681026|
|equal_matrix|0.01652908|0.015|0.907491524|
|filterM|29.07367396|0.496|0.017060107|
|floor_matrix|0.01497507|0.018|1.20199772|
|fourier|0.96469402|0.057|0.059086092|
|hamming_hanning|3.77220583|0.056|0.014845425|
|ic_matrix_multiplication|0.01430702|0.016|1.118332119|
|id_matrix_multiplication|0.01300001|0.018|1.38461432|
|ii_matrix_multiplication|0.01193905|0.016|1.340140128|
|index_matrix|0.01717591|0.017|0.98975833|
|invert_matrix|0.02106905|0.016|0.759407757|
|isEqual_matrix|0.01473808|0.016|1.085623093|
|logic_ops_matrix|0.02216601|0.017|0.766940013|
|matrix_division|0.03681111|0.017|0.461817098|
|power_matrix|28.58414912|1.067|0.037328381|
|randomness|0.58481288|0.017|0.029069127|
|reindex|0.15214396|0.02|0.131454446|
|reshape_matrix|0.01803398|0.016|0.887214026|
|round_matrix|0.01628995|0.017|1.043588225|
|scale_matrix|0.03513503|0.016|0.455385978|
|SPEED_big_matrix_addition|47.8581109|0.207|0.004325286|
|SPEED_big_matrix_eigen|77.30357909|17.859|0.231024232|
|SPEED_big_matrix_fourier|70.51402688|0.362|0.00513373|
|SPEED_big_matrix_inverse|71.02640319|2.711|0.038168904|
|SPEED_big_matrix_multiplication|48.14831805|10.814|0.224597669|
|SPEED_big_matrix_power_complex|28.77352095|36.165|1.256884761|
|SPEED_big_matrix_power_double|28.46034002|36.158|1.270469712|
|SPEED_big_matrix_power_int|20.95127106|12.962|0.618673682|
|SPEED_big_matrix_transpose|70.68946505|0.247|0.003494156|
|SPEED_medium_matrix_addition|0.49497485|0.061|0.123238585|
|SPEED_medium_matrix_eigen|3.87975693|4.43|1.141824109|
|SPEED_medium_matrix_fourier|0.75737691|0.097|0.128073617|
|SPEED_medium_matrix_inverse|1.82603002|0.33|0.18071992|
|SPEED_medium_matrix_multiplication|0.51294112|1.134|2.21078006|
|SPEED_medium_matrix_power_complex|5.41068292|6.503|1.201881555|
|SPEED_medium_matrix_power_double|5.28678203|6.508|1.230994575|
|SPEED_medium_matrix_power_int|0.28555298|1.471|5.151408331|
|SPEED_medium_matrix_transpose|0.72364402|0.044|0.060803377|
|SPEED_small_matrix_addition|10.48867297|1.312|0.125087321|
|SPEED_small_matrix_eigen|120.2999821|25.547|0.212360796|
|SPEED_small_matrix_fourier|69.93401885|35.531|0.50806461|
|SPEED_small_matrix_inverse|87.31383705|5.605|0.06419372|
|SPEED_small_matrix_multiplication|14.52929997|2.043|0.140612418|
|SPEED_small_matrix_power_complex|9.14674997|4.096|0.447809333|
|SPEED_small_matrix_power_double|8.1980319|4.115|0.501949742|
|SPEED_small_matrix_power_int|3.29784489|1.348|0.408751789|
|SPEED_small_matrix_transpose|9.07280111|1.194|0.131602135|
|stftM|5.43222594|0.213|0.039210446|
|subtract_matrix|0.02157187|0.018|0.834420011|
|transpose_matrix|0.05220509|0.018|0.344793965|
|xcorrM|56.26662898|0.692|0.012298586|
|||||
|Total|1242.146858|226.497|0.182343173|


Taking these numbers as a very rough estimate, we can estimate that Reference C code will give us about an 81% decrease in runtime.

### RVV C vs Reference C

Below is a table comparing runtimes of the Vectorized C and Reference C code, for a variety of input datatypes and matrix sizes, for a couple select functions.

#### Elementwise Absolute Value: absM


|Input Matrix Size|INTEGER input, vectorized function (cycles)|INTEGER input, reference function (cycles)|INTEGER input, runtime ratio (vec/ref)|DOUBLE input, vectorized function (cycles)|DOUBLE input, reference function (cycles)|DOUBLE input, runtime ratio (vec/ref)|COMPLEX input, vectorized function (cycles)|COMPLEX input, reference function (cycles)|COMPLEX input, runtime ratio (vec/ref)|
|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|
|1|400|5516|0.07252|376|506|0.74308|394|1400|0.28143|
|2|540|392|1.37755|486|522|0.93103|504|2318|0.21743|
|3|540|400|1.35000|494|522|0.94636|512|3211|0.15945|
|4|542|408|1.32843|464|524|0.88550|482|4049|0.11904|
|6|599|468|1.27991|379|577|0.65685|397|5904|0.06724|
|9|2748|447|6.14765|438|475|0.92211|456|8414|0.05420|
|16129|31956|158062|0.20217|62525|156033|0.40072|63929|14057670|0.00455|
|16256|31956|159078|0.20088|62525|156795|0.39877|63929|14166349|0.00451|
|16383|32004|161970|0.19759|62586|159390|0.39266|64001|14277347|0.00448|
|16384|32004|161978|0.19758|62586|159396|0.39264|64001|14278157|0.00448|
|16512|33849|163002|0.20766|62592|161999|0.38637|64018|14390251|0.00445|
|16641|33866|164034|0.20646|62618|162773|0.38470|64066|14501057|0.00442|
|1046529|1987685|10290394|0.19316|3941735|10114821|0.38970|4031689|910323562|0.00443|
|1047552|1989612|10300454|0.19316|3945578|10124711|0.38970|4035609|911202657|0.00443|
|1048575|1991556|10310514|0.19316|3949434|10134601|0.38970|4039553|912084927|0.00443|
|1048576|1991556|10310522|0.19316|3949434|10134607|0.38970|4039553|912085737|0.00443|
|1049600|1993500|10320590|0.19316|3953290|10144503|0.38970|4043497|912970253|0.00443|
|1050625|1995461|10330666|0.19316|3957159|10154405|0.38970|4047465|913855729|0.00443|


From these results we can see that for absolute value, in the limit as the size grows, the vectorized code runs in 20% the cycles compared to the reference code for INTEGER matrices, 39% for DOUBLE matrices, and 0.4% for COMPLEX matrices.


#### Elementwise Addition: plusM


|Input Matrices Size|INTEGER/INTEGER input, vectorized function (cycles)|INTEGER/INTEGER input, reference function (cycles)|INTEGER/INTEGER input, runtime ratio (vec/ref)|INTEGER/DOUBLE input, vectorized function (cycles)|INTEGER/DOUBLE input, reference function (cycles)|INTEGER/DOUBLE input, runtime ratio (vec/ref)|INTEGER/COMPLEX input, vectorized function (cycles)|INTEGER/COMPLEX input, reference function (cycles)|INTEGER/COMPLEX input, runtime ratio (vec/ref)|DOUBLE/INTEGER input, vectorized function (cycles)|DOUBLE/INTEGER input, reference function (cycles)|DOUBLE/INTEGER input, runtime ratio (vec/ref)|DOUBLE/DOUBLE input, vectorized function (cycles)|DOUBLE/DOUBLE input, reference function (cycles)|DOUBLE/DOUBLE input, runtime ratio (vec/ref)|DOUBLE/COMPLEX input, vectorized function (cycles)|DOUBLE/COMPLEX input, reference function (cycles)|DOUBLE/COMPLEX input, runtime ratio (vec/ref)|COMPLEX/INTEGER input, vectorized function (cycles)|COMPLEX/INTEGER input, reference function (cycles)|COMPLEX/INTEGER input, runtime ratio (vec/ref)|COMPLEX/DOUBLE input, vectorized function (cycles)|COMPLEX/DOUBLE input, reference function (cycles)|COMPLEX/DOUBLE input, runtime ratio (vec/ref)|COMPLEX/COMPLEX input, vectorized function (cycles)|COMPLEX/COMPLEX input, reference function (cycles)|COMPLEX/COMPLEX input, runtime ratio (vec/ref)|
|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|
|1|996|5594|0.17805|465|579|0.80311|478|586|0.81570|465|579|0.80311|453|574|0.78920|473|582|0.81271|478|586|0.81570|473|582|0.81271|459|583|0.78731|
|2|1318|477|2.76310|575|584|0.98459|598|602|0.99336|575|584|0.98459|563|578|0.97405|593|597|0.99330|598|602|0.99336|593|597|0.99330|579|599|0.96661|
|3|1214|492|2.46748|581|597|0.97320|599|606|0.98845|540|597|0.90452|569|590|0.96441|594|600|0.99000|555|606|0.91584|550|600|0.91667|536|603|0.88889|
|4|1025|727|1.40990|527|560|0.94107|540|571|0.94571|527|603|0.87396|515|552|0.93297|535|564|0.94858|540|614|0.87948|535|607|0.88138|521|611|0.85270|
|6|1047|595|1.75966|483|647|0.74652|496|673|0.73700|483|660|0.73182|471|641|0.73479|491|664|0.73946|496|682|0.72727|491|673|0.72957|477|679|0.70250|
|9|1068|686|1.55685|483|678|0.71239|496|713|0.69565|483|691|0.69899|471|669|0.70404|491|701|0.70043|496|722|0.68698|491|710|0.69155|477|719|0.66342|
|16129|209043|272883|0.76605|61547|206337|0.29828|125321|312884|0.40054|61547|206385|0.29821|61157|190204|0.32153|122478|296752|0.41273|122735|312929|0.39221|122478|296797|0.41267|123220|312926|0.39377|
|16256|209932|274788|0.76398|61547|207480|0.29664|124689|314408|0.39658|61547|207528|0.29657|61157|191220|0.31983|124306|298149|0.41693|124563|314453|0.39613|124306|298194|0.41686|125064|314450|0.39772|
|16383|212860|276734|0.76919|63449|208621|0.30414|124765|317808|0.39258|63449|208667|0.30407|63056|192234|0.32802|124380|301422|0.41264|124639|317852|0.39213|124380|301466|0.41258|125144|317849|0.39372|
|16384|212867|276749|0.76917|63449|208630|0.30412|124639|317820|0.39217|63449|208676|0.30406|63056|192242|0.32800|124380|301433|0.41263|124639|317864|0.39211|124380|301477|0.41257|125144|317861|0.39371|
|16512|217471|278669|0.78039|63461|211617|0.29989|124745|321191|0.38838|63461|211663|0.29982|63065|195101|0.32324|124399|304676|0.40830|124660|321235|0.38806|124399|304720|0.40824|125169|321232|0.38965|
|16641|218432|280604|0.77844|63499|212778|0.29843|126725|322780|0.39260|63499|212824|0.29836|63097|196133|0.32171|126334|306136|0.41267|126599|322824|0.39216|126334|306180|0.41261|127100|322821|0.39372|
|1046529|13421691|17616180|0.76190|3990892|13254490|0.30110|8079684|20228625|0.39942|3990892|13254490|0.30110|3966352|12207957|0.32490|7882676|19182093|0.41094|7899033|20228625|0.39049|7882676|19182093|0.41094|7931718|20228622|0.39210|
|1047552|13434777|17633401|0.76189|3994777|13267449|0.30110|7907638|20248405|0.39053|3994777|13267449|0.30110|3970216|12219893|0.32490|7890362|19200850|0.41094|7906733|20248405|0.39049|7890362|19200850|0.41094|7939462|20248402|0.39210|
|1048575|13447900|17650622|0.76189|3998681|13280408|0.30110|7915366|20268185|0.39053|3998681|13280408|0.30110|3974096|12231829|0.32490|7898074|19219607|0.41094|7914461|20268185|0.39049|7898074|19219607|0.41094|7947222|20268182|0.39210|
|1048576|13447907|17650637|0.76189|3998681|13280417|0.30110|7914461|20268197|0.39049|3998681|13280417|0.30110|3974096|12231837|0.32490|7898074|19219618|0.41094|7914461|20268197|0.39049|7898074|19219618|0.41094|7947222|20268194|0.39210|
|1049600|13461037|17667873|0.76189|4002585|13293385|0.30110|7923094|20287989|0.39053|4002585|13293385|0.30110|3977976|12243781|0.32490|7905786|19238386|0.41094|7922189|20287989|0.39049|7905786|19238386|0.41094|7954982|20287986|0.39210|
|1050625|13474211|17685124|0.76190|4006508|13306362|0.30110|7930850|20307793|0.39053|4006508|13306362|0.30110|3981872|12255733|0.32490|7913524|19257165|0.41094|7929945|20307793|0.39049|7913524|19257165|0.41094|7962758|20307790|0.39210|


From these results we can see that, in the limit as the size grows, for situations where the one of the inputs is a DOUBLE or COMPLEX type, we see that the vectorized code for matrix addition runs in around 35-45% of the cycles needed to run the reference code.

To factor in this with data from our C/Octave comparison, the tests `tests_C_Octave/SPEED_small_matrix_addition/`, `tests_C_Octave/SPEED_medium_matrix_addition/`, and `tests_C_Octave/SPEED_big_matrix_addition/` are run on COMPLEX/DOUBLE matrices of size 9, 10,000, and 1,000,000, and their C/Octave runtime ratios are 0.125087321, 0.123238585, and 0.004325, respectively.

Thus, we can say that for addition where the first matrix is of type COMPLEX and the second matrix is of type DOUBLE, for matrices of sizes 4, 10,000, and 1,000,000, **we would expect that RISCV-MATRIX's RVV-vectorized implementation of plusM should operate in 11.02%, 5.09%, and 0.18% the runtime of Octave**.
While the scope of this result is rather limited, as a preliminary, proxy result this is very exciting! In this small case, we see that RISCV-MATRIX performs ***over 500x*** faster than Octave.

#### Matrix Multiplication: mtimesM

Matrix multiplication is a particularly interesting function to examine. We assume that using BLAS functions for various matrix operations will be more efficient than naive implementations of the same algorithms.

However, it is more difficult to vectorize BLAS functions than naive ones, because doing so would require vectorizing nearly all of BLAS (or, at least, BLAS level 1), since its routines are interwoven, especially for level 3 functions like matrix multiplication.

Thus, below we compare three different implementations of matrix multiplication: a naive implementation, a vectorization of the naive implementation, and the BLAS implementation.

The first table shows the runtimes in cycles of the naive implementation, vectorized naive implemenation, and the BLAS implementation. 


|Input Matrix Size|INTEGER/INTEGER input, vectorized implementation (cycles)|INTEGER/INTEGER input, naive implementation (cycles)|INTEGER/INTEGER input, BLAS implementation (cycles)|INTEGER/DOUBLE input, vectorized implementation (cycles)|INTEGER/DOUBLE input, naive implementation (cycles)|INTEGER/DOUBLE input, BLAS implementation (cycles)|INTEGER/COMPLEX input, vectorized implementation (cycles)|INTEGER/COMPLEX input, naive implementation (cycles)|INTEGER/COMPLEX input, BLAS implementation (cycles)|DOUBLE/INTEGER input, vectorized implementation (cycles)|DOUBLE/INTEGER input, naive implementation (cycles)|DOUBLE/INTEGER input, BLAS implementation (cycles)|DOUBLE/DOUBLE input, vectorized implementation (cycles)|DOUBLE/DOUBLE input, naive implementation (cycles)|DOUBLE/DOUBLE input, BLAS implementation (cycles)|DOUBLE/COMPLEX input, vectorized implementation (cycles)|DOUBLE/COMPLEX input, naive implementation (cycles)|DOUBLE/COMPLEX input, BLAS implementation (cycles)|COMPLEX/INTEGER input, vectorized implementation (cycles)|COMPLEX/INTEGER input, naive implementation (cycles)|COMPLEX/INTEGER input, BLAS implementation (cycles)|COMPLEX/DOUBLE input, vectorized implementation (cycles)|COMPLEX/DOUBLE input, naive implementation (cycles)|COMPLEX/DOUBLE input, BLAS implementation (cycles)|COMPLEX/COMPLEX input, vectorized implementation (cycles)|COMPLEX/COMPLEX input, naive implementation (cycles)|COMPLEX/COMPLEX input, BLAS implementation (cycles)|
|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|
|1|841|5910|26762|622|810|1023|657|836|11330|623|815|1024|460|654|749|658|836|1079|661|841|1082|661|841|1082|490|683|799|
|2|942|808|1131|626|815|1026|669|862|1102|627|820|1027|460|653|885|670|861|1103|673|867|1106|673|866|1106|490|694|948|
|3|969|843|1178|725|784|949|760|836|1030|726|789|950|566|608|791|761|834|1031|764|841|1034|764|839|1034|596|661|867|
|4|972|1052|1302|784|866|1073|927|940|1182|785|923|1074|625|695|915|928|937|1183|931|996|1186|931|993|1186|763|820|1019|
|6|974|1037|1350|740|979|1119|837|1150|1270|741|992|1120|581|796|961|838|1145|1271|841|1163|1274|841|1158|1274|673|973|1107|
|9|1134|1193|1595|900|1169|1364|1099|1535|1593|901|1195|1365|741|968|1206|1100|1527|1594|1103|1561|1597|1103|1553|1597|935|1350|1430|
|961|34517|242710|227106|29964|224338|222556|53631|584739|472075|29965|224391|222557|29705|218423|222298|49308|583779|466448|49325|584792|466465|49311|583832|466451|48998|577912|466139|
|1024|35823|264254|248168|31864|246092|243705|56759|642451|519728|31865|246145|243706|31605|239799|243447|52470|641428|512273|52487|642504|512290|52473|641481|512276|52160|635183|511964|
|1089|38363|287302|273512|33838|269306|267155|60043|703848|568491|33839|269359|267156|33565|262623|266883|55752|702760|561034|55771|703901|561053|55755|702813|561037|55422|696125|560705|
|16129|672340|14867383|14715187|608846|14704507|14649819|1473193|39453622|31268099|608847|14704512|14649820|544284|14546833|14587144|1471668|39437494|31265262|1471923|39453627|31265517|1471671|39437499|31265265|1346521|39218107|31140116|
|16256|672851|14977828|14827969|608795|14818223|14763916|1471919|39760962|31509393|608796|14818228|14763917|546119|14657901|14699362|1471668|39744707|31508975|1471923|39760967|31509230|1471671|39744712|31508978|1346521|39524558|31383829|
|16383|898918|15094437|14944010|834757|14933764|14879852|1893222|40070229|31755055|834758|14933769|14879853|770090|14772639|14815236|1892969|40053847|31754635|1893226|40070234|31754892|1892972|40053852|31754638|1765875|39831009|31627542|
|16384|682605|15212426|15058379|618487|15051702|14994264|1495136|40388641|32005625|618488|15051707|14994265|553827|14890521|14929605|1494883|40372258|32005246|1495140|40388646|32005503|1494886|40372263|32005249|1367789|40149414|31878153|
|16512|912009|15328564|15174247|847877|15167158|15110118|1921142|40700833|32253240|847878|15167163|15110119|783203|15005209|15045445|1920887|40684322|32252818|1921146|40700838|32253077|1920890|40684327|32252821|1793773|40460710|32125705|
|16641|925219|15568174|15413600|861059|15404115|15347607|1951215|41338289|32757375|861060|15404120|15347608|794514|15241442|15282949|1950956|41321649|32756826|1951219|41338294|32757089|1950959|41321654|32756829|1821875|41095336|32627746|


The second table shows the ratios in the runtimes between the naive implementation, vectorized naive implemenation, and the BLAS implementation. (Calculated from above table but displayed separately for better viewing).


|Input Matrix Size|INTEGER/INTEGER input, runtime ratio (ref/BLAS)|INTEGER/INTEGER input, runtime ratio (vec/ref)|INTEGER/INTEGER input, runtime ratio (vec/BLAS)|INTEGER/DOUBLE input, runtime ratio (ref/BLAS)|INTEGER/DOUBLE input, runtime ratio (vec/ref)|INTEGER/DOUBLE input, runtime ratio (vec/BLAS)|INTEGER/COMPLEX input, runtime ratio (ref/BLAS)|INTEGER/COMPLEX input, runtime ratio (vec/ref)|INTEGER/COMPLEX input, runtime ratio (vec/BLAS)|DOUBLE/INTEGER input, runtime ratio (ref/BLAS)|DOUBLE/INTEGER input, runtime ratio (vec/ref)|DOUBLE/INTEGER input, runtime ratio (vec/BLAS)|DOUBLE/DOUBLE input, runtime ratio (ref/BLAS)|DOUBLE/DOUBLE input, runtime ratio (vec/ref)|DOUBLE/DOUBLE input, runtime ratio (vec/BLAS)|DOUBLE/COMPLEX input, runtime ratio (ref/BLAS)|DOUBLE/COMPLEX input, runtime ratio (vec/ref)|DOUBLE/COMPLEX input, runtime ratio (vec/BLAS)|COMPLEX/INTEGER input, runtime ratio (ref/BLAS)|COMPLEX/INTEGER input, runtime ratio (vec/ref)|COMPLEX/INTEGER input, runtime ratio (vec/BLAS)|COMPLEX/DOUBLE input, runtime ratio (ref/BLAS)|COMPLEX/DOUBLE input, runtime ratio (vec/ref)|COMPLEX/DOUBLE input, runtime ratio (vec/BLAS)|COMPLEX/COMPLEX input, runtime ratio (ref/BLAS)|COMPLEX/COMPLEX input, runtime ratio (vec/ref)|COMPLEX/COMPLEX input, runtime ratio (vec/BLAS)|
|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|-:|
|1|0.22084|0.14230|0.03143|0.79179|0.76790|0.60802|0.07379|0.78589|0.05799|0.79590|0.76442|0.60840|0.87316|0.70336|0.61415|0.77479|0.78708|0.60982|0.77726|0.78597|0.61091|0.77726|0.78597|0.61091|0.85482|0.71742|0.61327|
|2|0.71441|1.16584|0.83289|0.79435|0.76810|0.61014|0.78221|0.77610|0.60708|0.79844|0.76463|0.61052|0.73785|0.70444|0.51977|0.78060|0.77816|0.60743|0.78391|0.77624|0.60850|0.78300|0.77714|0.60850|0.73207|0.70605|0.51688|
|3|0.71562|1.14947|0.82258|0.82613|0.92474|0.76396|0.81165|0.90909|0.73786|0.83053|0.92015|0.76421|0.76865|0.93092|0.71555|0.80892|0.91247|0.73812|0.81335|0.90844|0.73888|0.81141|0.91061|0.73888|0.76240|0.90166|0.68743|
|4|0.80799|0.92395|0.74654|0.80708|0.90531|0.73066|0.79526|0.98617|0.78426|0.85940|0.85049|0.73091|0.75956|0.89928|0.68306|0.79205|0.99039|0.78445|0.83980|0.93474|0.78499|0.83727|0.93756|0.78499|0.80471|0.93049|0.74877|
|6|0.76815|0.93925|0.72148|0.87489|0.75587|0.66130|0.90551|0.72783|0.65906|0.88571|0.74698|0.66161|0.82830|0.72990|0.60458|0.90087|0.73188|0.65932|0.91287|0.72313|0.66013|0.90895|0.72625|0.66013|0.87895|0.69168|0.60795|
|9|0.74796|0.95054|0.71097|0.85704|0.76989|0.65982|0.96359|0.71596|0.68989|0.87546|0.75397|0.66007|0.80265|0.76550|0.61443|0.95797|0.72037|0.69009|0.97746|0.70660|0.69067|0.97245|0.71024|0.69067|0.94406|0.69259|0.65385|
|961|1.06871|0.14221|0.15199|1.00801|0.13357|0.13464|1.23866|0.09172|0.11361|1.00824|0.13354|0.13464|0.98257|0.13600|0.13363|1.25154|0.08446|0.10571|1.25367|0.08435|0.10574|1.25165|0.08446|0.10572|1.23978|0.08478|0.10511|
|1024|1.06482|0.13556|0.14435|1.00979|0.12948|0.13075|1.23613|0.08835|0.10921|1.01001|0.12946|0.13075|0.98502|0.13180|0.12982|1.25212|0.08180|0.10243|1.25418|0.08169|0.10246|1.25222|0.08180|0.10243|1.24068|0.08212|0.10188|
|1089|1.05042|0.13353|0.14026|1.00805|0.12565|0.12666|1.23810|0.08531|0.10562|1.00825|0.12563|0.12666|0.98404|0.12781|0.12577|1.25262|0.07933|0.09937|1.25461|0.07923|0.09940|1.25270|0.07933|0.09938|1.24152|0.07962|0.09884|
|16129|1.01034|0.04522|0.04569|1.00373|0.04141|0.04156|1.26179|0.03734|0.04711|1.00373|0.04141|0.04156|0.99724|0.03742|0.03731|1.26138|0.03732|0.04707|1.26189|0.03731|0.04708|1.26138|0.03732|0.04707|1.25941|0.03433|0.04324|
|16256|1.01011|0.04492|0.04538|1.00368|0.04108|0.04124|1.26188|0.03702|0.04671|1.00368|0.04108|0.04124|0.99718|0.03726|0.03715|1.26138|0.03703|0.04671|1.26188|0.03702|0.04671|1.26138|0.03703|0.04671|1.25939|0.03407|0.04290|
|16383|1.01007|0.05955|0.06015|1.00362|0.05590|0.05610|1.26185|0.04725|0.05962|1.00362|0.05590|0.05610|0.99712|0.05213|0.05198|1.26135|0.04726|0.05961|1.26186|0.04725|0.05962|1.26135|0.04726|0.05961|1.25938|0.04433|0.05583|
|16384|1.01023|0.04487|0.04533|1.00383|0.04109|0.04125|1.26192|0.03702|0.04671|1.00383|0.04109|0.04125|0.99738|0.03719|0.03710|1.26143|0.03703|0.04671|1.26193|0.03702|0.04672|1.26143|0.03703|0.04671|1.25946|0.03407|0.04291|
|16512|1.01017|0.05950|0.06010|1.00377|0.05590|0.05611|1.26191|0.04720|0.05956|1.00378|0.05590|0.05611|0.99733|0.05220|0.05206|1.26142|0.04721|0.05956|1.26192|0.04720|0.05956|1.26142|0.04721|0.05956|1.25945|0.04433|0.05584|
|16641|1.01003|0.05943|0.06003|1.00368|0.05590|0.05610|1.26195|0.04720|0.05957|1.00368|0.05590|0.05610|0.99728|0.05213|0.05199|1.26147|0.04721|0.05956|1.26196|0.04720|0.05957|1.26147|0.04721|0.05956|1.25952|0.04433|0.05584|


From this data, we notice a few interesting things:
- When there are no complex inputs, the naive implementation and the BLAS implementation perform almost identically.
- When there are complex inputs, the naive implementation takes around 26% longer to run than the BLAS implementation.
- Regardless of input type, the vectorized implementation runs in about 5-6% the time of the BLAS implementation. Since our VLEN is set to 1024, and the SEW of doubles is 64, we're typically operating on 16 doubles (or 8 complex numbers) at a time (integers are converted to doubles before going through matrix multiplication). Thus, it makes sense to see that the vectorized implementation runs in about 1/16th the time of the BLAS implementation.

## Future Directions

Here is a non-exhaustive list of next steps and TODOs for continuing development of RISCV-MATRIX, in no particular order:
- LMUL settings need to be double-checked for the vectorizations of the following functions, as they may be too high:
  - `writeM`
  - All `X_to_Y` casting functions in `src/utils.c`
  - All functions in `src/elementwise_comparison.c`
  - `floorM`
  - `ceilM`
  - `roundM`
  - `timesM`
  - `rdivideM`
- `src/elementwise_comparison.c` should be refactored.
  - The vectorized versions of `geM`, `leM`, and `neM` currently only do exact equality checking for doubles & complexes. They should instead compare the magnitude of the difference between values to `EPSILON`. (See how it's done in `equalM`).
  - This process may be greatly accelerated by writing vectorized versions of the `almostEqual_XY` functions in `src/utils.c`.
  - `pairwise_maxM_internal_ref` and `pairwise_minM_internal_ref` are very slow. They can be improved by directly doing max/min comparisons instead of their current circuitous method, which was originally implemented to avoid having to branch behavior on `COMPARISON_MODE`, but I now realize that is significantly slower.
- `COMPARISON_MODE`, `MODE`, and `EPSILON` could be made to be runtime-configurable.
- It would be good to analyze the lower bound for `EPSILON` for different functions to get an idea of different functions' precisions.
- RISCV-MATRIX has many tests in place, but more is always better. 
  - Some of the `tests_C_Octave` tests, for instance, only test a single case.
  - Particularly, edge cases involving &plusmn;infinity, &plusmn;0, and NaN. (Both in `tests_C_Octave` and `tests_Vec_C`).
  - RISCV-MATRIX would benefit from code coverage analysis, possibly with the [gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) tool.
- Memory leak checking for all vectorized functions.
- As more functions are vectorized &mdash; and the accuracy of the vectorized versions is more thoroughly tested &mdash; the default mode of different functions should be changed to whichever mode (`_vec` or `_ref`) gives better performance, possibly taking into account input types and size.
- Some elementwise and matrix operations may be further optimized by calling existing BLAS/LAPACKE functions.
- Vectorize remaining RISCV-MATRIX.
- Vectorize BLAS & LAPACKE functions.
- Vectorize fftw library.
- Find some way to count execution cycles of Octave programs in order to get more accurate performance results.
- Down the line, it would be good to consider re-writing RISCV-MATRIX in C++ (and still using RVV Assembly) instead of C. The template class feature of C++, combined with polymorphism, could greatly shrink the code base while maintaining performance, as code for different data types may not have to be duplicated.

## Known Bugs

- For some reason, valgrind and spike don't work nicely together. Even when a basic program like `main.c` &mdash; which allocates no memory usage &mdash; is compiled & run with spike, valgrind will detect memory leaks. This is likely due to a bug within the spike runner, but I haven't investigated further. For this reason, though, **No memory leak testing has been done on any of the vectorized functions.**
- `powerM` has rounding errors comparing between the Reference C version and Octave, these rounding errors are quite a bit larger than `EPSILON`.
- `mpowerM` has unexpected behavior when trying to raise defective (i.e., non-diagonalizable) matrices to non-integer powers.
- For large complex matrices, the vectorized naive implementation `naive_mtimesM_vec` will have small rounding errors compared to the BLAS implementation `mtimesM` and the naive reference implementation `naive_mtimesM_ref`, even though the latter two give exactly equal results (or, at least, to the precision of the smaller of `EPSILON` and 20 decimal places, which is how many decimal places I output when doing accuracy tests). See `tests_Vec_C/mtimes_vec_test/mtimes_vec_test_accuracy_output.txt`.
- Some operations (both vectorized and reference ones) will take more cycles (sometimes significantly more cycles) for a matrix of size 1 than of size 2. This predominantly happens when the input matrix or one of the input matrices is an INTEGER matrix, but not always. I have no good explanation for this.

## Acknowledgements

I would like to give special thanks so many people who have contributed towards this project's success and continuing growth!
- Rajit Manohar, for bringing me on to this project and being a wonderful teacher!
- Xiayuan Wen, for helping me every step of the way, answering my many questions, and guiding me.
- The Yale School of Engineering & Applied Sciences, without which none of this would have been possible.
- David McCowin, for jumping at the chance to help me out with some math.
- Katie Handler, for unwavering moral support.

## Sources

[^1]: I. Karageorgos *et al.*, "Hardware-Software Co-Design for Brain-Computer Interfaces," *2020 ACM/IEEE 47th Annual International Symposium on Computer Architecture (ISCA)*, 2020, pp. 391-404, doi: 10.1109/ISCA45697.2020.00041.
[^2]: https://github.com/riscv/riscv-v-spec/blob/master/v-spec.adoc
[^3]: https://github.com/riscv-non-isa/rvv-intrinsic-doc/blob/master/rvv-intrinsic-api.md