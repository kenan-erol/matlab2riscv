- [ts-traversal](#ts-traversal)
  * [Built with](#built-with)
  * [Loading modules](#loading-modules)
  * [Usage](#usage)
  * [Example](#example)
  * [Updating tree-sitter-matlab module](#updating-tree-sitter-matlab-module)
  * [Under the hood](#under-the-hood)
    + [cleanOctaveC.ts](#cleanoctavects)
    + [cleanUp.ts](#cleanupts)
    + [index.ts](#indexts)
    + [generateCode.ts](#generatecodets)
    + [typeInference.ts](#typeinferencets)
    + [identifyCustomFunctions.ts](#identifycustomfunctionsts)
    + [helperFunctions.ts](#helperfunctionsts)
    + [builtinFunctions.ts](#builtinfunctionsts)
    + [treeTraversal.ts](#treetraversalts)
- [Example 1](#example-1)
  * [1. Identify custom functions](#1-identify-custom-functions)
  * [2. Type inference](#2-type-inference)
  * [3. Generate code](#3-generate-code)
- [OctaveC Tests](#octavec-tests)
- [Current limitations/works in progress](#current-limitations-works-in-progress)

# ts-traversal

**ts-traversal** generates C code from MATLAB code by generating a parse tree, traversing the tree, and performing "operations" on each node. Note that all of the following code is currently being tested on the Grace cluster, so terminal commands are provided with that environment in mind.

## Built with

* [tree-sitter](https://github.com/tree-sitter/tree-sitter)
* [dts-tree-sitter](https://github.com/asgerf/dts-tree-sitter)
* [tree-sitter-matlab](https://github.com/daniellegruber/tree-sitter-matlab)

## Loading modules

It is important you load binutils before nodejs.
```sh
module load binutils
module load nodejs
module load GCC
module load ScaLAPACK/2.1.0-gompi-2020b
module load FFTW
``` 

For convenience, it is helpful to define several environment variables:

```sh
export TS_TRAVERSAL="$YOUR_DIR/ts-traversal"
cd $TS_TRAVERSAL
export EEGLAB="$YOUR_DIR/eeglab"
export OCTAVEC="$YOUR_DIR/Halo-Algorithm/OctaveC"
export TEST="$YOUR_DIR/Halo-Algorithm/OctaveC/tests"
``` 

## Usage
First change into generatedCode directory since Makefile is stored there.

```sh
cd generatedCode
```

### Copy and clean
Copy mfiles from tests_C_Octave folder into generatedCode and "clean"
```sh
make copyall
```

Alternatively, to copy only one mfile, use
```sh
make copyone ARGS=mfile
```

### Create "torun" version 
- Copy mfiles in all folders and create "torun" versions to be used in the comparison stage
- Within each mfile folder, generates folder matlabToRun and mfile testname_torun.m
- The "torun" version does the following:
  - Creates a fileID for the output file output.txt
  - Replaces calls to `disp` with `dispArr` (a function within the generateCode folder to help format outputs and send to output file)
  - Moves functions defined in file to their own files since the former isn't supported when calling MATLAB from command line

```sh
make matlabtorun
```

### Generate c code
Generates main.c for all mfiles in generatedCode folder that haven't already been generated
Also generates "torun" version of mfile by calling matlabtorun
```sh
make genall
```

Alternatively, to generate code for all folders, regardless of whether they've been already generated, use
```sh
make regenall
```

Alternatively, to generate code for only one mfile, use
```sh
make genone ARGS=mfile
```

### Compile c code
- Compile all C programs in generatedCode folder
- Generates log compile_log.txt with list of successful and failed tests
- SPEED argument
  - 0: all tests
  - 1: non-speed tests
  - 2: speed tests
```sh
make compileall SPEED=1
```

Alternatively, to compile only one C program, use
```sh
make compileone ARGS=testname
```

### Compare C and MATLAB
- Compare outputs of generated C program and original mfile for all tests
- Generates log compare_log.txt with list of successful and failed tests
- SPEED argument
  - 0: all tests
  - 1: non-speed tests
  - 2: speed tests
```sh
make compareall SPEED=1
```

Alternatively, to compare outputs of the generated C program and original mfile for one test, use
```sh
make compareone ARGS=testname
```

### General usage for generating C code for mfile
```sh
npx tsc -sourcemap index.ts
npx ts-node index.ts MATLAB_FILE SEARCH_FOLDER OUT_FOLDER SHOW_OUTPUT DEBUG
```

where
- `MATLAB_FILE`: main .m file to parse
- `SEARCH_FOLDER`: used to locate functions called upon in `MATLAB_FILE` (i.e., contains other .m files required for execution of `MATLAB_FILE`)
- `OUT_FOLDER`: generatedCode folder generated in this folder
- `SHOW_OUTPUT`: 1 to show generated code in console, 0 to suppress
- `DEBUG`: 1 for debug mode in console, 0 for normal mode

The first line is for debugging purposes and only needs to be used when a file is changed. The second line generates C code for the specified mfile and outputs it in a subfolder of the same name in generatedCode. 

## Example

Here is an example using one of the MATLAB test files in the OctaveC folder.

```sh
npx ts-node cleanUp.ts areEqual_matrix.m $TS_TRAVERSAL `# Clean up mfile from OctaveC test folder`
npx ts-node index.ts $TS_TRAVERSAL/generatedCode/areEqual_matrix/areEqual_matrix.m $TEST $TS_TRAVERSAL 1 0 `# Generate C code`
cd generatedCode/areEqual_matrix `# Change into generated directory`
make test `# Compile the automatically generated C code`
make check `# Compile the manually written C code`
./test `# Compare outputs`
./check
```

Here is a "real life" example using one of the EEGLAB functions. (You can download EEGLAB [here.](https://sccn.ucsd.edu/eeglab/download.php))

```sh
npx ts-node index.ts \
$EEGLAB/functions/@mmo/binaryopp.m \
$EEGLAB/functions \
$TS_TRAVERSAL/eeglab_test 0
```
## After first downloading the Halo folder
After first downloading the Halo folder or updating it, run 

```sh
npx ts-node modifyHalo.ts
```

to modify matrix.c and matrix.h so that they work with the current environment (the Grace cluster).

## Updating tree-sitter-matlab module
Whenever the tree-sitter-matlab grammar is updated, the corresponding module as well as the .d.ts files should be updated via the following commands:
```sh
export YARN="$TS_TRAVERSAL/node_modules/yarn/bin/"
$YARN/yarn add tree-sitter-matlab
export TS_MATLAB="$TS_TRAVERSAL/node_modules/tree-sitter-matlab"
node ./node_modules/@asgerf/dts-tree-sitter/build/src/index.js $TS_MATLAB > OUTPUT.d.ts
node ./node_modules/@asgerf/dts-tree-sitter/build/src/index.js tree-sitter-matlab > generated.d.ts
```

where `$TS_TRAVERSAL` is the path to your ts-traversal folder.

# Updates (OLD)
- Redefinition of a variable creates a new entry in var_types. If the scopes of two same-named variables overlap then the scope of the first ends where the scope of the second begins.
- If a (non-temporary, i.e., generated by the program) 1x1 matrix "flatten" to regular int, double, or complex
 - Example: functions maxM and minM return 1x1 matrices
- Added function parameters object (fun_params) and related updating/retrieving function to generateCode.ts to more easily pass global variables in this file to functions in other files

# Updates (03/18/23)
- Removed rowMajorFlatIdx function to remove clutter when converting subcripts/indices; now indices converted with help of C function convertSubscript.c in generatedCode folder

# Current limitations/works in progess (03/26/23)
- In power_matrix, in i_negative test fails
  - However, matlab output for this test in different when running in Grace vs running on my PC - the former yields a "matrix is close to singular" error whereas the latter yields the same result as the C code
  - Why is this? Is it because of different MATLAB versions?
- In stftM, order of output C matrix does not match order of MATLAB matrix, but values seem to match
  - Changed matrix.c stftV function so that it would output regular Fourier coeff, not absolute values
- In hamming_hanning, C-defined hanning doesnt work as expected
- In determinant_matrix, C-defined detM doesnt work as expected
- How to deal with varargout (varargin addressed)

# New: support for unknown types
Say you have a standalone MATLAB function like the following, where the types of the input arguments are unknown, since there are no function calls provided in some main script.

```matlab
function B = myfun(A)
    B = A';
    disp(B(end));
    disp(B(end, 1));
end
```

The generated C code is as follows:
```c
// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"
#include "../unknownTypes.h"

    
Matrix * myfun(Matrix * A) {
    int *A_dim = getDimsM(A);
    int A_dim0 = A_dim[0];
    int A_dim1 = A_dim[1];
    int A_type = gettypeM(A);
    Matrix * tmp1 = ctransposeM(A);
    Matrix * B = tmp1;
    int ndim1 = getnDimM(B);
    int *dim1 = getDimsM(B);
    int idx1 = convertSubscript(ndim1, dim1, (A_dim1)*(A_dim0));
    struct generic_val tmp2;
    unknown_indexM(&tmp2, A_type, B, 1, idx1+1);
    unknown_printf(&tmp2);
    struct generic_val tmp3;
    unknown_indexM(&tmp3, A_type, B, 1, (1-1) + (A_dim1-1)*A_dim0 + (1-1)*A_dim1*A_dim0 + (1-1)*A_dim1*A_dim0*1);
    unknown_printf(&tmp3);
    return B;
}
```

To test it, the following chunk of code can be added, and it can be seen that it will function in the same way as the MATLAB code.
```c
int main(void) {
    int dim[2] = {3,2};
    Matrix * B = myfun(randM(2, dim));
    printM(B);
    return 0;
}
```

# Under the hood
### cleanOctaveC.ts 
#### Overview
 - Use after newly downloading OctaveC folder to update matrix.c and main.h so they're compatible with ts_traversal

### cleanUp.ts
#### Overview
 - "Cleans up" MATLAB files in OctaveC/tests folder so that they're easier to parse and convert to C code

### index.ts
##### Overview
 - Entry-point code
##### Details

### generateCode.ts
#### Overview
 - Generates code based on node types and values
#### Functions
 - `main`: entry-point function
 - `transformNode`: based on inferred node type (from typeInference) and node content, transforms node into C equivalent
   - "Aliases" are created for certain variables that the program evaluates via temporary variables. `alias_tbl`, an array of type `Alias` (see below), keeps track of information about the relationship between variables and the temporary variables generated to perform operations on them. For example, say you have the code below. Since `zeros` is a "builtin function," it generates a temporary variable. `alias_tbl` allows the program to know that `A` is associated with this temporary variable.
   MATLAB:
   ```matlab
   A = zeros(2,2);
   for i = 1:4
    A(i) = i^2
   end
   ```
   C:
   alias_tbl:
    
### typeInference.ts
#### Overview
- Infers types of variabes used in program
#### Functions
- `typeInference`: Entry-point function.
  - Returns: `[var_types, custom_functions]`
- `inferTypeFromAssignment`: Iterates through assignment statements and updates variables on the LHS in `var_types`.
  1. The program begins by traversing the tree and keeping track of block starts, ends, and their level of nesting in the array `block_idxs`. This is required for determining variable scopes in the next part.
   - Each entry in `block_idxs` has the format `[node.startIndex, node.endIndex, block_level]`, with `block_level = -1` for function definitions to distinguish them from the main body.
  2. Next, the program traverses all of the assignment statements and updates variables on the LHS in `var_types`.
   - To determine the type of each variable, `inferType` is called.
   - To determine the scope of each variable, `findVarScope` is called. However, if a variable is redefined within the same scope as its previous instance, and thus an entry for it already exists in `var_types`, the scope for the two instances is "split." E.g., in the following code, `findVarScope` would identify both instances of `x` as belonging to the block `[0, 13, 0]`. However, the scope is split between the two so that the first `x` has scope `[0, 6, 0]` and the second has scope `[7, 13, 0]`. 
   ```matlab
   x = 1;
   x = 5;
   ```
  - Returns: `[var_types, custom_functions]`
- `getFunctionReturnType`: Gets return type of function by retrieving type from `custom_functions` or `builtin_functions` and updates the function's entry in `custom_functions` or `builtin_functions` with information regarding input/output types.
  - When inferring the type of the function's return variable via `inferType`, `arg_types` (the types of the arguments of the function call) is passed as the `var_types` parameter, thus instantiating parameters with known types.
  - Additionally, since `custom_functions` or `builtin_functions` is updated, this allows function calls to provide information about input/output types for other instances of the function (both in function calls and in the function definition, if it exists).
  - Returns: `[return_type, fun_dictionary]`, where `fun_dictionary` is an updated copy of either `custom_functions` or `builtin_functions`
- `inferType`: Main type inference procedure.
  - Returns: `[type, ndim, dim, ismatrix, ispointer, isstruct, custom_functions]`
  
  
### identifyCustomFunctions.ts
- Overview
  - Identifies user-defined functions to create a dictionary of custom functions
- Outputs
  - `custom_functions`: Typed array of custom functions of type `customFunction` (see below)
  - `file_traversal_order`: Order in which to traverse files for type inference and code generation, necessary since most deeply nested functions should have their types inferred first

### helperFunctions.ts
#### Overview
 - Helper functions
#### Functions
 - `getFilesInPath`: gets files in a given directory
 - `getNonClassFilesInPath`: gets non-class files in a given directory
 - `getClassFolders`: returns folders containing class definitions
 - `getClasses`: returns user-defined classes (using `getClassFolders`)
 - `parseFunctionDefNode`: necessary since sometimes function definitions present as ERROR type (due to missing end at the end of function definition)

### builtinFunctions.ts
#### Overview
- Transforms built-in (not user-defined) MATLAB functions into C functions
#### Exports
- `builtin_functions`: Typed array of type `functionMapping` (see below) containing information about how to transform each built-in MATLAB function to one or more C functions. For MATLAB code of the form `outs = fun_matlab(args)` or simply `fun_matlab(args)`,
  - `fun_matlab`: MATLAB function
  - `fun_c`: If not `NULL`, the corresponding C function for the given MATLAB function and argument types.
  - `args_transform`: Transforms original arguments to MATLAB function, e.g.,
     MATLAB:
     ```matlab
     A = zeros(3);
     ``` 
     C:
     ```c
     int ndim= 2;
     int dim[2]= {3, 3};
     Matrix * A = zerosM(ndim, dim);
     ```
  - `outs_transform`: Transforms original outputs of MATLAB function, e.g.,
     MATLAB:
     ```matlab
     [M, I] = max(x);
     ``` 
     C:
     ```c
     int I;
     Matrix * M = maxV(x, &I)
     ```
  - `n_req_args`: Number of required arguments for the C function.
  - `n_opt_args`: If arguments for the MATLAB and C functions are the same, this is the number of arguments that are optional when passed to the MATLAB function.
  - `opt_arg_defaults`: Default values for the optional arguments. If the number of arguments passed to the MATLAB function is less than the number of arguments required by the C function and this field is not `NULL`, then the default values for the optional arguments are appended to the given arguments. E.g.,
     MATLAB:
     ```matlab
     y = lognpdf(x);
     ```
     C:
     ```c
     Matrix * y = lognpdfM(x, 0, 1);
     ```
  - `ptr_args`: If a MATLAB function has more than one output, often these outputs are passed by reference to the C function, i.e., as pointer arguments. This field specifies how to convert outputs to pointer arguments, if applicable. E.g.,
     MATLAB:
     ```matlab
     [mu,sigma] = normfit(A);
     ``` 
     C:
     ```c
     double mu;
     double sigma;
     normfitM(A, &mu, &sigma)
      ```
  - `return_type`: If the C function returns a value, then this field specifies the type of the return value. Otherwise, it is `NULL`.
  - `push_main_before`: Specifies an expression to push to the main code before the call to the function.
  - `push_main_after`: Specifies an expression to push to the main code after the call to the function.
  - `init_before`: Specifies variables to initialize before the call to the function.
  - `tmp_out_transform`: Forces the function call output to be stored in a temporary variable and specifies how to transform the temporary variable. E.g.
     MATLAB:
     ```matlab
     disp(size(A, 1))
     ``` 
     C:
     ```c
     int * tmp = getDimsM(A);
     printf("\n%d\n", tmp[0]);
    ```

### treeTraversal.ts
##### Overview 
  - Contains algorithms for traversing tree
##### Functions
  - `gotoPreorderSucc`: cursor function, traverses nodes of tree preorder
  - `gotoPreorderSucc_OnlyMajorTypes`: same as cursor function above but does not traverse children of "major types": function definitions, if statements, while statements, for statements, call/subscripts, expression statements and comments 
  - `gotoPreorderSucc_SkipFunctionDef`: same as cursor function above but does not traverse children of function definitions
  - `fileIsFunction`: determines whether file is a function
  - `findEntryFunction`: if file is a function, determines the function

### modifyCode.ts
#### Overview 
  - Contains all helper functions for pushing, inserting, or replacing generated expressions to the already -generated code (main_function or function_definitions). All functions used within generateCode.ts.
#### Functions
  - `pushToMain`
  - `insertMain`
  - `replaceMain`

### convertSubscript.ts
#### Overview 
- Contains all helper functions for generating code for a subscript. All functions used within generateCode.ts.
##### Functions
- `slice2list`
- `matrix2list`
- `sub2idx`: Main function for transforming subscripts.
- `rowMajorFlatIdx`: Converts a given subscript into a row-major subscript.

### customTypes.ts
- Overview 
 - Contains all custom object types.
- Custom object types
  - Type

    ``` typescript
    type Type = {
      type: string;
      ndim: number;
      dim: Array<number>;
      ismatrix: boolean;
      ispointer: boolean;
      isstruct: boolean;
    };
    ```
    
  - VarType
  
    ``` typescript
    type VarType = {
      name: string;
      type: string;
      ndim: number;
      dim: Array<number>;
      ismatrix: boolean;
      ispointer: boolean;
      isstruct: boolean;
      initialized: boolean;
    };
    ```
   
  - CustomFunction
    ```typescript
    type CustomFunction = {
        name: string;
        arg_types: Array<VarType>;
        return_type:Type;
        //return_type: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Type; };
        outs_transform: { (outs: Array<string>): Array<string>; }; 
        ptr_args: { (arg_types: Array<Type>, outs: Array<string>): Array<VarType>; };
        external: boolean;
        file: string;
        def_node: g.SyntaxNode;
    };
    ```
    
  - Alias
    ``` typescript
    type Alias = {
      name: string;
      tmp_var: string;
      scope: number[];
    };
    ```
    
  - FunctionMapping
    ```typescript
    type functionMapping = {
        fun_matlab: string;
        fun_c: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): string; };
        args_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; 
        outs_transform: { (outs: Array<string>): Array<string>; }; 
        n_req_args: number; // # required args
        n_opt_args: number; // # optional args
        opt_arg_defaults: Array<string>;
        ptr_args: { (arg_types: Array<Type>, outs: Array<string>): Array<VarType>; };
        return_type: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Type; };
        push_main_before: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; // push to main before function call 
        push_main_after: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; // push to main after function call
        init_before: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<InitVar>; }; // vars to initialize before function call 
        tmp_out_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<InitVar>; }; // vars to initialize before function call 
    };
    ```

# Example 1

Source code:

```matlab
A = [1, 2.1, 1;
    3, 4, 1];
B = A * A';
C = 3 * B;
[F, G] = myfun1(1, 2);
function [F, G] = myfun1(f, g)
    F = f + g;
    G = f - g;
end
```

Generated code for main.c:

```c
//Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include <main.h>

// Function declarations
void myfun1(int f, int g, int* p_F, int* p_G);

// Entry-point function
int main(void) {

	
	int ndim1 = 2;
	int dim1[2] = {2,3};
	Matrix * A = createM(ndim1, dim1, 1);
	double *input1 = NULL;
	input1 = malloc( 6*sizeof(*input1));
	input1[0] = 1;
	input1[1] = 2.1;
	input1[2] = 1;
	input1[3] = 3;
	input1[4] = 4;
	input1[5] = 1;
	writeM( A, 6, input1);
	free(input1);
	
	Matrix * tmp1= ctransposeM(A);
	Matrix * tmp2= mtimesM(A, tmp1);
	Matrix * B= tmp2;
	int scalar1 = 3;
	Matrix * tmp3= scaleM(tmp2, &scalar1, 1);
	Matrix * C= tmp3;
	int F1;
	int G1;
	myfun1(1, 2, &F1, &G1);
	return 0;
}


// Subprograms

void myfun1(int f, int g, int* p_F, int* p_G) {
	int F= f + g;
	int G= f - g;
	*p_F = F;
	*p_G = G;
}
```

Generated code for main.h:

```c
#ifndef MAIN_H
#define MAIN_H
int main(void);
#endif
```

## 1. Identify custom functions
identifyCustomFunctions.ts identifies myfun1 as a custom function and thus returns the following "placeholder dictionary." Some of the fields will be updated during the type inference program, in particular in the body of function `getFunctionReturnType.`

```typescript
[
  {
    name: 'myfun1',
    arg_types: [ [Object], [Object], [Object], [Object] ],
    return_type: null,
    outs_transform: [Function: outs_transform],
    external: true,
    file: '/home/dlg59/project/ts-traversal/generatedCode/simple_test/simple_test.m',
    def_node: FunctionDefinitionNode {
      type: function_definition,
      startPosition: {row: 5, column: 0},
      endPosition: {row: 8, column: 3},
      childCount: 7,
    },
    ptr_args: [Function: ptr_args],
    var_types: [ [Object], [Object], [Object], [Object] ]
  }
]
```


## 2. Type inference
typeInference.ts
1. The program begins by traversing the tree and keeping track of block starts, ends, and their level of nesting in the array `block_idxs`. 
2. Next, the program traverses all of the assignment statements in the function `inferTypesFromAssignment`:
 - To determine the scope of each variable, `findVarScope` is called. However, if a variable is redefined within the same scope as its previous instance, and thus an entry for it already exists in `var_types`, the scope for the two instances is "split."
a. The program encounters the assignment statement `A = ...` in lines 1-2.
  - `inferType` is called on the RHS node. Since the RHS is of type `g.SyntaxType.Matrix`, no recursion is needed and the defining features of the matrix node are returned: `type, ndim, dim, ismatrix,ispointer, isstruct`.
  - `var_types` is updated with a new entry, the variable being the LHS of the assignment statement and all of its features being those received from the call to `inferType` on the RHS node:
    ```typescript
    {
      name: 'A',
      type: 'double',
      ndim: 2,
      dim: [ 2, 3 ],
      ismatrix: true,
      isvector: false,
      ispointer: false,
      isstruct: false,
      initialized: false,
      scope: [ 0, 76, 0 ]
    }
    ```

b. The program encounters the assignment statement `B = ...` in line 3.
  - Since the RHS is of type `g.SyntaxType.BinaryOperator`, `inferType` is called on each of the two operand nodes.
  - Since the left operand node is of type `g.SyntaxType.Identifier`, the name of the node (`A`) is looked up in `var_types`. `var_types` was just updated with an entry for `A` so this will return a match. Therefore `inferType` will return all the defining features of `A`: `left_type, left_ndim, left_dim, left_ismatrix, ...`.
  - Since the right operand node is of type `g.SyntaxType.TransposeOperator`, `inferType` is called on the the argument node, which yields the same as above. Since this is a transpose operation, `dim[0]` and `dim[1]` are swapped to arrive at the new dimensions. All the other fields are preserved and `inferType` returns `right_type, right_ndim, right_dim, right_ismatrix, ...`
  - `left_ismatrix` and `right_ismatrix` are compared to yield `ismatrix = true`
  - `left_type` and `right_type` are compared to yield `type = float`
  - Since the operator is `*`, the new dimensions are constructed as `[ left_dim[0], right_dim[1] ] = [2, 2]`
  - Thus, the following entry is added to `var_types`:

	```typescript
	{
	name: 'B',
	type: 'double',
	ndim: 2,
	dim: [ 2, 2 ],
	ismatrix: true,
	isvector: false,
	ispointer: false,
	isstruct: false,
	initialized: true,
	scope: [ 0, 76, 0 ]
	}
	```
c. The program encounters the assignment statement `C = ...` in line 4.
  - The program procedes through the same steps as in (b) until calculation of the new dimensions. Since the left operand is a scalar new dimensions are set to the dimensions of the right operand, i.e., `right_dim = [2, 2]`.
d. The program encounters the assignment statement `[F,G] = myfun1(f,g)` in line 5.
  - Since the RHS node is of type `g.SyntaxType.CallOrSubscript`, the program discerns whether it is a function call or subscript by checking for its name in `classes`, `builtin_functions`, and `custom_functions`. A match is found in `custom_functions`, and the corresponding entry is stored in `obj2`.
  - `getFunctionReturnType` is called to determine the type of the return variable of the function as well as update the function's entry in `custom_functions` using the arguments passed to the function call node.
  - Since the return variable is a matrix, the `CustomFunction` field `ptr_args` is updated to return an array of type `VarType` containing the names and types of pointer variables corresponding to each of the elements of the returned matrix. The type of each pointer is found by calling `inferType.` For the given function call node, `ptr_args(arg_types, outs)` yields:
	```typescript
	[
		{
		name: 'F',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: true,
		isstruct: false
		},
		{
		name: 'G',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: true,
		isstruct: false
		}
	]

	```
  - Additionally, the `CustomFunction` field `var_types` is updated with the function's local variables:
	```typescript
	[
		{
		name: 'f',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		ispointer: false,
		original_out: false,
		scope: [ 76, 140, 0 ]
		},
		{
		name: 'g',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		ispointer: false,
		original_out: false,
		scope: [ 76, 140, 0 ]
		},
		{
		name: 'F',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: false,
		scope: [ 76, 140, 0 ]
		},
		{
		name: 'G',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: false,
		scope: [ 76, 140, 0 ]
		}
	]
	```
  - The `CustomFunction` field `outs_transform` returns `NULL` since all of the outputs are treated as inputs to the function. For this reason `return_type` is `NULL` as well.
  - The final value of `var_types` is as follows:
  	```typescript
	[
		{
		name: 'A',
		type: 'double',
		ndim: 2,
		dim: [ 2, 3 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'B',
		type: 'double',
		ndim: 2,
		dim: [ 2, 2 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'C',
		type: 'double',
		ndim: 2,
		dim: [ 2, 2 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'ndim1',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'dim1',
		type: 'int',
		ndim: 2,
		dim: [ 2 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'tmp1',
		type: 'double',
		ndim: 2,
		dim: [ 3, 2 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'tmp2',
		type: 'double',
		ndim: 2,
		dim: [ 2, 3 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'scalar1',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'tmp3',
		type: 'double',
		ndim: 1,
		dim: [ 1, 3 ],
		ismatrix: true,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'F1',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		},
		{
		name: 'G1',
		type: 'int',
		ndim: 1,
		dim: [ 1 ],
		ismatrix: false,
		isvector: false,
		ispointer: false,
		isstruct: false,
		initialized: true,
		scope: [ 0, 76, 0 ]
		}
	]
	```

## 3. Generate code
generateCode.ts
`main` begins traversing all of the "major" syntax types
1. The program encounters the expression statement (of type `g.SyntaxType.Expression`) `A = ...` in lines 1-2.
  - `transformNode` is called on the first child node, which is of type `g.SyntaxType.Assignment`.
  - Since the RHS node is of type `g.SyntaxType.Matrix`, the `initializeMatrix` function is called.
2. The program encounters the expression statement (of type `g.SyntaxType.Expression`) `B = ...` in line 3.
  - `transformNode` is called on the first child node, which is of type `g.SyntaxType.Assignment`.
  - Since the RHS node is not of type `g.SyntaxType.Matrix`, `g.SyntaxType.Cell`, or `g.SyntaxType.CallOrSubscript`, `transformNode` is called on the RHS node.
  - Since the RHS node is of type `g.SyntaxType.BinaryOperator`, the node is passed is to the function `printMatrixFunctions`.
  - `parseNode(node)` returns `[args, outs, is_subscript]` and each entry in `args` is transformed with a call to `transformNode`.
  	- Since the left operand is of type `g.SyntaxType.Identifier` and it has no alias, it remains the same.
	- Since the right operand is of type `g.SyntaxType.TransposeOperator`, the node is passed is to the function `printMatrixFunctions`.
  	- `parseNode(node)` returns `[args, outs, is_subscript]`.
	- The entry for the transpose operator is searched for in `operatorMapping` and the match is assigned to `obj`.
	- The type of the result of the operation is found using the `return_type` field of `obj`: `return_type = obj.return_type(args, arg_types, outs)`.
	- The C function (or lack thereof) corresponding to the MATLAB operator is found using the `fun_c` field of `obj`: `fun_c = obj.fun_c(arg_types, outs)`. In this case, `obj.fun_c(arg_types, outs)` returns `ctransposeM` because the argument node is a matrix. 
	- Since the result of the operation is a matrix (`return_type.ismatrix = TRUE`), a temporary variable `tmp_var` is created using `generatedTmpVar` to store the result. Since it is the first temporary variable created in the program it will have a value of tmp1.
	- A call to `initVar` returns `Matrix * tmp1 = ctransposeM(A)` and this is pushed to the main body of the code.
	- `tmp_var` is returned by `printMatrixFunctions` and then by `transformNode` so that the larger expression containing the tranpose operation can replace it with the temporary variable.
  - The entry for the binary operator is searched for in `operatorMapping` and the match is assigned to `obj`.
  - The type of the result of the operation is found using the `return_type` field of `obj`: `return_type = obj.return_type(args, arg_types, outs)`.
  - The C function corresponding to the MATLAB operator is found using the `fun_c` field of `obj`: `fun_c = obj.fun_c(arg_types, outs)`. In this case, `obj.fun_c(arg_types, outs)` returns `mtimesM` because both operands are matrices. 
  - Since the result of the operation is a matrix (`return_type.ismatrix = TRUE`), a temporary variable `tmp_var` is created using `generatedTmpVar` to store the result. Since it is the second temporary variable created in the program it will have a value of tmp2.
  -  A call to `initVar` returns `Matrix * tmp2 = mtimesM(A, tmp1)` and this is pushed to the main body of the code.
  -  Since `tmp2` is an "alias" for `B`, the following entry is pushed to `alias_tbl`: `{ name: 'B', tmp_var: 'tmp2', scope: [ 0, 76, 0 ] }`
  - `tmp_var` is returned by `printMatrixFunctions` and then by `transformNode` so that the larger expression containing the binary operation can replace it with the temporary variable.
3. The program encounters the expression statement (of type `g.SyntaxType.Expression`) `C = ...` in line 4.
  - The program proceeds the same as in (2) until `init_before = obj.init_before(args, arg_types, outs)` is evaluated. In this case, `init_before` is not `NULL`; it contains the variable `scalar` to initialize before the function call.
  - `fun_c = obj.fun_c(arg_types, outs)` is evaluated and returns `scaleM` because only one operand is a matrix. 
  - Since the result of the operation is a matrix (`return_type.ismatrix = TRUE`), a temporary variable `tmp_var` is created using `generatedTmpVar` to store the result. Since it is the third temporary variable created in the program it will have a value of tmp3.
  - A call to `initVar` returns `Matrix * tmp3= scaleM(tmp2, &scalar1, 1)` and this is pushed to the main body of the code.
  - Since `tmp3` is an "alias" for `C`, the following entry is pushed to `alias_tbl`: `{ name: 'C', tmp_var: 'tmp3', scope: [ 0, 76, 0 ] }`
  - `tmp_var` is returned by `printMatrixFunctions` and then by `transformNode` so that the larger expression containing the tranpose operation can replace it with the temporary variable.
4. The program encounters the expression statement (of type `g.SyntaxType.Expression`) `[F,G] = myfun1(f,g)` in line 5.
  - `transformNode` is called on the first child node, which is of type `g.SyntaxType.Assignment`.
  - Since the RHS node is of type `g.SyntaxType.CallOrSubscript`, the program discerns whether it is a function call or subscript by checking for its name in `classes`, `builtin_functions`, and `custom_functions`. A match is found in `custom_functions`, and the corresponding entry is stored in `obj1`.
  - The LHS for the generated expression is evaluated using the `outs_transform` field of `obj1`: `lhs = obj1.outs_transform(outs);`. This evaluates as `NULL` since the `outs_transform` function returns `NULL` for any custom function with multiple return variables (the multiple outputs are converted to pointer inputs).
  - THE RHS for the generated expression is evaluated by calling `transformNode` on the RHS node: `rhs:string = transformNode(node.rightNode);`.
  - Since `lhs == NULL`, only `rhs` is pushed to the main body of the code.
5. The program encounters the function definition (of type `g.SyntaxType.FunctionDefinition`) `function [F,G] = myfun1(f,g) ...` in lines 6-8.
  - The node is passed to `printFunctionDefDeclare`.
  - The parameter of the function are parsed and their types and values and stored in the array `param_list`.
  - Since the function returns an output and this output is a matrix, each of the elements of the output matrix are transformed into pointer variables. Their declarations are stored in the array `ptr_declarations` and their types and values are pushed onto `param_list` so that they are treated as inputs to the function.
  - The transformed output (`"void"`) and parameters are pushed to `function_declarations`.
  - The transformed output (`"void"`) and parameters, the transformed body of the function, and `ptr_declaration` are pushed to `function_definitions`.
  - The final value of `alias_tbl` is as follows:
	```typescript
	[
	  { name: 'B', tmp_var: 'tmp2', scope: [ 0, 76, 0 ] },
	  { name: 'C', tmp_var: 'tmp3', scope: [ 0, 76, 0 ] },
	  { name: 'F', tmp_var: 'F1', scope: [ 0, 76, 0 ] },
	  { name: 'G', tmp_var: 'G1', scope: [ 0, 76, 0 ] }
	]
	```

 


