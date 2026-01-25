// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./main.h"
#include "./mmo.h"
#include "../unknownTypes.h"

// Entry-point function
int main(void) {

	// Structure for cell arrays
	struct cell {
		int type;
		union {
			int ival;
			double dval;
			complex double cval;
			char chval[20];
		} data;
	};
	
	int ndim1 = 2;
	int dim1[2] = {1,3};
	Matrix * dim = createM(ndim1, dim1, 0);
	int *input1 = NULL;
	input1 = malloc( 3*sizeof(*input1));
	input1[0] = 64;
	input1[1] = 1024;
	input1[2] = 50;
	writeM( dim, 3, input1);
	free(input1);
	
	undefined mat1[] = {};
	if (null) {
		mmo tmpobj = obj2;
		obj2 = obj1;
		obj1 = tmpobj;
		
		
		
		clear;
		tmpobj;
	
	}
	struct generic_val s1;
	struct generic_val varargout1;
	struct generic_val s2;
	struct generic_val varargout2;
	bool tmp1 = isEqualM(size(obj1, &s1, &varargout1), size(obj2, &s2, &varargout2));
	struct generic_val s3;
	struct generic_val varargout3;
	if (~tmp1 && fabs(null - 1) > 1e-6) {
		
	
	}
	if (null) {
		
		} else {
		mmo data2 = obj2;
		
	
	}
	unknown newFileName = mmo.getnewfilename;
	int ndim2 = getnDimM(fopen);
	int *dim2 = getDimsM(fopen);
	struct generic_val tmp2;
	unknown_indexM(&tmp2, fopen_type, fopen, 1, ('w'-1) + (newFileName-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
	tmp2;
	struct generic_val s11;
	struct generic_val varargout4;
	unknown s1 = size(obj1, &s11, &varargout4);
	char ss.type[4] = '()';
	int ndim3 = getnDimM(ss.subs);
	int *dim3 = getDimsM(ss.subs);
	
	int ndim4 = 2;
	int dim4[2] = {1,3};
	Matrix * tmp3 = createM(ndim4, dim4, 3);
	char *input2 = NULL;
	input2 = malloc( 3*sizeof(*input2));
	input2[0][] = ":";
	writeM( tmp3, 3, input2);
	free(input2);
	
	lhs_data1[UNKNOWN] = null;
	// Write matrix mat2
	int size1 = 1;
	for (int iter1 = 0 ; iter1 < ndim4; iter1++)
	{
		size1 *= dim4[iter1];
	}
	Matrix *mat2 = createM(ndim4, dim4, 3);
	writeM(mat2, size1, lhs_data1);
	char* lhs_data4 = c_to_c(mat2);
	int ndim5 = getnDimM(s1);
	int *dim5 = getDimsM(s1);
	int idx1 = convertSubscript(ndim5, dim5, 1);
	unknown tmp4;
	indexM(s1, &tmp4, 1, idx1+1);
	for (int iter2 = 1; iter2 <= tmp4; iter2++) {
		int ndim6 = 2;
		int dim6[2] = {1,1};
		int tmp5 = getsizeM(s1);
		int idx2 = convertSubscript(ndim6, dim6, tmp5);
		mat2[idx2] = index;
		int * dim7 = getDimsM(data2);
		if (fabs(null - 1) < 1e-6) {
			int ndim7 = getnDimM(subsref);
			int *dim8 = getDimsM(subsref);
			struct generic_val tmp7;
			unknown_indexM(&tmp7, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim8 = getnDimM(f);
			int *dim9 = getDimsM(f);
			int ndim9 = getnDimM(subsref);
			int *dim10 = getDimsM(subsref);
			struct generic_val tmp9;
			unknown_indexM(&tmp9, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim10 = getnDimM(subsref);
			int *dim11 = getDimsM(subsref);
			struct generic_val tmp10;
			unknown_indexM(&tmp10, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			unknown tmp8;
			indexM(f, &tmp8, 2, tmp10, data2);
			tmp8;
			
			} else {
			int ndim11 = getnDimM(subsref);
			int *dim12 = getDimsM(subsref);
			struct generic_val tmp11;
			unknown_indexM(&tmp11, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim12 = getnDimM(subsref);
			int *dim13 = getDimsM(subsref);
			struct generic_val tmp12;
			unknown_indexM(&tmp12, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim13 = getnDimM(f);
			int *dim14 = getDimsM(f);
			int ndim14 = getnDimM(subsref);
			int *dim15 = getDimsM(subsref);
			struct generic_val tmp14;
			unknown_indexM(&tmp14, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim15 = getnDimM(subsref);
			int *dim16 = getDimsM(subsref);
			struct generic_val tmp15;
			unknown_indexM(&tmp15, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim16 = getnDimM(subsref);
			int *dim17 = getDimsM(subsref);
			struct generic_val tmp16;
			unknown_indexM(&tmp16, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim17 = getnDimM(subsref);
			int *dim18 = getDimsM(subsref);
			struct generic_val tmp17;
			unknown_indexM(&tmp17, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			unknown tmp13;
			indexM(f, &tmp13, 2, tmp16, tmp17);
			tmp13;
			
		
		}
		int ndim18 = getnDimM(fwrite);
		int *dim19 = getDimsM(fwrite);
		struct generic_val tmp18;
		unknown_indexM(&tmp18, fwrite_type, fwrite, 1, (tmpdata-1) + (fid-1)*1 + ('float'-1)*1*1 + (1-1)*1*1*1);
		tmp18;
	
	}
	int ndim19 = getnDimM(fclose);
	int *dim20 = getDimsM(fclose);
	int idx3 = convertSubscript(ndim19, dim19, fid);
	struct generic_val tmp19;
	unknown_indexM(&tmp19, fclose_type, fclose, 1, idx3+1);
	updateWorkspace(obj3);
	int ncopies = 0;
	int ndim20 = getnDimM(isstruct);
	int *dim21 = getDimsM(isstruct);
	int idx4 = convertSubscript(ndim20, dim20, arg);
	struct generic_val tmp20;
	unknown_indexM(&tmp20, isstruct_type, isstruct, 1, idx4+1);
	if (tmp20) {
		int tmp21 = getsizeM(arg);
		for (int iter3 = 1; iter3 <= tmp21; iter3++) {
				int ndim21 = 1;
				int dim22[1] = {1};
				int idx5 = convertSubscript(ndim21, dim21, 1);
				int ndim22 = getnDimM(arg(ilen).);
				int *dim23 = getDimsM(arg(ilen).);
				int ndim23 = 1;
				int dim24[1] = {1};
				int idx7 = convertSubscript(ndim23, dim23, 1);
				int idx6 = convertSubscript(ndim22, dim22, index[idx7]);
				unknown tmp22;
				indexM(arg(ilen)., &tmp22, 1, idx6+1);
				ncopies = ncopies + checkcopies_local(obj, tmp22);
				if (ncopies > 1) {
					return;
				
				}
			
			}
		
		}
		
		int ndim24 = getnDimM(iscell);
		int *dim25 = getDimsM(iscell);
		int idx8 = convertSubscript(ndim24, dim24, arg);
		struct generic_val tmp23;
		unknown_indexM(&tmp23, iscell_type, iscell, 1, idx8+1);
		} else if (tmp23) {
		int ndim25 = getnDimM(arg);
		int *dim26 = getDimsM(arg);
		int idx9 = convertSubscript(ndim25, dim25, undefined);
		unknown tmp24;
		indexM(arg, &tmp24, 1, idx9+1);
		int tmp25 = getsizeM(tmp24);
		for (int iter5 = 1; iter5 <= tmp25; iter5++) {
			int ndim26 = 2;
			int dim27[2] = {1,1};
			int idx10 = convertSubscript(ndim26, dim26, index);
			ncopies = ncopies + checkcopies_local_mmoint(obj, arg[idx10]);
			if (ncopies > 1) {
				return;
			
			}
		
		}
		
		bool tmp26 = isEqualM(obj, arg);
		} else if (null && tmp26) {
		ncopies = 1;
		
		} else {
		ncopies = 0;
		
	
	}
	if (fabs(nargin - 1) < 1e-6) {
		useconj = 1;
		
	
	}
	int tmp27 = getsizeM(obj.dimensions);
	int vec1[(1)*((int) floor((dimensions_dim0-3)/1) + 1)];
	
	for (int iter6 = 0; iter6 < (int) floor((dimensions_dim0-3)/1) + 1; iter6++) {
		vec1[iter6] = 3 + (1)*iter6;
	}
	int ndim27 = getnDimM(obj.dimensions);
	int *dim28 = getDimsM(obj.dimensions);
	struct generic_val tmp28;
	unknown_indexM(&tmp28, dimensions_type, obj.dimensions, 1, undefined);
	if (tmp27 <= 2 || null) {
		int res = 2;
		
		} else {
		int tmp29 = getsizeM(obj.dimensions);
		int res = tmp29;
		
	
	}
	int tmp30 = strcmpi(s(1).type, '.');
	if (tmp30) {
		
		return;
	
	}
	unknown subs = s(1).subs;
	int ndim28 = getnDimM(cellfun);
	int *dim29 = getDimsM(cellfun);
	struct generic_val tmp31;
	unknown_indexM(&tmp31, cellfun_type, cellfun, 1, (subs-1) + ('length'-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
	tmp31;
	int tmp32 = getsizeM(s);
	int tmp33 = strcmpi(s(1).type, '()');
	if (tmp32 > 1 || ~tmp33) {
		
	
	}
	if (obj.transposed) {
		int ndim29 = getnDimM(transposeindices);
		int *dim30 = getDimsM(transposeindices);
		struct generic_val tmp34;
		unknown_indexM(&tmp34, transposeindices_type, transposeindices, 1, (s-1) + (obj-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		tmp34;
		
	
	}
	int tmp35 = getsizeM(subs);
	int lastdim = tmp35;
	int ndim30 = 2;
	int dim31[2] = {1,1};
	int idx11 = convertSubscript(ndim30, dim30, 1);
	int ndim31 = getnDimM(ischar);
	int *dim32 = getDimsM(ischar);
	int ndim32 = 2;
	int dim33[2] = {1,1};
	int idx13 = convertSubscript(ndim32, dim32, 1);
	int idx12 = convertSubscript(ndim31, dim31, subs[idx13]);
	struct generic_val tmp36;
	unknown_indexM(&tmp36, ischar_type, ischar, 1, idx12+1);
	if (tmp36 && ndims(obj) > lastdim) {
		for (int iter7 = lastdim + 1; iter7 <= ndims(obj); iter7++) {
			int tmp37 = getsizeM(obj.dimensions);
			if (index > tmp37) {
				int ndim33 = 2;
				int dim34[2] = {1,1};
				int idx14 = convertSubscript(ndim33, dim33, index);
				subs[idx14] = 1;
				
				} else {
				int ndim34 = 2;
				int dim35[2] = {1,1};
				int idx15 = convertSubscript(ndim34, dim34, index);
				
			
			}
		
		}
		
	
	}
	int tmp38 = getsizeM(subs);
	for (int iter8 = 1; iter8 <= tmp38; iter8++) {
		int ndim35 = 2;
		int dim36[2] = {1,1};
		int idx16 = convertSubscript(ndim35, dim35, index);
		int ndim36 = getnDimM(ischar);
		int *dim37 = getDimsM(ischar);
		int ndim37 = 2;
		int dim38[2] = {1,1};
		int idx18 = convertSubscript(ndim37, dim37, index);
		int idx17 = convertSubscript(ndim36, dim36, subs[idx18]);
		struct generic_val tmp39;
		unknown_indexM(&tmp39, ischar_type, ischar, 1, idx17+1);
		if (tmp39) {
			int tmp40 = getsizeM(obj.dimensions);
			if (index > tmp40) {
				int ndim38 = 2;
				int dim39[2] = {1,1};
				int idx19 = convertSubscript(ndim38, dim38, index);
				subs[idx19] = 1;
				
				} else {
				int ndim39 = 2;
				int dim40[2] = {1,1};
				int idx20 = convertSubscript(ndim39, dim39, index);
				
			
			}
			// can only be ":"
			
		
		}
	
	}
	int ndim40 = getnDimM(cellfun);
	int *dim41 = getDimsM(cellfun);
	struct generic_val tmp41;
	unknown_indexM(&tmp41, cellfun_type, cellfun, 1, (subs-1) + (undefined-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
	tmp41;
	int ndim41 = getnDimM(finaldim);
	int *dim42 = getDimsM(finaldim);
	int vec2[(1)*((int) floor((finaldim_dim0-lastdim)/1) + 1)];
	
	for (int iter9 = 0; iter9 < (int) floor((finaldim_dim0-lastdim)/1) + 1; iter9++) {
		vec2[iter9] = lastdim + (1)*iter9;
	}
	int ndim42 = getnDimM(finaldim);
	int *dim43 = getDimsM(finaldim);
	struct generic_val tmp43;
	unknown_indexM(&tmp43, finaldim_type, finaldim, 1, undefined);
	lhs_data2[UNKNOWN] = tmp42;
	removeValue(finaldim, 0
	int ndim43 = 2;
	int dim44[2] = {1,1};
	int idx21 = convertSubscript(ndim43, dim43, undefined);
	int ndim44 = getnDimM(tmpMMO.data.x);
	int *dim45 = getDimsM(tmpMMO.data.x);
	int ndim45 = 2;
	int dim46[2] = {1,1};
	int idx23 = convertSubscript(ndim45, dim45, undefined);
	int idx22 = convertSubscript(ndim44, dim44, subs[idx23]);
	unknown tmp44;
	indexM(tmpMMO.data.x, &tmp44, 1, idx22+1);
	tmp44;
	int tmp45 = getsizeM(finaldim);
	if (tmp45 == 1) {
		int ndim46 = getnDimM(finaldim);
		int *dim47 = getDimsM(finaldim);
		int tmp46 = 1;
		lhs_data3[UNKNOWN] = tmp46;
		
	
	}
	int ndim47 = 2;
	int dim48[2] = {finaldim,finaldim};
	Matrix * tmp47 = reshapeM(res, ndim47, dim48);
	unknown res = tmp47;
	if (obj.transposed) {
		// Write matrix mat4
		int size3 = 1;
		for (int iter11 = 0 ; iter11 < ndim46; iter11++)
		{
			size3 *= dim47[iter11];
		}
		Matrix *mat4 = createM(ndim46, dim47, 0);
		writeM(mat4, size3, lhs_data3);
		int ndim48 = getnDimM(finaldim);
		int *dim49 = getDimsM(finaldim);
		int idx24 = convertSubscript(ndim48, dim48, (finaldim_dim0)*(finaldim_dim1));
		struct generic_val tmp48;
		unknown_indexM(&tmp48, finaldim_type, finaldim, 1, idx24+1);
		if (fabs(tmp48 - 1) < 1e-6) {
			removeValue(finaldim, 0
			
		
		}
		int tmp49 = getsizeM(finaldim);
		if (tmp49 <= 2) {
			Matrix * tmp50 = ctransposeM(res);
			res = tmp50;
			
			} else {
			int ndim50 = getnDimM(finaldim);
			int *dim51 = getDimsM(finaldim);
			int idx25 = convertSubscript(ndim50, dim50, 1);
			struct generic_val tmp51;
			unknown_indexM(&tmp51, finaldim_type, finaldim, 1, idx25+1);
			int ndim51 = getnDimM(finaldim);
			int *dim52 = getDimsM(finaldim);
			int idx26 = convertSubscript(ndim51, dim51, 2);
			struct generic_val tmp52;
			unknown_indexM(&tmp52, finaldim_type, finaldim, 1, idx26+1);
			int ndim52 = getnDimM(finaldim);
			int *dim53 = getDimsM(finaldim);
			int idx27 = convertSubscript(ndim52, dim52, 3);
			struct generic_val tmp53;
			unknown_indexM(&tmp53, finaldim_type, finaldim, 1, idx27+1);
			
			int ndim49 = 2;
			int dim50[2] = {1,2};
			Matrix * mat5 = createM(ndim49, dim50, 1);
			unknown *input3 = NULL;
			input3 = malloc( 2*sizeof(*input3));
			for (int iter12 = 0; iter12 < (1)*(undefined); iter12++) {
			   input3[0 + iter12] = tmp51 * tmp52[iter12];
			}
			input3[0(1)*(undefined)] = tmp53;
			writeM( mat5, 2, input3);
			free(input3);
			
			unknown * dim54 = u_to_u(mat5);
			int ndim53 = 2;
			Matrix * tmp54 = reshapeM(res, ndim53, dim54);
			Matrix * tmp55 = ctransposeM(tmp54);
			res = tmp55;
			int ndim55 = getnDimM(finaldim);
			int *dim56 = getDimsM(finaldim);
			int idx28 = convertSubscript(ndim55, dim55, 3);
			struct generic_val tmp56;
			unknown_indexM(&tmp56, finaldim_type, finaldim, 1, idx28+1);
			int ndim56 = getnDimM(finaldim);
			int *dim57 = getDimsM(finaldim);
			int idx29 = convertSubscript(ndim56, dim56, 1);
			struct generic_val tmp57;
			unknown_indexM(&tmp57, finaldim_type, finaldim, 1, idx29+1);
			int ndim57 = getnDimM(finaldim);
			int *dim58 = getDimsM(finaldim);
			int idx30 = convertSubscript(ndim57, dim57, 2);
			struct generic_val tmp58;
			unknown_indexM(&tmp58, finaldim_type, finaldim, 1, idx30+1);
			
			int ndim54 = 2;
			int dim55[2] = {1,3};
			Matrix * mat6 = createM(ndim54, dim55, 1);
			unknown *input4 = NULL;
			input4 = malloc( 3*sizeof(*input4));
			input4[0] = tmp56;
			input4[1] = tmp57;
			input4[2] = tmp58;
			writeM( mat6, 3, input4);
			free(input4);
			
			unknown * dim59 = u_to_u(mat6);
			int ndim58 = 2;
			Matrix * tmp59 = reshapeM(res, ndim58, dim59);
			res = tmp59;

			
		
		}

		
	
	}
	unknown newFileName = mmo.getnewfilename;
	int ndim59 = getnDimM(fopen);
	int *dim60 = getDimsM(fopen);
	struct generic_val tmp60;
	unknown_indexM(&tmp60, fopen_type, fopen, 1, ('w'-1) + (newFileName-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
	tmp60;
	int ndim60 = getnDimM(obj1.dimensions);
	int *dim61 = getDimsM(obj1.dimensions);
	int idx31 = convertSubscript(ndim60, dim60, (dimensions_dim0)*(dimensions_dim1));
	struct generic_val tmp61;
	unknown_indexM(&tmp61, dimensions_type, obj1.dimensions, 1, idx31+1);
	if (fabs(tmp61 - 1) < 1e-6) {
		removeValue(obj1.dimensions, 0
		
	
	}
	char ss.type[4] = '()';
	int ndim61 = getnDimM(mat2);
	int *dim62 = getDimsM(mat2);
	
	int ndim62 = 2;
	int dim63[2] = {1,3};
	Matrix * tmp62 = createM(ndim62, dim63, 3);
	char *input5 = NULL;
	input5 = malloc( 3*sizeof(*input5));
	input5[0][] = ":";
	writeM( tmp62, 3, input5);
	free(input5);
	
	lhs_data4[UNKNOWN] = null;
	// Write matrix mat7
	int size4 = 1;
	for (int iter13 = 0 ; iter13 < ndim62; iter13++)
	{
		size4 *= dim63[iter13];
	}
	Matrix *mat7 = createM(ndim62, dim63, 3);
	writeM(mat7, size4, lhs_data4);
	char* lhs_data6 = c_to_c(mat7);
	int ndim63 = getnDimM(obj1.dimensions);
	int *dim64 = getDimsM(obj1.dimensions);
	int idx32 = convertSubscript(ndim63, dim63, (dimensions_dim0)*(dimensions_dim1));
	struct generic_val tmp63;
	unknown_indexM(&tmp63, dimensions_type, obj1.dimensions, 1, idx32+1);
	for (int iter14 = 1; iter14 <= tmp63; iter14++) {
		int ndim64 = 2;
		int dim65[2] = {1,1};
		int tmp64 = getsizeM(obj1.dimensions);
		int idx33 = convertSubscript(ndim64, dim64, tmp64);
		mat7[idx33] = index;
		int ndim65 = getnDimM(subsref);
		int *dim66 = getDimsM(subsref);
		struct generic_val tmp65;
		unknown_indexM(&tmp65, subsref_type, subsref, 1, (ss-1) + (data.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		int ndim66 = 2;
		int dim67[2] = {1,1};
		int idx34 = convertSubscript(ndim66, dim66, undefined);
		int ndim67 = getnDimM(f);
		int *dim68 = getDimsM(f);
		int ndim68 = getnDimM(subsref);
		int *dim69 = getDimsM(subsref);
		struct generic_val tmp67;
		unknown_indexM(&tmp67, subsref_type, subsref, 1, (ss-1) + (data.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		int ndim69 = 2;
		int dim70[2] = {1,1};
		int idx35 = convertSubscript(ndim69, dim69, undefined);
		int ndim70 = getnDimM(subsref);
		int *dim71 = getDimsM(subsref);
		struct generic_val tmp68;
		unknown_indexM(&tmp68, subsref_type, subsref, 1, (ss-1) + (data.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		int ndim71 = 2;
		int dim72[2] = {1,1};
		int idx36 = convertSubscript(ndim71, dim71, undefined);
		unknown tmp66;
		indexM(f, &tmp66, 2, tmp68, varargin[idx36]);
		tmp66;
		int ndim72 = getnDimM(fwrite);
		int *dim73 = getDimsM(fwrite);
		struct generic_val tmp69;
		unknown_indexM(&tmp69, fwrite_type, fwrite, 1, (tmpdata-1) + (fid-1)*1 + ('float'-1)*1*1 + (1-1)*1*1*1);
		tmp69;
	
	}
	int ndim73 = getnDimM(fclose);
	int *dim74 = getDimsM(fclose);
	int idx37 = convertSubscript(ndim73, dim73, fid);
	struct generic_val tmp70;
	unknown_indexM(&tmp70, fclose_type, fclose, 1, idx37+1);
	updateWorkspace(obj2);
	if (nargin < 3) {
		struct generic_val writableVal;
		
	
	}
	if (nargin < 4) {
		struct generic_val transposedVal;
		
	
	}
	if (nargin < 5) {
		struct generic_val debugVal;
		
	
	}
	int ndim74 = getnDimM(isempty);
	int *dim75 = getDimsM(isempty);
	int idx38 = convertSubscript(ndim74, dim74, dataFileIn);
	struct generic_val tmp71;
	unknown_indexM(&tmp71, isempty_type, isempty, 1, idx38+1);
	if (~tmp71) {
		int ndim75 = getnDimM(ischar);
		int *dim76 = getDimsM(ischar);
		int idx39 = convertSubscript(ndim75, dim75, dataFileIn);
		struct generic_val tmp72;
		unknown_indexM(&tmp72, ischar_type, ischar, 1, idx39+1);
		if (~tmp72) {
			
		
		}
		int ndim76 = getnDimM(dir);
		int *dim77 = getDimsM(dir);
		int idx40 = convertSubscript(ndim76, dim76, dataFileIn);
		struct generic_val tmp73;
		unknown_indexM(&tmp73, dir_type, dir, 1, idx40+1);
		tmp73;
		int ndim77 = getnDimM(isempty);
		int *dim78 = getDimsM(isempty);
		int idx41 = convertSubscript(ndim77, dim77, dirContent);
		struct generic_val tmp74;
		unknown_indexM(&tmp74, isempty_type, isempty, 1, idx41+1);
		if (tmp74) {
			//error([ 'Data file ''' dataFileIn '''not found' ]);
			
			} else if (fabs(dirContent(1).bytes - 0) < 1e-6) {
			//error([ 'Empty data file ''' dataFileIn '''' ]);
			
		
		}
		
		
		
		} else {
		struct generic_val dataFileIn;
		int ndim78 = getnDimM(fopen);
		int *dim79 = getDimsM(fopen);
		struct generic_val tmp75;
		unknown_indexM(&tmp75, fopen_type, fopen, 1, ('w'-1) + (dataFileIn-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		tmp75;
		if (fabs(fid - -1) < 1e-6) {
			
		
		}
		int tmp76 = getsizeM(datadims);
		if (tmp76 == 1) {
			int ndim79 = getnDimM(datadims);
			int *dim80 = getDimsM(datadims);
			int tmp77 = 1;
			lhs_data5[UNKNOWN] = tmp77;
			
		
		}
		int vec3[(1)*((int) floor((datadims_dim0-2)/1) + 1)];
		
		for (int iter16 = 0; iter16 < (int) floor((datadims_dim0-2)/1) + 1; iter16++) {
			vec3[iter16] = 2 + (1)*iter16;
		}
		int ndim80 = getnDimM(datadims);
		int *dim81 = getDimsM(datadims);
		struct generic_val tmp78;
		unknown_indexM(&tmp78, datadims_type, datadims, 1, undefined);
		int ndim81 = 3;
		int dim82[3] = {1, , 'single'};
		Matrix * tmp79 = zerosM(ndim81, dim82);
		Matrix * tmpdata = tmp79;
		int ndim82 = getnDimM(datadims);
		int *dim83 = getDimsM(datadims);
		int idx42 = convertSubscript(ndim82, dim82, 1);
		struct generic_val tmp80;
		unknown_indexM(&tmp80, datadims_type, datadims, 1, idx42+1);
		for (int iter17 = 1; iter17 <= tmp80; iter17++) {
			// Write matrix mat8
			int size5 = 1;
			for (int iter15 = 0 ; iter15 < ndim79; iter15++)
			{
				size5 *= dim80[iter15];
			}
			Matrix *mat8 = createM(ndim79, dim80, 0);
			writeM(mat8, size5, lhs_data5);
			int ndim83 = getnDimM(fwrite);
			int *dim84 = getDimsM(fwrite);
			struct generic_val tmp81;
			unknown_indexM(&tmp81, fwrite_type, fwrite, 1, (tmpdata-1) + (fid-1)*1 + ('float'-1)*1*1 + (1-1)*1*1*1);
			tmp81;
		
		}
		int ndim84 = getnDimM(fclose);
		int *dim85 = getDimsM(fclose);
		int idx43 = convertSubscript(ndim84, dim84, fid);
		struct generic_val tmp82;
		unknown_indexM(&tmp82, fclose_type, fclose, 1, idx43+1);
		
		
		
		
		
		
		tmp82;
	
	}
	int ndim85 = getnDimM(datadims);
	int *dim86 = getDimsM(datadims);
	int idx44 = convertSubscript(ndim85, dim85, (datadims_dim0)*(datadims_dim1));
	struct generic_val tmp83;
	unknown_indexM(&tmp83, datadims_type, datadims, 1, idx44+1);
	bool tmp84 = fabs(tmp83 - 1) < 1e-6;
	int tmp85 = getsizeM(datadims);
	while (tmp84 && tmp85 > 1) {
		removeValue(datadims, 0
		
	
	}
	if (transposedVal) {
		int tmp86 = getsizeM(datadims);
		if (tmp86 == 1) {
			struct cell datadims[2];
			datadims[0].type = 0;
			datadims[0].data.ival = 1;
			
			
			} else {
			
		
		}
		
	
	}
	struct generic_val dataout.dataFile;
	unknown dataout.dimensions = datadims;
	struct generic_val dataout.writable;
	struct generic_val dataout.transposed;
	updateWorkspace(dataout);
	struct generic_val dataout.debug;
	checkworkspace(obj);
	if (ncopies < 2) {
		int ndim86 = getnDimM(evalin);
		int *dim87 = getDimsM(evalin);
		struct generic_val tmp87;
		unknown_indexM(&tmp87, evalin_type, evalin, 1, ('whos'-1) + ('caller'-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
		tmp87;
		int tmp88 = getsizeM(s);
		for (int iter18 = 1; iter18 <= tmp88; iter18++) {
			int tmp89 = strcmpi(s(index).class, 'struct');
			int tmp90 = strcmpi(s(index).class, 'cell');
			if (tmp89 || tmp90) {
				int ndim87 = getnDimM(evalin);
				int *dim88 = getDimsM(evalin);
				struct generic_val tmp91;
				unknown_indexM(&tmp91, evalin_type, evalin, 1, (s(index).name-1) + ('caller'-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
				tmp91;
				int ncopies = ncopies + checkcopies_local_mmounknown(obj, tmpVar);

				
				int tmp92 = strcmpi(s(index).class, 'mmo');
				} else if (tmp92) {
				if (s(index).persistent || s(index).global) {
					printf("\n%s\n", "Warning: mmo objects should not be made persistent or global. Behavior is unpredictable.");
					
				
				}
				int ndim88 = getnDimM(evalin);
				int *dim89 = getDimsM(evalin);
				struct generic_val tmp93;
				unknown_indexM(&tmp93, evalin_type, evalin, 1, (s(index).name-1) + ('caller'-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
				tmp93;
				bool tmp94 = isEqualM(tmpVar, obj);
				if (tmp94) {
					ncopies = ncopies + 1;
					
				
				}
				if (ncopies > 1) {
					
				
				}
				
				
				
				
			
			}
		
		}

		
	
	}
	if (nargin < 1) {
		struct generic_val dim;
		
	
	}
	struct generic_val s4;
	struct generic_val varargout5;
	double val = (double) sum(obj, dim) / size(obj, dim, &s4, &varargout5);
	binaryopp(, obj1, obj2);
	binaryopp(, obj1, obj2);
	int ndim89 = getnDimM(gettempfolder);
	int *dim90 = getDimsM(gettempfolder);
	int idx45 = convertSubscript(ndim89, dim89, true);
	struct generic_val tmp95;
	unknown_indexM(&tmp95, gettempfolder_type, gettempfolder, 1, idx45+1);
	int ndim90 = 2;
	int dim91[2] = {1,1};
	Matrix * tmp96 = randM(ndim90, dim91);
	int tmp97 = pow(10, 9);
	int scalar1 = tmp97;
	Matrix * tmp98 = scaleM(tmp96, &scalar1, 0);
	Matrix * tmp99 = roundM(tmp98);
	int ndim91 = 2;
	int dim92[2] = {1,1};
	Matrix * tmp100 = randM(ndim91, dim92);
	int tmp101 = pow(10, 9);
	int scalar2 = tmp101;
	Matrix * tmp102 = scaleM(tmp100, &scalar2, 0);
	Matrix * tmp103 = roundM(tmp102);
	sprintf(str, "memapdata_%.9d%.9d.fdt", tmp99, tmp103);
	char tmp104[50] = fullfile_toreplace(tmp95, );
	char str[50] = tmp104;
	int tmp105 = getsizeM(obj.dimensions);
	int vec4[(1)*((int) floor((dimensions_dim0-3)/1) + 1)];
	
	for (int iter19 = 0; iter19 < (int) floor((dimensions_dim0-3)/1) + 1; iter19++) {
		vec4[iter19] = 3 + (1)*iter19;
	}
	int ndim92 = getnDimM(obj.dimensions);
	int *dim93 = getDimsM(obj.dimensions);
	struct generic_val tmp106;
	unknown_indexM(&tmp106, dimensions_type, obj.dimensions, 1, undefined);
	if (tmp105 <= 2 || null) {
		res = 2;
		
		} else {
		int tmp107 = getsizeM(obj.dimensions);
		res = tmp107;
		
	
	}
	unknown stack = dbstack;
	removeValue(stack, 0
	heterogeneous obj.workspace = stack;
	unknown stack = dbstack;
	removeValue(stack, 0
	int ncopies = 0;
	int ndim93 = getnDimM(isempty);
	int *dim94 = getDimsM(isempty);
	int idx46 = convertSubscript(ndim93, dim93, stack);
	struct generic_val tmp108;
	unknown_indexM(&tmp108, isempty_type, isempty, 1, idx46+1);
	if (~tmp108) {
		bool tmp109 = isEqualM(stack, obj.workspace);
		if (~tmp109) {
			unknown subFlag = false;
			int tmp110 = getsizeM(obj.workspace);
			for (int iter20 = 1; iter20 <= tmp110; iter20++) {
				int ndim94 = getnDimM(obj.workspace);
				int *dim95 = getDimsM(obj.workspace);
				int idx47 = convertSubscript(ndim94, dim94, index);
				struct generic_val tmp111;
				unknown_indexM(&tmp111, workspace_type, obj.workspace, 1, idx47+1);
				int ndim95 = getnDimM(stack);
				int *dim96 = getDimsM(stack);
				int idx48 = convertSubscript(ndim95, dim95, 1);
				heterogeneous tmp112;
				indexM(stack, &tmp112, 1, idx48+1);
				bool tmp113 = isEqualM(tmp111, tmp112);
				if (tmp113) {
					unknown subFlag = true;
					int tmp114 = getsizeM(stack);
					if (tmp114 > 1) {
						int tmp115 = getsizeM(obj.workspace);
						for (int iter21 = 1; iter21 <= tmp115 - index; iter21++) {
							int tmp116 = getsizeM(stack);
							if (tmp116 < index2 + 1) {
								bool subFlag = false;
								
								int ndim96 = getnDimM(obj.workspace);
								int *dim97 = getDimsM(obj.workspace);
								int idx49 = convertSubscript(ndim96, dim96, index + index2);
								struct generic_val tmp117;
								unknown_indexM(&tmp117, workspace_type, obj.workspace, 1, idx49+1);
								int ndim97 = getnDimM(stack);
								int *dim98 = getDimsM(stack);
								int idx50 = convertSubscript(ndim97, dim97, index2 + 1);
								heterogeneous tmp118;
								indexM(stack, &tmp118, 1, idx50+1);
								bool tmp119 = isEqualM(tmp117, tmp118);
								} else if (~tmp119) {
								bool subFlag = false;
								
							
							}
						
						}
						
					
					}
					if (subFlag) {
						return;
					
					}
					
					
					
				
				}
			
			}
			int ndim98 = getnDimM(isempty);
			int *dim99 = getDimsM(isempty);
			int idx51 = convertSubscript(ndim98, dim98, obj.workspace);
			struct generic_val tmp120;
			unknown_indexM(&tmp120, isempty_type, isempty, 1, idx51+1);
			int tmp121 = strcmpi(stack(end).file, obj.workspace(end).file);
			if (~tmp120 && tmp121 && undefined) {
				int tmp122 = strcmpi(stack(end).name, obj.workspace(end).name);
				ncopies = 2;
				~tmp122;
				// We are within a subfunction. The MMO must have
				// been passed as an argument (otherwise the current
				// workspace and the workspace variable would be
				// equal).
				
				} else {
				int ndim99 = getnDimM(isscript);
				int *dim100 = getDimsM(isscript);
				int idx52 = convertSubscript(ndim99, dim99, stack(1).file);
				struct generic_val tmp123;
				unknown_indexM(&tmp123, isscript_type, isscript, 1, idx52+1);
				if (~tmp123) {
					ncopies = 2;
					
					// we are within a function. The MMO must have
					// been passed as an argument (otherwise the current
					// workspace and the workspace variable would be
					// equal).
					} else {
					int ndim100 = getnDimM(isempty);
					int *dim101 = getDimsM(isempty);
					int idx53 = convertSubscript(ndim100, dim100, stack);
					struct generic_val tmp124;
					unknown_indexM(&tmp124, isempty_type, isempty, 1, idx53+1);
					bool tmp125 = isEqualM(stack, obj.workspace);
					while (~tmp124 && ~tmp125) {
						removeValue(stack, 0
						
					
					}
					bool tmp126 = isEqualM(stack, obj.workspace);
					if (~tmp126) {
						ncopies = 2;
						
					
					}
					// we cannot be in a function with 0 argument
					// (otherwise the current workspace and the workspace
					// variable would be equal). We must assume that
					// we are in a script.
					
					
				
				}
				
			
			}
			// is the current stack a subset of the obj workspace?
			// if yes, it means that the object was created in a subfunction
			// and that it is OK to modify it (because it does not exist in the
			// subfunction any more)
			
			
			// if subfunction, must be a copie
			
		
		}
		// empty stack means the base workspace
		// check if we are in a different workspace
		
	
	}
	ncopies = 0;
	int ndim101 = getnDimM(isstruct);
	int *dim102 = getDimsM(isstruct);
	int idx54 = convertSubscript(ndim101, dim101, arg);
	struct generic_val tmp127;
	unknown_indexM(&tmp127, isstruct_type, isstruct, 1, idx54+1);
	if (tmp127) {
		int tmp128 = getsizeM(arg);
		for (int iter22 = 1; iter22 <= tmp128; iter22++) {
				int ndim102 = 1;
				int dim103[1] = {1};
				int idx55 = convertSubscript(ndim102, dim102, 1);
				int ndim103 = getnDimM(arg(ilen).);
				int *dim104 = getDimsM(arg(ilen).);
				int ndim104 = 1;
				int dim105[1] = {1};
				int idx57 = convertSubscript(ndim104, dim104, 1);
				int idx56 = convertSubscript(ndim103, dim103, index[idx57]);
				unknown tmp129;
				indexM(arg(ilen)., &tmp129, 1, idx56+1);
				ncopies = ncopies + checkcopies_local_mmounknown(obj, tmp129);
				if (ncopies > 1) {
					return;
				
				}
			
			}
		
		}
		
		int ndim105 = getnDimM(iscell);
		int *dim106 = getDimsM(iscell);
		int idx58 = convertSubscript(ndim105, dim105, arg);
		struct generic_val tmp130;
		unknown_indexM(&tmp130, iscell_type, iscell, 1, idx58+1);
		} else if (tmp130) {
		int ndim106 = getnDimM(arg);
		int *dim107 = getDimsM(arg);
		int idx59 = convertSubscript(ndim106, dim106, undefined);
		unknown tmp131;
		indexM(arg, &tmp131, 1, idx59+1);
		int tmp132 = getsizeM(tmp131);
		for (int iter24 = 1; iter24 <= tmp132; iter24++) {
			int ndim107 = 2;
			int dim108[2] = {1,1};
			int idx60 = convertSubscript(ndim107, dim107, index);
			ncopies = ncopies + checkcopies_local_mmoint(obj, arg[idx60]);
			if (ncopies > 1) {
				return;
			
			}
		
		}
		
		bool tmp133 = isEqualM(obj, arg);
		} else if (null && tmp133) {
		ncopies = 1;
		
		} else {
		ncopies = 0;
		
	
	}
	int ncopies = 0;
	int ndim108 = getnDimM(isstruct);
	int *dim109 = getDimsM(isstruct);
	int idx61 = convertSubscript(ndim108, dim108, arg);
	struct generic_val tmp134;
	unknown_indexM(&tmp134, isstruct_type, isstruct, 1, idx61+1);
	if (tmp134) {
		int tmp135 = getsizeM(arg);
		for (int iter25 = 1; iter25 <= tmp135; iter25++) {
				int ndim109 = 1;
				int dim110[1] = {1};
				int idx62 = convertSubscript(ndim109, dim109, 1);
				int ndim110 = getnDimM(arg(ilen).);
				int *dim111 = getDimsM(arg(ilen).);
				int ndim111 = 1;
				int dim112[1] = {1};
				int idx64 = convertSubscript(ndim111, dim111, 1);
				int idx63 = convertSubscript(ndim110, dim110, index[idx64]);
				unknown tmp136;
				indexM(arg(ilen)., &tmp136, 1, idx63+1);
				ncopies = ncopies + checkcopies_local_mmounknown(obj, tmp136);
				if (ncopies > 1) {
					return;
				
				}
			
			}
		
		}
		
		int ndim112 = getnDimM(iscell);
		int *dim113 = getDimsM(iscell);
		int idx65 = convertSubscript(ndim112, dim112, arg);
		struct generic_val tmp137;
		unknown_indexM(&tmp137, iscell_type, iscell, 1, idx65+1);
		} else if (tmp137) {
		int ndim113 = getnDimM(arg);
		int *dim114 = getDimsM(arg);
		int idx66 = convertSubscript(ndim113, dim113, undefined);
		unknown tmp138;
		indexM(arg, &tmp138, 1, idx66+1);
		int tmp139 = getsizeM(tmp138);
		for (int iter27 = 1; iter27 <= tmp139; iter27++) {
			int ndim114 = 2;
			int dim115[2] = {1,1};
			int idx67 = convertSubscript(ndim114, dim114, index);
			ncopies = ncopies + checkcopies_local_mmoint(obj, arg[idx67]);
			if (ncopies > 1) {
				return;
			
			}
		
		}
		
		bool tmp140 = isEqualM(obj, arg);
		} else if (null && tmp140) {
		ncopies = 1;
		
		} else {
		ncopies = 0;
		
	
	}
	if (obj.transposed) {
		int tmp141 = getsizeM(obj.dimensions);
		int tmp142 = getsizeM(obj.dimensions);
		if (tmp141 != 2 && tmp142 != 3) {
			
		
		}
		int tmp143 = getsizeM(obj.dimensions);
		if (tmp143 == 2) {
			
			} else {
			
		
		}

		
		} else {
		struct generic_val tmpdimensions;
		
	
	}
	struct generic_val s;
	if (nargin > 1) {
		int tmp144 = getsizeM(s);
		if (dim > tmp144) {
			struct generic_val s;
			
			} else {
			int ndim115 = getnDimM(s);
			int *dim116 = getDimsM(s);
			int idx68 = convertSubscript(ndim115, dim115, dim);
			struct generic_val tmp145;
			unknown_indexM(&tmp145, tmpdimensions_type, s, 1, idx68+1);
			tmp145;
			
		
		}
		
		} else {
		if (nargout > 1) {
			struct cell s[17];
			s[1].type = 0;
			s[1].data.ival = 1;
			s[2].type = 0;
			s[2].data.ival = 1;
			s[3].type = 0;
			s[3].data.ival = 1;
			s[4].type = 0;
			s[4].data.ival = 1;
			s[5].type = 0;
			s[5].data.ival = 1;
			s[6].type = 0;
			s[6].data.ival = 1;
			s[7].type = 0;
			s[7].data.ival = 1;
			s[8].type = 0;
			s[8].data.ival = 1;
			s[9].type = 0;
			s[9].data.ival = 1;
			s[10].type = 0;
			s[10].data.ival = 1;
			s[11].type = 0;
			s[11].data.ival = 1;
			s[12].type = 0;
			s[12].data.ival = 1;
			s[13].type = 0;
			s[13].data.ival = 1;
			s[14].type = 0;
			s[14].data.ival = 1;
			s[15].type = 0;
			s[15].data.ival = 1;
			s[16].type = 0;
			s[16].data.ival = 1;
			
			struct generic_val alls;
			int ndim116 = getnDimM(s);
			int *dim117 = getDimsM(s);
			int idx69 = convertSubscript(ndim116, dim116, 1);
			struct generic_val tmp146;
			unknown_indexM(&tmp146, tmpdimensions_type, s, 1, idx69+1);
			tmp146;
			Matrix * tmp147 = maxM(nargout, 1);
			nargout_type * tmp148 = n_to_n(tmp147);
			for (int iter28 = 1; iter28 <= tmp148[0] - 1; iter28++) {
				int ndim117 = 2;
				int dim118[2] = {varargout_dim0,varargout_dim1};
				int idx70 = convertSubscript(ndim117, dim117, index);
				int ndim118 = getnDimM(alls);
				int *dim119 = getDimsM(alls);
				int idx71 = convertSubscript(ndim118, dim118, index + 1);
				struct generic_val tmp149;
				unknown_indexM(&tmp149, alls_type, alls, 1, idx71+1);
				tmp149;
			
			}
			
			
			
			
		
		}
		
	
	}
	*p_[s varargout] = [s varargout];
	if (null) {
		tmpobj = obj2;
		obj2 = obj1;
		obj1 = tmpobj;
		
		
		
		clear;
		tmpobj;
	
	}
	struct generic_val s5;
	struct generic_val varargout6;
	struct generic_val s6;
	struct generic_val varargout7;
	bool tmp150 = isEqualM(size(obj1, &s5, &varargout6), size(obj2, &s6, &varargout7));
	struct generic_val s7;
	struct generic_val varargout8;
	if (~tmp150 && fabs(null - 1) > 1e-6) {
		
	
	}
	if (null) {
		
		} else {
		data2 = obj2;
		
	
	}
	newFileName = mmo.getnewfilename;
	int ndim119 = getnDimM(fopen);
	int *dim120 = getDimsM(fopen);
	struct generic_val tmp151;
	unknown_indexM(&tmp151, fopen_type, fopen, 1, ('w'-1) + (newFileName-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
	tmp151;
	struct generic_val s12;
	struct generic_val varargout9;
	s1 = size(obj1, &s12, &varargout9);
	ss.type = '()';
	int ndim120 = getnDimM(mat7);
	int *dim121 = getDimsM(mat7);
	
	int ndim121 = 2;
	int dim122[2] = {1,3};
	Matrix * tmp152 = createM(ndim121, dim122, 3);
	char *input6 = NULL;
	input6 = malloc( 3*sizeof(*input6));
	input6[0][] = ":";
	writeM( tmp152, 3, input6);
	free(input6);
	
	lhs_data6[UNKNOWN] = null;
	// Write matrix mat9
	int size6 = 1;
	for (int iter29 = 0 ; iter29 < ndim121; iter29++)
	{
		size6 *= dim122[iter29];
	}
	Matrix *mat9 = createM(ndim121, dim122, 3);
	writeM(mat9, size6, lhs_data6);
	int ndim122 = getnDimM(s1);
	int *dim123 = getDimsM(s1);
	int idx72 = convertSubscript(ndim122, dim122, 1);
	unknown tmp153;
	indexM(s1, &tmp153, 1, idx72+1);
	for (int iter30 = 1; iter30 <= tmp153; iter30++) {
		int ndim123 = 2;
		int dim124[2] = {1,1};
		int tmp154 = getsizeM(s1);
		int idx73 = convertSubscript(ndim123, dim123, tmp154);
		mat7[idx73] = index;
		int * dim125 = getDimsM(data2);
		if (fabs(null - 1) < 1e-6) {
			int ndim124 = getnDimM(subsref);
			int *dim126 = getDimsM(subsref);
			struct generic_val tmp156;
			unknown_indexM(&tmp156, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim125 = getnDimM(f);
			int *dim127 = getDimsM(f);
			int ndim126 = getnDimM(subsref);
			int *dim128 = getDimsM(subsref);
			struct generic_val tmp158;
			unknown_indexM(&tmp158, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim127 = getnDimM(subsref);
			int *dim129 = getDimsM(subsref);
			struct generic_val tmp159;
			unknown_indexM(&tmp159, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			unknown tmp157;
			indexM(f, &tmp157, 2, tmp159, data2);
			tmp157;
			
			} else {
			int ndim128 = getnDimM(subsref);
			int *dim130 = getDimsM(subsref);
			struct generic_val tmp160;
			unknown_indexM(&tmp160, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim129 = getnDimM(subsref);
			int *dim131 = getDimsM(subsref);
			struct generic_val tmp161;
			unknown_indexM(&tmp161, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim130 = getnDimM(f);
			int *dim132 = getDimsM(f);
			int ndim131 = getnDimM(subsref);
			int *dim133 = getDimsM(subsref);
			struct generic_val tmp163;
			unknown_indexM(&tmp163, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim132 = getnDimM(subsref);
			int *dim134 = getDimsM(subsref);
			struct generic_val tmp164;
			unknown_indexM(&tmp164, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim133 = getnDimM(subsref);
			int *dim135 = getDimsM(subsref);
			struct generic_val tmp165;
			unknown_indexM(&tmp165, subsref_type, subsref, 1, (ss-1) + (data1.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			int ndim134 = getnDimM(subsref);
			int *dim136 = getDimsM(subsref);
			struct generic_val tmp166;
			unknown_indexM(&tmp166, subsref_type, subsref, 1, (ss-1) + (data2.Data.x-1)*1 + (1-1)*1*1 + (1-1)*1*1*1);
			unknown tmp162;
			indexM(f, &tmp162, 2, tmp165, tmp166);
			tmp162;
			
		
		}
		int ndim135 = getnDimM(fwrite);
		int *dim137 = getDimsM(fwrite);
		struct generic_val tmp167;
		unknown_indexM(&tmp167, fwrite_type, fwrite, 1, (tmpdata-1) + (fid-1)*1 + ('float'-1)*1*1 + (1-1)*1*1*1);
		tmp167;
	
	}
	int ndim136 = getnDimM(fclose);
	int *dim138 = getDimsM(fclose);
	int idx74 = convertSubscript(ndim136, dim136, fid);
	struct generic_val tmp168;
	unknown_indexM(&tmp168, fclose_type, fclose, 1, idx74+1);
	updateWorkspace(obj3);
	*p_[s varargout] = [s varargout];
	stack = dbstack;
	removeValue(stack, 0
	obj.workspace = stack;
	*p_[s varargout] = [s varargout];
	return 0;
}
