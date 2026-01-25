//Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "./convertSubscript.h"

int convertSubscript(int ndim, int dim[], int idx) {
    int newidx;
    int d0=1, d1=1, d2=1, d3=1;
    
    int numel = 1;
    for (int i = 0; i < ndim; i ++) {
        numel *= dim[i];
    }
    if (idx > numel) {
        return idx - 1;
    }
    
    // Tmp variables
    if (ndim <= 2) {
        d0 = idx % dim[0];
        if (d0 == 0) {
            d0 = dim[0];
        }
        d1 = (idx - d0)/dim[0] + 1;
    
    } else if (ndim == 3) {
        d2 = ceil((double) idx / (dim[0] * dim[1]));
        int tmp_var = idx % (dim[0] * dim[1]);
        if (tmp_var == 0) {
            tmp_var = dim[0] * dim[1];
        }
        d0 = tmp_var % dim[0];
        if (d0 == 0) {
            d0 = dim[0];
        }
        d1 = (tmp_var - d0)/dim[0] + 1;
            
    } else if (ndim == 4) {
        d3 = ceil((double) idx / (dim[0] * dim[1] * dim[2]));
        d2 = ((int) ceil((double) idx / (dim[0] * dim[1]))) % dim[2];
        if (d2 == 0) {
            d2 = dim[2];
        }
        int tmp_var = idx % (dim[0] * dim[1]);
        if (tmp_var == 0) {
            tmp_var = dim[0] * dim[1];
        }
        d0 = tmp_var % dim[0];
        if (d0 == 0) {
            d0 = dim[0];
        }
        d1 = (tmp_var - d0)/dim[0] + 1;
        
    }
    
    // Calculate idx
    newidx = (d1-1) + (d0-1) * dim[1] + (d2-1) * dim[0] * dim[1] + (d3-1) * dim[0] * dim[1] * dim[2];
    return newidx;
}