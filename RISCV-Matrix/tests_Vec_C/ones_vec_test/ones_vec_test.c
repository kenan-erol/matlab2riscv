//
//  ones_vec_test.c
//  Octave-C 
//
//  Created by Zach Taylor on 12/14/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../../src/tgmath.h"
#include "../../src/matrix.h"

#include "../common/utils.h"

bool ones_vec_test(int s1, int s2, bool accuracy_check)
{
    bool success = true;

    int ndim = 2;
    int dim[2] = {s1,s2};

    int size = 1;
    for (int i = 0 ; i < ndim ; i++)
    {
        size *= dim[i];
    }

    char notes[] = "";

    // Get the reference time (in cycles)
    unsigned long start_ref = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_ref = onesM_ref(ndim, dim);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_ref = get_cycles();
    double elapsed_ref = elapsed_cycles(start_ref, end_ref);

    // Get the vectorized time (in cycles)
    unsigned long start_vec = get_cycles();
    #ifndef USE_RISCV_VECTOR
    START_TRACE;
    #endif
    Matrix *result_vec = onesM_vec(ndim, dim);
    #ifndef USE_RISCV_VECTOR
    STOP_TRACE;
    #endif
    unsigned long end_vec = get_cycles();
    double elapsed_vec = elapsed_cycles(start_vec, end_vec);

    // Get the ratio
    double ratio = elapsed_vec/elapsed_ref;

    if (accuracy_check)
    {
        // Print it out
        char *op_name = "ones";
        char *mat_type = "integer";
        print_cycles_M("vec", op_name, mat_type, s1, s2, notes, elapsed_vec);
        print_cycles_M("ref", op_name, mat_type, s1, s2, notes, elapsed_ref);
        print_ratio_M(op_name, mat_type, s1, s2, notes, ratio);

        if(!isEqualM(result_ref, result_vec))
        {
            success = false;
            print_error_M(op_name, mat_type, s1, s2, notes);
            Matrix *eq = equalM(result_ref, result_vec);

            int size = getsizeM(eq);
            for (int index = 1 ; index < size+1 ; index++)
            {
                int eq_val = 0;
                indexM(eq, &eq_val, 1, index);

                // Skip places where the ref and vec are equal
                if (eq_val == 1)
                {
                    continue;
                }

                // Show the differences
                int ref_val_i = -1;
                indexM(result_ref, &ref_val_i, 1, index);
                fprintf(stdout,"Ref val: %d\n", ref_val_i);

                int vec_val_i = -1;
                indexM(result_vec, &vec_val_i, 1, index);
                fprintf(stdout,"Vec val: %d\n", vec_val_i);
                fprintf(stdout,"\n");
            }

            destroyM(eq);
        }
    }
    else
    {
        print_table_size(s1, s1);
        print_table_field(elapsed_vec, elapsed_ref, ratio);
        print_table_newline();
    }

    if (!destroyM(result_ref))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }
    if (!destroyM(result_vec))
    {
        fprintf(stderr, "Failed to destroy matrix\n");
    }

    return(success);
}

int main(int argc, char *argv[]) 
{
    char *comment="Normal";
    return(run_test(argc, argv, ones_vec_test,"onesM", ONES, comment));
}