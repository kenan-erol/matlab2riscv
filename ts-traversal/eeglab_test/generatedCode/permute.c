//Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <length.h>
#include <length.h>

// Entry-point function
int permute(void)
{

// Initialize variables
unknown newFileName;
unknown res;
unknown res.dimensions;
unknown res.dataFile;
unknown tmpMMO1;
unknown tmpMMO2;
unknown d;
char * s.type;





if (length(dims) > 3)
{
error('Max 3 dimensions for permutation');
}
else if (length(dims) == 2)
{
error('Permutation with 2 dimensions: use transpose instead');
}
newFileName = mmo.getnewfilename;
double tmp3;
indexM(copyfile, &tmp3, obj.dataFile, newFileName);
tmp3;
res = obj;
double tmp4;
indexM(obj.dimensions, &tmp4, dims);
res.dimensions = tmp4;
res.dataFile = newFileName;
tmpMMO1 = memmapfile(obj.dataFile, 'writable', obj.writable, 'format', { 'single' obj.dimensions 'x' });
tmpMMO2 = memmapfile(res.dataFile, 'writable', true        , 'format', { 'single' res.dimensions 'x' });
// copy the data
// -------------
d = res.dimensions;
s.type = '()';
// slower versions below
// for i1 = 1:obj.dimensions(1)
//     for i2 = 1:obj.dimensions(3)
//         s.type = '()';
//         s.subs = { i1 ':' i2 };
//         tmpdata = subsref(tmpMMO1.Data.x,s);
//         if all(dims == [2 1 3]), tmpMMO2.Data.x( :,i1,i2) = tmpdata; end
//         if all(dims == [2 3 1]), tmpMMO2.Data.x( :,i2,i1) = tmpdata; end
//         
//         if all(dims == [1 2 3]), tmpMMO2.Data.x(i1, :,i2) = tmpdata; end
//         if all(dims == [3 2 1]), tmpMMO2.Data.x(i2, :,i1) = tmpdata; end
//         
//         if all(dims == [1 3 2]), tmpMMO2.Data.x(i1,i2, :) = tmpdata; end
//         if all(dims == [3 1 2]), tmpMMO2.Data.x(i2,i1, :) = tmpdata; end
//     end
// end
// 
// for i1 = 1:obj.dimensions(2)
//     for i2 = 1:obj.dimensions(3)
//         s.type = '()';
//         s.subs = { ':' i1 i2 };
//         tmpdata = subsref(tmpMMO1.Data.x,s);
//         if all(dims == [1 2 3]), tmpMMO2.Data.x( :,i1,i2) = tmpdata; end
//         if all(dims == [1 3 2]), tmpMMO2.Data.x( :,i2,i1) = tmpdata; end
//         if all(dims == [2 1 3]), tmpMMO2.Data.x(i1, :,i2) = tmpdata; end
//         if all(dims == [2 3 1]), tmpMMO2.Data.x(i1,i2, :) = tmpdata; end
//         if all(dims == [3 2 1]), tmpMMO2.Data.x(i2,i1, :) = tmpdata; end
//         if all(dims == [3 1 2]), tmpMMO2.Data.x(i2, :,i1) = tmpdata; end
//     end
// end
return 0;
}
