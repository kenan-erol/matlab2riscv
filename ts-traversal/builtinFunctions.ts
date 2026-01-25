import {Type, VarType} from "./typeInference";
import {Alias} from "./customTypes";
import {numel} from "./helperFunctions";
import {binaryOpType} from "./customTypes";
//import {SyntaxNode} from "./generated";
    
type typeToMatrixType = {
  type: string;
  matrix_type: number;
};
    
const type_to_matrix_type: typeToMatrixType[] = [
    {type: "integer", matrix_type: 0},
    {type: "int", matrix_type: 0},
    {type: "double", matrix_type: 1},
    {type: "complex", matrix_type: 2},
    {type: "char", matrix_type: 3}
];

type InitVar = {
    name: string;
    val: string;
    type: string;
    ndim: number;
    dim: Array<number>;
    ismatrix: boolean;
    isvector: boolean;
    ispointer: boolean;
    isstruct: boolean;
};

type functionMapping = {
    fun_matlab: string;
    fun_c: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>, fun_matlab: string): string; };
    req_arg_types: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<Type>; }; //arg types required by C function
    args_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; };
    arg_types_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<VarType>; }; //if MATLAB function arguments must have specific type, used to help determine unknown types
    outs_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; 
    n_req_args: number; // # required args
    n_opt_args: number; // # optional args
    opt_arg_defaults: Array<string>;
    ptr_args: { (arg_types: Array<Type>, outs: Array<string>): Array<VarType>; };
    return_type: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Type; };
    push_main_before: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; // push to main before function call 
    push_main_after: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<string>; }; // push to main after function call
    init_before: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<InitVar>; }; // vars to initialize before function call 
    tmp_out_transform: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<InitVar>; }; // vars to initialize before function call 
    push_alias_tbl: { (args: Array<string>, arg_types: Array<Type>, outs: Array<string>): Array<Alias>; }; // vars to initialize before function call 
};

function parseCharArg(arg:string) {
    let regex = /(?<=')(.*?)(?=')|(?<=")(.*?)(?=")/;
    let match = arg.match(regex);
    if (match != null) {
        return match[0];
    } else {
        return arg;
    }
}

function parseVectorArg(arg:string) {
    let regex = /(?<=\[)(.*?)(?=\])/;
    let match1 = arg.match(regex);
    if (match1 != null) {
        let vec_str = match1[0];
        let regex2 = /".*?"|[^,; ]*/g;
        let match2 = vec_str.match(regex2);
        let vec_elements = [];
        if (match2 != null) {
            for (let element of match2) {
                if (element != "") {
                    vec_elements.push(parseCharArg(element));
                }
            }
        }
        //return [`{${vec_str}}`, vec_elements];
        return vec_elements;
    } else {
        //return [arg, []];
        return [];
    }
}
    
export const operatorMapping = [
    { // Matrix * plusM(Matrix *m, Matrix *n)
        //fun_matlab: '+',
        fun_matlab: /\+|\-/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (args.length == 1) {
                return null;
            }
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                if (fun_matlab == "+") {
                    return 'plusM'; 
                } else {
                    return 'minusM';
                }
            } else {
                return null;
            }
        }, 
        req_arg_types: (args, arg_types, outs) => {
            if (args.length == 1) {
                return null;
            }
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if ((left_ismatrix && !right_ismatrix) ||  (!left_ismatrix && right_ismatrix)) {
                return [
                    {
        		        type: null,
        		        ismatrix: false,
        		        isvector: false,
        		        ispointer: false,
                        isstruct: false
		            },
                    {
        		        type: null,
        		        ismatrix: false,
        		        isvector: false,
        		        ispointer: false,
                        isstruct: false
		            }
                ];
            } else {
                return null;
            }
        },
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
        outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            let ndim = arg_types[0].ndim;
            let dim = arg_types[0].dim;
            let ismatrix = arg_types[0].ismatrix;
            
            if (args.length == 2) {
                type = binaryOpType(type, arg_types[1].type);
            }

            return {
                type: type, // create function to get types giving precedence to complex, double, then int
                ndim: ndim,
                dim: dim,
                ismatrix: ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * scaleM(Matrix* restrict m, void* restrict scalar, int type)
      // Matrix * mtimesM(Matrix *m, Matrix *n)
        fun_matlab: '*', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return 'mtimesM';
            } else if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
                return 'scaleM';
            } else {
                return null;
            }
        },
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;

            if (left_ismatrix && !right_ismatrix) {
                let obj = type_to_matrix_type.find(x => x.type === arg_types[1].type);
                return [args[0], "&scalar", obj.matrix_type.toString()];
                
            } else if (!left_ismatrix && right_ismatrix) {
                let obj = type_to_matrix_type.find(x => x.type === arg_types[0].type);
                return [args[1], "&scalar", obj.matrix_type.toString()];
                
            } else {
                return args;
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return {
                    type: binaryOpType(left_type, right_type), 
                    ndim: left_ndim,
                    dim: left_dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            } else {
                return {
                    type: binaryOpType(left_type, right_type),
                    ndim: left_ndim,
                    dim: [left_dim[0], right_dim[1]],
                    ismatrix: left_ismatrix || right_ismatrix,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let left_type = arg_types[0].type;
            let right_ismatrix = arg_types[1].ismatrix;
            let right_type = arg_types[1].type;
            if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
                let init_var: InitVar[] = [];
                let val = args[0];
                let type = left_type;
                if (left_ismatrix) {
                    val = args[1];
                    type = right_type;
                }
                init_var.push({
                    name: 'scalar',
                    val: val,
                    type: type,
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                return init_var;
            } else {
                return null;
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * mrdivideM(Matrix *m, Matrix *n)
        fun_matlab: '/', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return 'mrdivideM';
            } else if (left_ismatrix && !right_ismatrix) {
                return 'scaleM';
            } else {
                return null;
            }
        },
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return args;
            } else if (left_ismatrix && !right_ismatrix) {
                let left_type = arg_types[0].type;
                let right_type = arg_types[1].type;
                let type = binaryOpType(left_type, right_type);
                let obj = type_to_matrix_type.find(x => x.type === type);
                return [args[0], `1/(${args[1]})`, obj.matrix_type.toString()];
            } else {
                if (arg_types[0].type == "complex" && arg_types[1].type != "complex") {
                    return [`(${args[0]})`, `((complex) ${args[1]})`];
                } else if (arg_types[0].type != "complex" && arg_types[1].type == "complex") {
                    return [`((complex) ${args[0]})`, `(${args[1]})`];
                } else if (arg_types[1].type == "complex") {
                    return [args[0], `(${args[1]})`];
                }
                return [`(double) ${args[0]}`, args[1]];
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            let type = binaryOpType(left_type, right_type);
            if (left_type == 'complex' || right_type == 'complex') {
                type = 'complex';
            } else {
                type = 'double';
            }
            
            return {
                type: type, // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: [left_dim[0], right_dim[1]],
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * mldivideM(Matrix *m, Matrix *n)
        fun_matlab: '\\', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return 'mldivideM';
            } else if (!left_ismatrix && right_ismatrix) {
                return 'scaleM';
            } else {
                return null;
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && right_ismatrix) {
                return args;
            } else {
                let left_type = arg_types[0].type;
                let right_type = arg_types[1].type;
                let type = binaryOpType(left_type, right_type);
                let obj = type_to_matrix_type.find(x => x.type === type);
                return [`1/(${args[0]})`, args[1], obj.matrix_type.toString()];
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: [left_dim[0], right_dim[1]],
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * mpowerM(Matrix *m, void *scalar, int type)
        fun_matlab: '^', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let left_type = arg_types[0].type;
            let right_ismatrix = arg_types[1].ismatrix;
            let right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                return 'mpowerM';
            } else {
                if (left_type == 'complex' || right_type != 'int') {
                    return 'cpow';
                } else {
                    return 'pow';
                }
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let left_type = arg_types[0].type;
            let right_ismatrix = arg_types[1].ismatrix;
            let right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                /*let type = left_type;
                if (right_type != 'int') {
                    type = 'complex';
                } else {
                    if (args[1][0] == "-") {
                        type = 'complex';
                    }
                }*/
                
                let type = right_type;
                let obj = type_to_matrix_type.find(x => x.type === type);
                
                /*let isnum = /^[\d.]+[\+\-][\d.]+\*I$/.test(args[1]);
                if (!isnum) {
                    isnum = /^[\d.]+$/.test(args[1]);
                }*/
                let isnum = !/^[a-z].*$/.test(args[1]);
                if (isnum) {
                    return [args[0], '&exponent', obj.matrix_type.toString()];
                } else {
                    return [args[0], `&${args[1]}`, obj.matrix_type.toString()];
                }
            }
            
            return args;
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let right_ismatrix = arg_types[1].ismatrix;
            /*let type = binaryOpType(left_type, right_type);
            if (left_type == 'complex' || right_type != 'int') {
                type = 'complex';
            }*/
            let type = left_type;
            if (right_type != 'int') {
                type = 'complex';
            }
            
            return {
                type: type, // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix && !right_ismatrix,
                isvector: false,
                ispointer: false, //left_ismatrix && !right_ismatrix,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && !right_ismatrix) {
                let isnum = !/^[a-z].*$/.test(args[1]);
                if (isnum) {
                    let init_var: InitVar[] = [];
                    init_var.push({
                        name: 'exponent',
                        val: `${args[1]}`,
                        type: right_type,
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * timesM(Matrix *m, Matrix *n)
        fun_matlab: '.*', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'timesM';
            } else {
                return null;
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * rdivideM(Matrix *m, Matrix *n)
        fun_matlab: './', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'rdivideM';
            } else {
                return null;
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * ldivideM(Matrix *m, Matrix *n)
        fun_matlab: '.\\', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'ldivideM';
            } else {
                return null;
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * powerM(Matrix *m, Matrix *n)
      // Matrix * scalarpowerM(Matrix* restrict m, void* restrict exponent, int type)
        fun_matlab: '.^', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            /*if (!left_ismatrix && !right_ismatrix) {
                return null;
            } else {
                return 'powerM';
            }*/
            if (left_ismatrix && right_ismatrix) {
                return 'powerM';
            } if (left_ismatrix && !right_ismatrix) {
                return 'scalarpowerM';
            } else {
                return null;
            }
        },   
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            let left_type = arg_types[0].type;
            let right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                /*let type = left_type;
                if (right_type != 'int') {
                    type = 'complex';
                }*/
                let type = right_type; 
                
                //let type = binaryOpType(left_type, right_type);
                let obj = type_to_matrix_type.find(x => x.type === type);
                let isnum = /^\d+$/.test(args[1]);
                if (isnum) {
                    return [args[0], 'scalar', `${obj.matrix_type}`];
                } else {
                    return [args[0], `&${args[1]}`, `${obj.matrix_type}`];
                }
            } else {
                return args;
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            let type = left_type;
            if (right_type != 'int') {
                type = 'complex';
            }
            return {
                //type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                type: type,
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let right_ismatrix = arg_types[1].ismatrix;
            
            if (left_ismatrix && !right_ismatrix) {
                let isnum = /^\d+$/.test(args[1]);
                if (isnum) {
                    let init_var: InitVar[] = [];
                    init_var.push({
                        name: 'scalar',
                        val: `${args[1]}`,
                        type: right_type,
                        //ndim: right_ndim,
                        //dim: [right_ndim],
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * ltM(Matrix *m, Matrix *n)
        fun_matlab: /(<=)|(>=)|<|>/, 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            if (left_ismatrix) {
                if (fun_matlab == "<") {
                    return 'matlab_ltM';
                } else if (fun_matlab == "<=") {
                    return 'matlab_leM';
                } else if (fun_matlab == ">") {
                    return 'matlab_gtM';
                } else if (fun_matlab == ">=") {
                    return 'matlab_geM';
                }
            } else {
                return null;
            }
        },  
        req_arg_types: (args, arg_types, outs) => {
            if (args.length == 1) {
                return null;
            }
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if ((left_ismatrix && !right_ismatrix) ||  (!left_ismatrix && right_ismatrix)) {
                return [
                    {
        		        type: null,
        		        ismatrix: false,
        		        isvector: false,
        		        ispointer: false,
                        isstruct: false
		            },
                    {
        		        type: null,
        		        ismatrix: false,
        		        isvector: false,
        		        ispointer: false,
                        isstruct: false
		            }
                ];
            } else {
                return null;
            }
        },
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                return [args[0], 'tmp'];
            } else {
                return args;
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: 'bool', // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                let dim = `{${arg_types[0].dim}}`;
                let ndim = arg_types[0].ndim;
                let init_var: InitVar[] = [];
                init_var.push({
                    name: 'ndim',
                    val: `${ndim}`,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'dim',
                    val: dim,
                    type: 'int',
                    ndim: 1,
                    dim: [ndim],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'scalar',
                    val: args[1],
                    type: arg_types[1].type,
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                let obj = type_to_matrix_type.find(x => x.type === arg_types[1].type);
                init_var.push({
                    name: 'tmp',
                    val: `scaleM(onesM(ndim, dim), &scalar, ${obj.matrix_type})`,
                    type: 'int',
                    ndim: ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                return init_var;
            } else {
                return null;
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * equalM(Matrix *m, Matrix *n)
        fun_matlab: '==', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'equalM';
            } else {
                if (arg_types[0].type == "int" && arg_types[1].type == "int") {
                    return `${args[0]} == ${args[1]}`;
                } else {
                    return `fabs(${args[0]} - ${args[1]}) < 1e-6`;
                }
            }
        },   
        req_arg_types: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                return [
                    {
        		        type: null,
        		        ismatrix: false,
        		        isvector: false,
        		        ispointer: false,
                        isstruct: false
		            }
                ];
            } else {
                return null;
            }
        },
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            } else {
                return ['void'];
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: 'bool', // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
            
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * neM(Matrix *m, Matrix *n)
        fun_matlab: '~=', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'neM';
            } else {
                if (arg_types[0].type == "int" && arg_types[1].type == "int") {
                    return `${args[0]} != ${args[1]}`;
                } else {
                    return `fabs(${args[0]} - ${args[1]}) > 1e-6`;
                }
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            } else {
                return ['void'];
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;

            return {
                type: 'bool', // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * andM(Matrix *m, Matrix *n)
        fun_matlab: /\&|\|/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;

            if (left_ismatrix && right_ismatrix) {
                if (fun_matlab == "&") {
                    return 'andM';
                } else {
                    return 'orM';
                }
            } else {
                return null;
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            let left_ismatrix = arg_types[0].ismatrix;
            let right_ismatrix = arg_types[1].ismatrix;
            
            return {
                type: 'bool', // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { 
        fun_matlab: /\&\&|\|\|/,
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: false,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * notM(Matrix* restrict m)
        fun_matlab: '~', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let ismatrix = arg_types[0].ismatrix;
            if (ismatrix) {
                return 'notM';
            } else {
                return null;
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            let ndim = arg_types[0].ndim;
            let dim = arg_types[0].dim;
            
            return {
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: arg_types[0].ismatrix,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * ctransposeM(Matrix* restrict m)
        fun_matlab: /'|\.'/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let ismatrix = arg_types[0].ismatrix;
            if (ismatrix) {
                if (fun_matlab == "'") {
                    return 'ctransposeM';
                } else {
                    return 'transposeM';
                }
            } else {
                return null;
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix == null) {
                return [{
                    name: args[0],
                    type: arg_types[0].type,
                    ndim: arg_types[0].ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false,
                    initialized: false,
                    scope: null
                }];
            }
            return null;
        },
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            let ndim = arg_types[0].ndim;
            let dim = arg_types[0].dim;
            
            return {
                type: type,
                ndim: ndim,
                dim: [dim[1], dim[0]],
                ismatrix: arg_types[0].ismatrix, //true
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    }
];


export const builtin_functions = [
    { // bool isEqualM(Matrix *m, Matrix *n)
        fun_matlab: 'isequal', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'isEqualM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'bool',
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * sinM(Matrix *m);
        fun_matlab: /sin|cos|tan/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (arg_types[0].ismatrix) {
                return 'fun_matlabM';
            } else {
                return 'fun_matlab';
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                return {
                    type: "double",
                    ndim: arg_types[0].ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false 
                };
            } else {
                return {
                    type: "double",
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false 
                };
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * sindM(Matrix *m);
        fun_matlab: /sind|cosd|tand/, 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (arg_types[0].ismatrix) {
                return 'fun_matlabM';
            } else {
                return 'fun_matlab';
            }
        },  
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                return args;
            } else {
                return [`fmod((${args[0]}),360) * M_PI / 180`];
            }
        },
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                return {
                    type: "double",
                    ndim: arg_types[0].ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false 
                };
            } else {
                return {
                    type: "double",
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false 
                };
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // TO DO: Matrix * xcorrM(Matrix *x, Matrix *y, int maxlag, char *scale)
        fun_matlab: 'xcorr', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'xcorrM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let x = args[0];
            let y = null;
            let maxlag = 0;
            let scale = "none";
            
            let scale_opts = ["none", "biased", "unbiased", "normalized", "coeff"];
            let match = undefined;
            let i = 0;
            while (match == undefined && i < scale_opts.length ) {
                match = args.find(x => x.includes(scale_opts[i]));
                i++;
            }
            if (match != null && match != undefined) {
                if (match == "normalized") {
                    scale = "coeff";
                } else {
                    scale = match.replace(/'/g, '"');
                }
            }
            
            if (args.indexOf(match) == 3) {
                y = args[1];
                maxlag = args[2];
            } else if (args.indexOf(match) == 2) {
                y = args[1];
            }
            
            return [x, y, maxlag, scale];
        },
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 4,
        n_opt_args: 3,
        //opt_arg_defaults: ['null', '0', 'none'],
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * sortM(Matrix* restrict m, int direction)
        fun_matlab: 'sort', // sort(m, dim, direction)
        fun_c: (args, arg_types, outs, fun_matlab) => 'sortM', // sortM(m, direction) -> update c function to accept dim
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: args => {
            let args_transformed = [];
            for (let i=0; i < args.length; i++) {
                if (i == 0) {
                    args_transformed.push(args[i]);
                } else {
                    if (parseCharArg(args[i]) == 'ascend') {
                        args_transformed.push('0');
                    } else if (parseCharArg(args[i]) == 'descend') {
                        args_transformed.push('1');
                    }
                }
            }
            return args_transformed;
        },
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['0'],
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            let ndim = arg_types[0].ndim;
            let dim = arg_types[0].dim;
            
            return {
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void ttestM(Matrix* restrict m, double mu, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict tstat, double* restrict df, double* restrict sd)
      // [h,p,ci,stats] = ttest(___)  
		fun_matlab: 'ttest', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (arg_types.length >= 2) {
                if (arg_types[1].ismatrix) {
                    return 'ttestM_xy';
                }
            }
            return 'ttestM';
        },
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => null,
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['0'],
        ptr_args: (arg_types, outs) => {
            var arg_h = 'h';
            var arg_pval = 'pval';
            var arg_ci = 'ci';
			if (outs.length >= 1) {
			    arg_h = outs[0];
			}
			if (outs.length >= 2) {
			    arg_pval = outs[1];
			}
			if (outs.length >= 3) {
				arg_ci = outs[2];
			} 
			return [
			    {
			        name: arg_h,
			        type: 'bool',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true
			    },
			    {
			        name: arg_pval,
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true
			    },
			    {
			        name: arg_ci,
			        type: 'double',
			        ndim: 2,
			        dim: [1,2],
			        ismatrix: false,
			        isvector: true,
			        ispointer: true
			    },
			    {
			        name: 'tstat',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true
			    },
			    {
			        name: 'df',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true
			    },
			    {
			        name: 'sd',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true
			    },
			];
		},
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => [
            {
                name: 'stats.tstat',
                tmp_var: 'tstat',
                scope: null
            },
            {
                name: 'stats.df',
                tmp_var: 'df',
                scope: null
            },
            {
                name: 'stats.sd',
                tmp_var: 'sd',
                scope: null
            },
        ]
    },
    { // void ztestM(Matrix* restrict m, double mu, double s, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict z, double* restrict zcrit)
        fun_matlab: 'ztest', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'ztestM', 
        req_arg_types: (args, arg_types, outs) => [   
            {
		        type: null,
		        ismatrix: true,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    },
		    {
		        type: "double",
		        ismatrix: false,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    },
		    {
		        type: "double",
		        ismatrix: false,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    }
		],
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => null,
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_h = 'h';
            var arg_pval = 'pval';
            var arg_ci = 'ci';
            var arg_z = 'z';
			if (outs.length >= 1) {
			    arg_h = outs[0];
			}
			if (outs.length >= 2) {
			    arg_pval = outs[1];
			}
			if (outs.length >= 3) {
				arg_ci = outs[2];
			} 
			if (outs.length >= 4) {
				arg_z = outs[3];
			} 
			
			return [
			    {
			        name: arg_h,
			        type: 'bool',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_pval,
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_ci,
			        type: 'double',
			        ndim: 2,
			        dim: [1,2],
			        ismatrix: false,
			        isvector: true,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_z,
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: 'zcrit',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    }
			];
		},
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void vartestM(Matrix* restrict m, double v, bool* restrict h, double* restrict pval, double* restrict *ci, double* restrict chisqstat, double* restrict df)
        fun_matlab: 'vartest', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'vartestM', 
        req_arg_types: (args, arg_types, outs) => [   
            {
		        type: null,
		        ismatrix: true,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    },
		    {
		        type: "double",
		        ismatrix: false,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    }
		],
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_h = 'h';
            var arg_pval = 'pval';
            var arg_ci = 'ci';
			if (outs.length >= 1) {
			    arg_h = outs[0];
			}
			if (outs.length >= 2) {
			    arg_pval = outs[1];
			}
			if (outs.length >= 3) {
				arg_ci = outs[2];
			} 
			return [
			    {
			        name: arg_h,
			        type: 'bool',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_pval,
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_ci,
			        type: 'double',
			        ndim: 2,
			        dim: [1,2],
			        ismatrix: false,
			        isvector: true,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: 'chisqstat',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: 'df',
			        type: 'double',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    }
			];
		},
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * betapdfM(Matrix* restrict m, double a, double b)
        fun_matlab: 'betapdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'betapdfM', 
        req_arg_types: (args, arg_types, outs) => [   
            {
		        type: null,
		        ismatrix: true,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    },
		    {
		        type: "double",
		        ismatrix: false,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    },
		    {
		        type: "double",
		        ismatrix: false,
		        isvector: false,
		        ispointer: false,
                isstruct: false
		    }
		],
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * exppdfM(Matrix* restrict m, double lambda)
        fun_matlab: 'exppdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'exppdfM',
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['1'],
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * chi2pdfM(Matrix* restrict m, double n)
        fun_matlab: 'chi2pdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'chi2pdfM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * gampdfM(Matrix* restrict m, double a, double b)
        fun_matlab: 'gampdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'gampdfM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 3,
        n_opt_args: 1,
        opt_arg_defaults: ['1'],
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * lognpdfM(Matrix* restrict m, double mu, double sigma)
        fun_matlab: 'lognpdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'lognpdfM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 3,
        n_opt_args: 2,
        opt_arg_defaults: ['0','1'],
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * normpdfM(Matrix* restrict m, double mu, double sigma)
        fun_matlab: 'normpdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'normpdfM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 3,
        n_opt_args: 2,
        opt_arg_defaults: ['0','1'],
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * unidpdfM(Matrix* restrict m, int n)
        fun_matlab: 'unidpdf', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'unidpdfM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            let left_type = arg_types[0].type;
            let left_ndim = arg_types[0].ndim;
            let left_dim = arg_types[0].dim;
            let right_type = arg_types[1].type;
            let right_ndim = arg_types[1].ndim;
            let right_dim = arg_types[1].dim;
            
            return {
                type: binaryOpType(left_type, right_type), // create function to get types giving precedence to complex, double, then int
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void normfitM(Matrix* restrict m, void* restrict mu, void* restrict sigma)
        fun_matlab: 'normfit', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'normfitM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_mu = 'mu';
            var arg_sigma = 'sigma';
			if (outs.length >= 1) {
			    arg_mu = outs[0];
			}
			if (outs.length >= 2) {
			    arg_sigma = outs[1];
			}
			return [
			    {
			        name: arg_mu,
			        type: 'double',
			        ndim: 1,
			        dim: [1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_sigma,
			        type: 'double',
			        ndim: 1,
			        dim: [1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    }
			];
		},
        return_type: arg_types => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void unifitM(Matrix* restrict m, void* restrict a, void* restrict b)
        fun_matlab: 'unifit', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'unifitM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_a = 'a';
            var arg_b = 'b';
			if (outs.length >= 1) {
			    arg_a = outs[0];
			}
			if (outs.length >= 2) {
			    arg_b = outs[1];
			}
			return [
			    {
			        name: arg_a,
			        type: 'double', //'void',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    },
			    {
			        name: arg_b,
			        type: 'double', //'void',
			        ndim: 2,
			        dim: [1,1],
			        ismatrix: false,
			        isvector: false,
			        ispointer: true,
                    isstruct: false
			    }
			];
		},
        return_type: arg_types => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * filterM(Matrix *b, Matrix *a, Matrix *x, Matrix **state);
		fun_matlab: 'filter', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'filterM',
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (args.length == 3) {
                args.push("&zero");
            }
            return args;
        },
        
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = arg_types[2].type;
            let ndim = arg_types[2].ndim;
            let dim = arg_types[2].dim;
            
            return {
                type: "complex", //type,
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            if (args.length == 3) {
                let init_var: InitVar[] = [];
				let state_size = Math.max(numel(arg_types[0].dim), numel(arg_types[1].dim));
				init_var.push({
                    name: "state_size",
                    //val: `{(int) ${state_size - 1}}`,
                    val: `{(int) fmax(getsizeM(${args[0]}), getsizeM(${args[1]})) - 1}`,
                    type: 'int',
                    ndim: 1,
                    dim: [state_size],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: "zero",
                    val: "zerosM(1, state_size)",
                    type: 'int',
                    ndim: 2,
                    dim: [1, state_size],
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                return init_var;
            } else {
                return null;
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void eigM(Matrix* restrict m, Matrix* restrict *evals, Matrix* restrict *evecs)
        fun_matlab: 'eig', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'eigM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => null,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_evals = 'evals';
            var arg_evecs = 'evecs';
			/*if (outs.length >= 1) {
			    arg_evals = outs[0];
			}
			if (outs.length >= 2) {
			    arg_evecs = outs[1];
			}*/
			return [
			    {
			        name: arg_evals,
			        type: 'complex',
			        ndim: arg_types[0].ndim,
			        dim: arg_types[0].dim,
			        ismatrix: true,
			        isvector: false,
			        ispointer: false, //true,
                    isstruct: false
			    },
			    {
			        name: arg_evecs,
			        type: 'complex',
			        ndim: arg_types[0].ndim,
			        dim: arg_types[0].dim,
			        ismatrix: true,
			        isvector: false,
			        ispointer: false, //true,
                    isstruct: false
			    }
			];
		},
        return_type: arg_types => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => {
            let expression = [];
            expression.push(`${outs[1]} = scaleM(evals, &complex_one, COMPLEX);`);
            expression.push(`${outs[0]} = scaleM(evecs, &complex_one, COMPLEX);`)
            return expression.join("\n");
        },
        init_before: (args, arg_types, outs) => {
            
            let init_var: InitVar[] = [];
            init_var.push({
                name: "complex_one",
                val: "1",
                type: 'complex',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            })
            init_var.push({
                name: outs[0],
                val: "NULL",
                type: 'complex',
                ndim: 2,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            })
            init_var.push({
                name: outs[1],
                val: "NULL",
                type: 'complex',
                ndim: 2,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false
            })
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * floorM(Matrix* restrict m)
        fun_matlab: /abs|round|floor|ceil/,
        fun_c: (args, arg_types, outs, fun_matlab) => 'fun_matlabM',
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            let ndim = arg_types[0].ndim;
            let dim = arg_types[0].dim;
            
            return {
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * maxM(Matrix* restrict m)
        fun_matlab: /\bmax\b|\bmin\b/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (outs.length > 1) {
                return 'fun_matlabV';
            } else if (args.length > 1) {
                if (arg_types[0].ismatrix && arg_types[1].ismatrix) {
                    return 'pairwise_fun_matlabM';
                }
            }
            return 'fun_matlabM';
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => {
			return outs[0];
		},
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
			if (outs.length > 1) {
			    return [
                    {
                        name: outs[1],
                        type: 'int',
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: true,
                        isstruct: false
                    }
                ];
			} else {
			    return null;
			}
        },
        return_type: (args, arg_types, outs) => {
            let type = arg_types[0].type;
            
            return {
                type: type,
                ndim: 2,
                dim: [1,1],
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * varM(Matrix* restrict m)
        fun_matlab: /std|var/, 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (args.length == 2) {
                if (args[1] == "1") {
                    return 'popfun_matlabM';
                }
            }
            return 'fun_matlabM';
        },
        args_transform: (args, arg_types, outs) => {
            if (args.length == 3) {
                if (args[2] == "2") {
                    return ["mat"];
                }
            }
            return [args[0]];
        },
        arg_types_transform: (args, arg_types, outs) => null,
        req_arg_types: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = "double";
            if (arg_types[0].type == "complex") {
                type = "complex";
            }
            let dim = [1,1];
            let ndim = 2;
            if (!dim.some(x => x === 1)) {
                dim = [1, arg_types[0].dim[1]];
                if (args.length == 3) {
                    if (args[2] == "2") { 
                        dim = [1, arg_types[0].dim[0]];
                    }
                }
            }
            return {
                type: type, 
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            if (args.length == 3) {
                if (args[2] == "2") {
                    let init_var: InitVar[] = [];
                    init_var.push({
                        name: 'mat',
                        val: `transposeM(${args[0]})`,
                        type: arg_types[0].type,
                        ndim: arg_types[0].ndim,
                        dim: arg_types[0].dim.reverse(),
                        ismatrix: true,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * quantileM_vec(Matrix* restrict m, int N, double* restrict quantiles)
        fun_matlab: 'quantile', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'quantileM_vec', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let quantile_specifier = Number(args[1]);
            let quantiles = "vec"
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles:number = quantile_specifier;
            } else {
                var n_quantiles:number = numel(arg_types[1].dim);
                if (!arg_types[1].ismatrix) {
                    quantiles = args[1];
                }
            }
            return [args[0], n_quantiles, quantiles];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let arg0_type = arg_types[0].type;
            let arg0_ndim = arg_types[0].ndim;
            let arg0_dim = arg_types[0].dim;
            let quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles:number = quantile_specifier;
            } else {
                var n_quantiles:number = numel(arg_types[1].dim);
            }
            
            // If m is a row or column vector, returns a 1xN matrix with the specified quantiles
            // If m is a matrix of size rxc, returns a Nxc matrix with the specified quantiles of each column
            if (arg0_ndim == 2 && arg0_dim.some(x => x === 1)) { // vector
                return {
                    type: arg0_type,
                    ndim: 2,
                    dim: [1, n_quantiles],
                    ismatrix: true,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            } else {
                return {
                    type: arg0_type,
                    ndim: 2,
                    dim: [n_quantiles, arg0_dim[1]],
                    ismatrix: true,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            }
            
        },
        push_main_before: (args, arg_types, outs) => {
            let quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles:number = quantile_specifier;
                //let expression = [];
                //expression.push(`double arr[${n_quantiles}];`);
                let step = 1/(quantile_specifier + 1);
                let expression = `
for (int i = 1; ${step}*i < 1; i ++) {
    vec[i-1] = ${step}*i;
}
                `;
                return expression;
            } else {
                return null;
            }
        },
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
            
                let init_var: InitVar[] = [];
                init_var.push({
                    name: 'vec',
                    val: "{}",
                    type: 'double',
                    ndim: 1,
                    dim: [quantile_specifier],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false, //true,
                    isstruct: false
                })

                return init_var;
            } else {
                if (arg_types[1].ismatrix) {
                    let init_var: InitVar[] = [];
                    
                    let type = arg_types[1].type;
                    init_var.push({
                        name: 'vec',
                        val: `${type.charAt(0)}_to_${type.charAt(0)}(${args[1]})`,
                        type: type,
                        ndim: 1,
                        dim: [numel(arg_types[1].dim)],
                        ismatrix: false,
                        isvector: false, //true,
                        ispointer: true,
                        isstruct: false
                    })
    
                    return init_var;
                }
                return null
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * zerosM(int ndim, int dim[ndim])
        fun_matlab: /zeros|ones/, 
        fun_c: (args, arg_types, outs, fun_matlab) => 'fun_matlabM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (arg_types[0].ispointer) {
                return ['ndim', args[0]]
            }
            return ['ndim', 'dim'];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let dim = [];
            let ndim = args.length;
            if (arg_types[0].ispointer) {
                dim = arg_types[0].dim;
                ndim = arg_types[0].ndim;
            } else {
                if (args.length == 1) {
                    dim = [Number(args[0]), Number(args[0])];
                    ndim = 2;
                } else {
                    for (let arg of args) {
                        dim.push(Number(arg));
                    }
                }
            }
            return {
                type: 'int',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let dim = `{${args.join(", ")}}`;
            let ndim = args.length;
            let init_var: InitVar[] = [];
            
            if (arg_types[0].ispointer) {
                init_var.push({
                    name: 'ndim',
                    val: arg_types[0].ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
            } else {
                if (args.length == 1) {
                    dim = `{${args[0]},${args[0]}}`;
                    ndim = 2;
                }
                init_var.push({
                    name: 'ndim',
                    val: ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'dim',
                    val: dim,
                    type: 'int',
                    ndim: 1,
                    dim: [ndim],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
            }
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * identityM(int size)
        fun_matlab: 'eye', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'identityM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => [args[0]],
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            var dim = [Number(args[0]), Number(args[0])];
            var ndim = 2;
            
            return {
                type: 'int',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // bool reshapeM (Matrix* m, int ndim, int dim[ndim]);
        fun_matlab: 'reshape', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'reshapeM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            return [args[0], 'ndim', 'dim'];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let dim = [];
            let ndim = 2;
            if (arg_types[1].ispointer) {
                dim = arg_types[1].dim;
                ndim = arg_types[1].ndim;
            } else {
                let vec_el = parseVectorArg(args[1]);
                ndim = vec_el.length;
                if (ndim != 0) {
                    for (let v of vec_el) {
                        let isnum = !/^[a-z].*$/.test(v);
                        if (isnum) {
                            dim.push(Number(v));
                        } else {
                            dim.push(v);
                        }
                    }
                } else {
                    ndim = 1; //null;
                    dim = [1]; //null;
                }
            }
            return {
                type: arg_types[0].type,
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,
        /*push_main_after: (args, arg_types, outs) => {
            return `${outs[0]} = ${args[0]};`
        },*/         
        init_before: (args, arg_types, outs) => {
            let init_var: InitVar[] = [];
            /*init_var.push({
                name: outs[0],
                val: args[0],
                type: arg_types[0].type,
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            })*/
            if (arg_types[1].ismatrix) {
                let type = arg_types[1].type;
                init_var.push({
                    name: 'dim',
                    val: `${type.charAt(0)}_to_${type.charAt(0)}(${args[1]})`,
                    type: type,
                    ndim: 1,
                    dim: [numel(arg_types[1].dim)],
                    ismatrix: false,
                    isvector: false, //true,
                    ispointer: true,
                    isstruct: false
                })
                init_var.push({
                    name: 'ndim',
                    val: arg_types[1].ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
            } else {
                let dim = `{${args.slice(1).join(", ")}}`;
                let ndim = args.slice(1).length;
                if (args.length == 2) {
                    dim = `{${args[1]},${args[1]}}`;
                    ndim = 2;
                }
            
                init_var.push({
                    name: 'ndim',
                    val: ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'dim',
                    val: dim,
                    type: 'int',
                    ndim: 1,
                    dim: [ndim],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
            }
                
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // void determinantM(Matrix* restrict m, void* restrict output)
        fun_matlab: 'det', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'detM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => {
            var arg_d = 'd';
			if (outs.length >= 1) {
			    arg_d = outs[0];
			}
			
			return [
			    {
			        name: arg_d,
			        //type: "double",
			        type: arg_types[0].type,
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true, //false,
                    isstruct: false 
			    }
			];
		},
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {  // Matrix * invertM(Matrix* restrict m)
        fun_matlab: 'inv', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'invertM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: "double",
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false,
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { 
        fun_matlab: 'cell', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => null,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            var dim = [];
            var ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            } else {
                for (let arg of args) {
                    dim.push(Number(arg));
                }
            }
            
            return {
                type: 'heterogeneous',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: true,
                isstruct: false,
            };
        },
        push_main_before: (args, arg_types, outs) => {
            var dim = [];
            var ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            } else {
                for (let arg of args) {
                    dim.push(Number(arg));
                }
            }
            let numel = dim.reduce(function(a, b) {return a * b;});
            return `
Matrix **${outs[0]} = NULL;
${outs[0]} = malloc(${numel}*sizeof(*${outs[0]}));
	        `
        },
        push_main_after: (args, arg_types, outs) => null,      
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // int strcmp(const char* str1, const char* str2)
        fun_matlab: 'strcmp', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'strcmp', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'int',
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // int strcmpi(const char * str1, const char * str2 )
        fun_matlab: 'strcmpi', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'strcmpi', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        
        return_type: (args, arg_types, outs) => {
            return {
                type: 'int',
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'struct', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        //return_type: (args, arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'heterogeneous',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: true
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'rmfield', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        //return_type: (args, arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'heterogeneous',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: true
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'builtin', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        //return_type: (args, arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'all', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        //return_type: (args, arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'bool',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    // TO DO: make separate entry for length, length often used similarly to numel but not always
    { //int getsizeM(Matrix* restrict m)
        fun_matlab: /numel|length/, 
        fun_c: (args, arg_types, outs, fun_matlab) => 'getsizeM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'int',
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // int * getDimsM(Matrix* restrict m)
        fun_matlab: 'size', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'getDimsM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => [args[0]],
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'int',
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: false,
                isvector: false,
                ispointer: true
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => {
            if (args.length > 1) {
                return `tmp_out[${args[1] - 1}]`;
            }
            return null;
        },
        push_alias_tbl: (args, arg_types, outs) => null
    },
    /*{
        fun_matlab: 'length', // length(X) = max(size(X))
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },*/
    {
        fun_matlab: 'sum', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'prod', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'error', 
        fun_c: (args, arg_types, outs, fun_matlab) => null,
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'permute', 
        fun_c: (args, arg_types, outs, fun_matlab) => null,
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * randM(int ndim, int dim[ndim])
        fun_matlab: 'rand', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let match = args.find(x => x.includes('seed'));
            if (match != null && match != undefined) {
                //let tmp = args[args.indexOf(match)];
                return 'randomSeed';
            } else {
                return 'randM'; 
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let match = args.find(x => x.includes('seed'));
            if (match != null && match != undefined) {
                return ['void'];
            } else {
                if (arg_types[0].ispointer) {
                    return ['ndim', args[0]]
                }
                return ['ndim', 'dim'];
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => {
            let match = args.find(x => x.includes('seed'));
            if (match != null && match != undefined) {
                return null;
            } else {
                return outs; 
            }
        },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let match = args.find(x => x.includes('seed'));
            if (match != null && match != undefined) {
                return null;
            } else {
                let dim = [];
                let ndim = args.length;
                if (arg_types[0].ispointer) {
                    dim = arg_types[0].dim;
                    ndim = arg_types[0].ndim;
                } else {
                    if (args.length == 1) {
                        dim = [Number(args[0]), Number(args[0])];
                        ndim = 2;
                    } else {
                        for (let arg of args) {
                            dim.push(Number(arg));
                        }
                    }
                }
                return {
                    type: 'double',
                    ndim: ndim,
                    dim: dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let match = args.find(x => x.includes('seed'));
            if (match != null && match != undefined) {
                return null;
            } else {
                let dim = `{${args.join(", ")}}`;
                let ndim = args.length;
                let init_var: InitVar[] = [];
                
                if (arg_types[0].ispointer) {
                    init_var.push({
                        name: 'ndim',
                        val: arg_types[0].ndim,
                        type: 'int',
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                } else {
                    if (args.length == 1) {
                        dim = `{${args[0]},${args[0]}}`;
                        ndim = 2;
                    }
                    init_var.push({
                        name: 'ndim',
                        val: ndim,
                        type: 'int',
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                    init_var.push({
                        name: 'dim',
                        val: dim,
                        type: 'int',
                        ndim: 1,
                        dim: [ndim],
                        ismatrix: false,
                        isvector: true,
                        ispointer: false,
                        isstruct: false
                    })
                }
                return init_var;
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * randiM(int ndim, int dim[ndim], int type, int imax)
        fun_matlab: 'randi', // X = randi(imax,sz1,...,szN)
        fun_c: (args, arg_types, outs, fun_matlab) => 'randiM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            return ['ndim', 'dim', 0, args[0]];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let dim = [];
            let ndim = args.slice(1).length;
            if (args.length == 2) {
                dim = [Number(args[1]), Number(args[1])];
                ndim = 2;
            } else {
                for (let arg of args.slice(1)) {
                    dim.push(Number(arg));
                }
            }
            return {
                type: "int",
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let dim = '';
            let ndim = args.slice(1).length;
            if (args.length == 2) {
                dim = `{${args[1]}, ${args[1]}}`;
                ndim = 2;
            } else {
                dim = `{${args.slice(1).join(", ")}}`;
            }
            let init_var: InitVar[] = [];
            
            if (arg_types[0].ispointer) {
                init_var.push({
                    name: 'ndim',
                    val: arg_types[0].ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
            } else {
                init_var.push({
                    name: 'ndim',
                    val: ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'dim',
                    val: dim,
                    type: 'int',
                    ndim: 1,
                    dim: [ndim],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
            }
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * randnM(int ndim, int dim[ndim])
        fun_matlab: 'randn', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'randnM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (arg_types[0].ispointer) {
                return ['ndim', args[0]]
            }
            return ['ndim', 'dim'];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let dim = [];
            let ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            } else {
                for (let arg of args) {
                    dim.push(Number(arg));
                }
            }
            return {
                type: "double",
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let dim = `{${args.join(", ")}}`;
            let ndim = args.length;
            let init_var: InitVar[] = [];
            
            if (arg_types[0].ispointer) {
                init_var.push({
                    name: 'ndim',
                    val: arg_types[0].ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
            } else {
                if (args.length == 1) {
                    dim = `{${args[0]},${args[0]}}`;
                    ndim = 2;
                }
                init_var.push({
                    name: 'ndim',
                    val: ndim,
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                init_var.push({
                    name: 'dim',
                    val: dim,
                    type: 'int',
                    ndim: 1,
                    dim: [ndim],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
            }
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'memmapfile', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: arg_types => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * medianM(Matrix* restrict m)
        fun_matlab: 'median', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'medianM',
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: arg_types[0].type, 
                ndim: 2,
                dim: [1, 1],
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * meanM(Matrix* restrict m)
        fun_matlab: 'mean', // M = mean(A,dim)
        fun_c: (args, arg_types, outs, fun_matlab) => 'meanM', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (args.length == 2) {
                if (args[1] == "2") {
                    return ["mat"];
                }
            }
            return [args[0]];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let type = "double";
            if (arg_types[0].type == "complex") {
                type = "complex";
            }
            let dim = [1,1];
            let ndim = 2;
            if (!dim.some(x => x === 1)) {
                dim = [1, arg_types[0].dim[1]];
                if (args.length == 2) {
                    if (args[1] == "2") { 
                        dim = [1, arg_types[0].dim[0]];
                    }
                }
            }
            return {
                type: type, 
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            if (args.length == 2) {
                if (args[1] == "2") {
                    let init_var: InitVar[] = [];
                    init_var.push({
                        name: 'mat',
                        val: `transposeM(${args[0]})`,
                        type: arg_types[0].type,
                        ndim: arg_types[0].ndim,
                        dim: arg_types[0].dim.reverse(),
                        ismatrix: true,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    })
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * meanM(Matrix* restrict m)
        fun_matlab: 'sqrt', // M = mean(A,dim)
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (arg_types[0].ismatrix) {
                return 'powerM';
            } else {
                return 'sqrt';
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                let obj = type_to_matrix_type.find(x => x.type === 'double');
                return [args[0], '&exponent', obj.matrix_type.toString()];
            } else {
                return args;
            }
            
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'complex', // create function to get types giving precedence to complex, double, then int
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: arg_types[0].ismatrix,
                isvector: false,
                ispointer: false, //left_ismatrix && !right_ismatrix,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                let init_var: InitVar[] = [];
                init_var.push({
                    name: 'exponent',
                    val: '0.5',
                    type: 'double',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                })
                return init_var;
            }
            return null;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: /isa|orderfields/, 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: /fullfile|fileparts|which/, 
        fun_c: (args, arg_types, outs, fun_matlab) => 'fun_matlab_toreplace', 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'char', 
                ndim: 1,
                dim: [50],
                ismatrix: false,
                isvector: true,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: 'fieldnames', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // TO DO: fix this
        fun_matlab: 'struct', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'unknown', 
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: true 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // TO DO: FIX THIS https://www.tutorialspoint.com/c_standard_library/c_function_printf.htm
        fun_matlab: 'disp', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if ((arg_types[0].type != 'char' && arg_types[0].isvector) || arg_types[0].type == "dynamic" || arg_types[0].type.includes("_type") || args[0] == null) {
                return null; // args[0] == null occurs when calling disp(sprintf(...)) 
            } else if (arg_types[0].ismatrix) {
                return 'printM';
            } else {
                return 'printf';
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            if (arg_types[0].ismatrix) {
                return args;
            } else {
                let format = '"\\n%d\\n"';
                //if (arg_types[0].type == 'double' || arg_types[0].type == 'complex') {
                if (arg_types[0].type == 'double') {
                    format = '"\\n%.4f\\n"';
                } else if (arg_types[0].type == 'complex') {
                    format = '"\\n%.4f + %.4fi\\n"';
                } else if (arg_types[0].type == 'int') {
                    format = '"\\n%d\\n"';
                } else if (arg_types[0].type == 'char') {
                    format = '"\\n%s\\n"';
                    args[0] = args[0].replace(/'/g, '"');
                }
                
                if (arg_types[0].type == 'complex') {
                    return [format, `creal(${args[0]})`, `cimag(${args[0]})`];
                } else {
                    return [format, args[0]];
                }
                //return [format, args[0]];
                //args.unshift(format);
                //return args;
            }
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        //push_main_before: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => {
            if (arg_types[0].type == "dynamic") {
return `switch(${args[0]}.type) {
\tcase 0:
\tprintf("\\n%d\\n", ${args[0]}.data.ival);
\tbreak;
        
\tcase 1:
\tprintf("\\n%f\\n", ${args[0]}.data.dval);
\tbreak;
        
\tcase 2:
\tprintf("\\n%f\\n", ${args[0]}.data.cval);
\tbreak;
        
\tcase 3:
\tprintf("\\n%s\\n", ${args[0]}.data.chval);
\tbreak;
}`;
            } else if (arg_types[0].type.includes("_type")) {
                return `unknown_printf(&${args[0]});`;
            } else if (arg_types[0].type != 'char' && arg_types[0].isvector) {
                let format = '"\\n%d\\n"';
                if (arg_types[0].type == 'double') {
                    format = '"\\n%.4f\\n"';
                } else if (arg_types[0].type == 'complex') {
                    format = '"\\n%.4f + %.4fi\\n"';
                } else if (arg_types[0].type == 'int') {
                    format = '"\\n%d\\n"';
                }
return `for (int i = 0 ; i < ${arg_types[0].dim[0]} ; i++) {
printf(${format}, ${args[0]}[i]);
}`;     // to do: move free to generatecode.ts  free(${args[0]});`;       
            } else {
                return null;
            }
        },
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // TO DO: FIX THIS
        fun_matlab: 'mod', 
        fun_c: (args, arg_types, outs, fun_matlab) => `${args[0]} % ${args[1]}`,
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => null,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'int', 
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    {
        fun_matlab: /real|imag/, 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            return 'cfun_matlab';
        },  
        req_arg_types: (args, arg_types, outs) => [
            {
                type: 'complex', 
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            }
        ],
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'double', 
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // TO DO: FIX THIS
        fun_matlab: 'sprintf', 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (args[0] == "disp") {
                return 'printf';
            } else {
                return 'sprintf';
            }
        },
        args_transform: (args, arg_types, outs) => {
            if (args[0] == "disp") {
                args.shift();
                for (let i = 0; i < args.length; i++) {
                    args[i] = args[i].replace(/'/g, '"');
                }
                if (args.length == 1) {
                    return ['"\\n%s\\n"', args[0]];
                }
                
                args[0] = args[0].replace(/stdout/g, '"\\n%s\\n"');
                return args;
            }
            
            let buffer = 'str';
            if (outs.length > 0) {
                buffer = outs[0];
            }
            for (let i = 0; i < args.length; i++) {
                args[i] = args[i].replace(/'/g, '"');
            }
            if (args.length == 1) {
                return [buffer, '"\\n%s\\n"', args[0]];
            }
            
            args[0] = args[0].replace(/stdout/g, '"\\n%s\\n"');
            args.unshift(buffer);
            return args;
        },
        arg_types_transform: (args, arg_types, outs) => null,
        req_arg_types: (args, arg_types, outs) => [
            {
                type: 'char', 
                ndim: null,
                dim: null,
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            },
            {
                type: null, 
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            }
        ],
		outs_transform: (args, arg_types, outs) => null,
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => null,
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            if (outs.length > 0) {
                let init_var: InitVar[] = [];
                init_var.push({
                    name: outs[0],
                    val: null,
                    type: 'char',
                    ndim: 1,
                    dim: [50],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                })
                return init_var;
            }
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // Matrix * fftM(Matrix* restrict m)
        fun_matlab: /fft|ifft/, 
        fun_c: (args, arg_types, outs, fun_matlab) => {
            if (args.length == 2) {
                if (arg_types[1].type == 'int') {
                    return 'fun_matlabV';
                }
            }
            return 'fun_matlabM';
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => args,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            
            let dim = arg_types[0].dim;
            let ndim = arg_types[0].ndim;
            
            if (args.length == 2) {
                console.log("WARNING: fftM dimensions adjusted")
            }
            
            return {
                type: 'double',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,//true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { //TO DO: Matrix * stftV(Matrix* restrict m, int win_size, int inc, int num_coef, int win_type)
        fun_matlab: 'stft', 
        fun_c: (args, arg_types, outs, fun_matlab) => 'stftV',
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => {
            let win_type = 1;
            let num_coef = '128';
            let inc = '24';
            let win_size = '80';
            
            let match = args.find(x => x.includes('Window'));
            if (match != null && match != undefined) {
                let tmp = args[args.indexOf(match) + 1];
                let match11 = tmp.match(/(\S+)\(/);
                let tmp2 = match11[1];
                let re = new RegExp(`${tmp2}\\((.*)[,\)]`);
                let match12 = tmp.match(re);
                win_size = match12[1];
                
                if (tmp2 === "hanning") {
                    win_type = 1;
                } else if (tmp2 === "hamming") {
                    win_type = 2;
                } else if (tmp2 === "rectangle") {
                    win_type = 3;
                }
            }
            
            let match2 = args.find(x => x.includes('OverlapLength'));
            
            if (match2 != null && match2 != undefined) {
                inc = 'inc';
                
            }
            
            let match3 = args.find(x => x.includes('FFTLength'));
            
            if (match3 != null && match3 != undefined) {
                num_coef = 'num_coef';
                
            }
            
            return [args[0], win_size, inc, num_coef, win_type];
        },
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            let dim = arg_types[0].dim;
            let ndim = arg_types[0].ndim;
            
            return {
                type: 'double',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false, //true,
                isstruct: false 
            };
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        init_before: (args, arg_types, outs) => {
            let init_var: InitVar[] = [];
            
            let num_coef = '128';
            let inc = '24';
            let win_size = '80';
            
            let match = args.find(x => x.includes('Window'));
            if (match != null && match != undefined) {
                let tmp = args[args.indexOf(match) + 1];
                let match11 = tmp.match(/(\S+)\(/);
                let tmp2 = match11[1];
                let re = new RegExp(`${tmp2}\\((.*)[,\)]`);
                let match12 = tmp.match(re);
                win_size = match12[1];
            }
            
            let match2 = args.find(x => x.includes('OverlapLength'));
            let overlap = '0';
            if (match2 != null && match2 != undefined) {
                overlap = args[args.indexOf(match2) + 1];
            }
            
            let match3 = args.find(x => x.includes('FFTLength'));
            
            if (match3 != null && match3 != undefined) {
                num_coef = args[args.indexOf(match3) + 1];
                num_coef = num_coef.concat("/2");
                
            }
            
            init_var.push({
                name: 'inc',
                val: `${win_size} - ${overlap}`,
                type: 'int',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            });
            init_var.push({
                name: 'num_coef',
                val: num_coef,
                type: 'int',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            });
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { // double * hamming(int n)
        fun_matlab: /hamming|hanning/,
        fun_c: (args, arg_types, outs, fun_matlab) => {
            let match = args.find(x => x.includes("periodic"));
            if (match !== null && match !== undefined) {
                return 'periodicfun_matlab';
            } else {
                return 'fun_matlab';
            }
        }, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => [args[0]], 
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            return {
                type: 'double',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: true,
                ispointer: false,
                isstruct: false 
            };
            
            /*return {
                type: 'double',
                ndim: 1,
                dim: [Number(args[0])],
                ismatrix: false,
                isvector: true,
                ispointer: false,
                isstruct: false 
            };*/
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => null,         
        //init_before: (args, arg_types, outs) => null,
        init_before: (args, arg_types, outs) => {
            let init_var: InitVar[] = [];
            
            if (outs.length > 0) {
                init_var.push({
                    name: outs[0],
                    val: null,
                    type: 'double',
                    ndim: 1,
                    dim: [args[0]],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                });
            } else {
                init_var.push({
                    name: 'tmp_out',
                    val: null,
                    type: 'double',
                    ndim: 1,
                    dim: [args[0]],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                });
            }
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { 
        fun_matlab: 'tic', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => null,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            if (outs.length == 1) {
                return {
                    type: 'time_t',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            } else {
                return null;
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => {
            return 'tstart = time(0);';
        },         
        init_before: (args, arg_types, outs) => {
           // time_t tstart, tend;
            let init_var: InitVar[] = [];
            
            
            init_var.push({
                name: 'tstart',
                val: null,
                type: 'time_t',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            })
            init_var.push({
                name: 'tend',
                val: null,
                type: 'time_t',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            })
            
            return init_var;
        },
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    },
    { 
        fun_matlab: 'toc', 
        fun_c: (args, arg_types, outs, fun_matlab) => null, 
        req_arg_types: (args, arg_types, outs) => null,
        args_transform: (args, arg_types, outs) => null,
        arg_types_transform: (args, arg_types, outs) => null,
		outs_transform: (args, arg_types, outs) => outs[0],
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: (arg_types, outs) => null,
        return_type: (args, arg_types, outs) => {
            if (outs.length == 1) {
                return {
                    type: 'double',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false, //true,
                    isstruct: false 
                };
            } else {
                return null;
            }
        },
        push_main_before: (args, arg_types, outs) => null,
        push_main_after: (args, arg_types, outs) => {
            let expression = [];
            expression.push('tend = time(0);');
            if (outs.length == 1) {
                expression.push(`double ${outs[0]} = difftime(tend, tstart)`);
            }
            return 'tend = time(0);';
            // difftime(tend, tstart)
        },         
        init_before: (args, arg_types, outs) => null,
        tmp_out_transform: (args, arg_types, outs) => null,
        push_alias_tbl: (args, arg_types, outs) => null
    }
];