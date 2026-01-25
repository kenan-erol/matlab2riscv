//Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>

// Initialize variables
unknown tmp;
unknown res;


unknown length(unknown this)
{

tmp = struct(this);

//if any(cellfun(@length, { tmp.EEG }) > 1)
//    error('EEG structure in object with more than 1 element')
//end


if (res > 1)
{
error('EEG structure in object with more than 1 element');
}

res = length(tmp);

}