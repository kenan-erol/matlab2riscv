struct mmo {
	struct generic_val dataFile;
	struct generic_val dimensions;
	bool writable;
	struct generic_val workspace;
	char *type = NULL;
type = malloc(5*sizeof(*type));
	unknown transposed;
	unknown debug;
}

mmo mmo_binaryopp(unknown f, mmo obj1, mmo obj2) {




// make new memory mapped data file (blank)
// --------------------------------






tmp19;
// create object
// -------------


return obj3;
}

int mmo_checkcopies_local(mmo obj, int arg) {


return ncopies;
}

res_type mmo_ctranspose(mmo obj, int useconj) {
return res;
}

res_type mmo_ndims(mmo obj) {

return res;
}

unknown mmo_subsref(mmo obj, ss_type s) {




// one dimension input
// -------------------

// deal with transposed data
// -------------------------

// convert : to real sizes
// -----------------------






// non-transposed data
// -------------------




return res;
}

mmo mmo_unitaryopp(unknown f, mmo obj1, unknown varargin) {
// make new memory mapped data file (blank)
// ----------------------------------------







tmp70;
// copy the data
// -------------


return obj2;
}

mmo mmo_mmo(dataFileIn_type dataFileIn, unknown datadims, writableVal_type writableVal, transposedVal_type transposedVal, debugVal_type debugVal) {



// check that the file is not empty

// --------------------------------


// test memory map but discards it

// -------------------------------


clear;
test;
// set fields

// ----------







// set workspace

// -------------


//             stack = dbstack;

//             stack(1) = [];

//             stack = rmfield(stack, 'line');

//             dataout.workspace = stack;


return dataout;
}

int mmo_checkcopies(mmo obj) {


return ncopies;
}

double mmo_mean(mmo obj, dim_type dim) {



return val;
}

mmo mmo_minus(mmo obj1, mmo obj2) {

return obj3;
}

mmo mmo_time(mmo obj1, mmo obj2) {

return obj3;
}

char mmo_getnewfilename(void) {

return str;
}

res_type mmo_ndims(mmo obj) {

return res;
}

mmo mmo_updateWorkspace(mmo obj) {




return obj;
}

int mmo_checkworkspace(mmo obj) {





return ncopies;
}

int mmo_checkcopies_local_mmounknown(mmo obj, int arg) {


return ncopies;
}

int mmo_checkcopies_local_mmoint(mmo obj, int arg) {


return ncopies;
}

void mmo_size(mmo obj, dim_type dim, tmpdimensions_type* p_[s varargout]) {



return ncopies;
}

mmo mmo_binaryopp(unknown f, mmo obj1, mmo obj2) {




// make new memory mapped data file (blank)
// --------------------------------






tmp168;
// create object
// -------------


return obj3;
}

mmo mmo_updateWorkspace(mmo obj) {




return obj;
}