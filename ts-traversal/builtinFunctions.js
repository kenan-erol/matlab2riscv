"use strict";
exports.__esModule = true;
exports.builtin_functions = exports.operatorMapping = void 0;
var helperFunctions_1 = require("./helperFunctions");
var customTypes_1 = require("./customTypes");
var type_to_matrix_type = [
    { type: "integer", matrix_type: 0 },
    { type: "int", matrix_type: 0 },
    { type: "double", matrix_type: 1 },
    { type: "complex", matrix_type: 2 },
    { type: "char", matrix_type: 3 }
];
function parseCharArg(arg) {
    var regex = /(?<=')(.*?)(?=')|(?<=")(.*?)(?=")/;
    var match = arg.match(regex);
    if (match != null) {
        return match[0];
    }
    else {
        return arg;
    }
}
function parseVectorArg(arg) {
    var regex = /(?<=\[)(.*?)(?=\])/;
    var match1 = arg.match(regex);
    if (match1 != null) {
        var vec_str = match1[0];
        var regex2 = /".*?"|[^,; ]*/g;
        var match2 = vec_str.match(regex2);
        var vec_elements = [];
        if (match2 != null) {
            for (var _i = 0, match2_1 = match2; _i < match2_1.length; _i++) {
                var element = match2_1[_i];
                if (element != "") {
                    vec_elements.push(parseCharArg(element));
                }
            }
        }
        //return [`{${vec_str}}`, vec_elements];
        return vec_elements;
    }
    else {
        //return [arg, []];
        return [];
    }
}
exports.operatorMapping = [
    {
        //fun_matlab: '+',
        fun_matlab: /\+|\-/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (args.length == 1) {
                return null;
            }
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                if (fun_matlab == "+") {
                    return 'plusM';
                }
                else {
                    return 'minusM';
                }
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) {
            if (args.length == 1) {
                return null;
            }
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
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
            }
            else {
                return null;
            }
        },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            var ndim = arg_types[0].ndim;
            var dim = arg_types[0].dim;
            var ismatrix = arg_types[0].ismatrix;
            if (args.length == 2) {
                type = (0, customTypes_1.binaryOpType)(type, arg_types[1].type);
            }
            return {
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        // Matrix * mtimesM(Matrix *m, Matrix *n)
        fun_matlab: '*',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'mtimesM';
            }
            else if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
                return 'scaleM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                var obj = type_to_matrix_type.find(function (x) { return x.type === arg_types[1].type; });
                return [args[0], "&scalar", obj.matrix_type.toString()];
            }
            else if (!left_ismatrix && right_ismatrix) {
                var obj = type_to_matrix_type.find(function (x) { return x.type === arg_types[0].type; });
                return [args[1], "&scalar", obj.matrix_type.toString()];
            }
            else {
                return args;
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return {
                    type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                    ndim: left_ndim,
                    dim: left_dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
            else {
                return {
                    type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                    ndim: left_ndim,
                    dim: [left_dim[0], right_dim[1]],
                    ismatrix: left_ismatrix || right_ismatrix,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var left_type = arg_types[0].type;
            var right_ismatrix = arg_types[1].ismatrix;
            var right_type = arg_types[1].type;
            if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
                var init_var = [];
                var val = args[0];
                var type = left_type;
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
                });
                return init_var;
            }
            else {
                return null;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '/',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'mrdivideM';
            }
            else if (left_ismatrix && !right_ismatrix) {
                return 'scaleM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            }
            else if (left_ismatrix && !right_ismatrix) {
                var left_type = arg_types[0].type;
                var right_type = arg_types[1].type;
                var type_1 = (0, customTypes_1.binaryOpType)(left_type, right_type);
                var obj = type_to_matrix_type.find(function (x) { return x.type === type_1; });
                return [args[0], "1/(".concat(args[1], ")"), obj.matrix_type.toString()];
            }
            else {
                if (arg_types[0].type == "complex" && arg_types[1].type != "complex") {
                    return ["(".concat(args[0], ")"), "((complex) ".concat(args[1], ")")];
                }
                else if (arg_types[0].type != "complex" && arg_types[1].type == "complex") {
                    return ["((complex) ".concat(args[0], ")"), "(".concat(args[1], ")")];
                }
                else if (arg_types[1].type == "complex") {
                    return [args[0], "(".concat(args[1], ")")];
                }
                return ["(double) ".concat(args[0]), args[1]];
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            var type = (0, customTypes_1.binaryOpType)(left_type, right_type);
            if (left_type == 'complex' || right_type == 'complex') {
                type = 'complex';
            }
            else {
                type = 'double';
            }
            return {
                type: type,
                ndim: left_ndim,
                dim: [left_dim[0], right_dim[1]],
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '\\',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'mldivideM';
            }
            else if (!left_ismatrix && right_ismatrix) {
                return 'scaleM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            }
            else {
                var left_type = arg_types[0].type;
                var right_type = arg_types[1].type;
                var type_2 = (0, customTypes_1.binaryOpType)(left_type, right_type);
                var obj = type_to_matrix_type.find(function (x) { return x.type === type_2; });
                return ["1/(".concat(args[0], ")"), args[1], obj.matrix_type.toString()];
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: [left_dim[0], right_dim[1]],
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '^',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var left_type = arg_types[0].type;
            var right_ismatrix = arg_types[1].ismatrix;
            var right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                return 'mpowerM';
            }
            else {
                if (left_type == 'complex' || right_type != 'int') {
                    return 'cpow';
                }
                else {
                    return 'pow';
                }
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var left_type = arg_types[0].type;
            var right_ismatrix = arg_types[1].ismatrix;
            var right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                /*let type = left_type;
                if (right_type != 'int') {
                    type = 'complex';
                } else {
                    if (args[1][0] == "-") {
                        type = 'complex';
                    }
                }*/
                var type_3 = right_type;
                var obj = type_to_matrix_type.find(function (x) { return x.type === type_3; });
                /*let isnum = /^[\d.]+[\+\-][\d.]+\*I$/.test(args[1]);
                if (!isnum) {
                    isnum = /^[\d.]+$/.test(args[1]);
                }*/
                var isnum = !/^[a-z].*$/.test(args[1]);
                if (isnum) {
                    return [args[0], '&exponent', obj.matrix_type.toString()];
                }
                else {
                    return [args[0], "&".concat(args[1]), obj.matrix_type.toString()];
                }
            }
            return args;
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var right_ismatrix = arg_types[1].ismatrix;
            /*let type = binaryOpType(left_type, right_type);
            if (left_type == 'complex' || right_type != 'int') {
                type = 'complex';
            }*/
            var type = left_type;
            if (right_type != 'int') {
                type = 'complex';
            }
            return {
                type: type,
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix && !right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                var isnum = !/^[a-z].*$/.test(args[1]);
                if (isnum) {
                    var init_var = [];
                    init_var.push({
                        name: 'exponent',
                        val: "".concat(args[1]),
                        type: right_type,
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    });
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '.*',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'timesM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: './',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'rdivideM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '.\\',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'ldivideM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        // Matrix * scalarpowerM(Matrix* restrict m, void* restrict exponent, int type)
        fun_matlab: '.^',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            /*if (!left_ismatrix && !right_ismatrix) {
                return null;
            } else {
                return 'powerM';
            }*/
            if (left_ismatrix && right_ismatrix) {
                return 'powerM';
            }
            if (left_ismatrix && !right_ismatrix) {
                return 'scalarpowerM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            var left_type = arg_types[0].type;
            var right_type = arg_types[1].type;
            if (left_ismatrix && !right_ismatrix) {
                /*let type = left_type;
                if (right_type != 'int') {
                    type = 'complex';
                }*/
                var type_4 = right_type;
                //let type = binaryOpType(left_type, right_type);
                var obj = type_to_matrix_type.find(function (x) { return x.type === type_4; });
                var isnum = /^\d+$/.test(args[1]);
                if (isnum) {
                    return [args[0], 'scalar', "".concat(obj.matrix_type)];
                }
                else {
                    return [args[0], "&".concat(args[1]), "".concat(obj.matrix_type)];
                }
            }
            else {
                return args;
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            var type = left_type;
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
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                var isnum = /^\d+$/.test(args[1]);
                if (isnum) {
                    var init_var = [];
                    init_var.push({
                        name: 'scalar',
                        val: "".concat(args[1]),
                        type: right_type,
                        //ndim: right_ndim,
                        //dim: [right_ndim],
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    });
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /(<=)|(>=)|<|>/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            if (left_ismatrix) {
                if (fun_matlab == "<") {
                    return 'matlab_ltM';
                }
                else if (fun_matlab == "<=") {
                    return 'matlab_leM';
                }
                else if (fun_matlab == ">") {
                    return 'matlab_gtM';
                }
                else if (fun_matlab == ">=") {
                    return 'matlab_geM';
                }
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) {
            if (args.length == 1) {
                return null;
            }
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if ((left_ismatrix && !right_ismatrix) || (!left_ismatrix && right_ismatrix)) {
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
            }
            else {
                return null;
            }
        },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                return [args[0], 'tmp'];
            }
            else {
                return args;
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: 'bool',
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && !right_ismatrix) {
                var dim = "{".concat(arg_types[0].dim, "}");
                var ndim = arg_types[0].ndim;
                var init_var = [];
                init_var.push({
                    name: 'ndim',
                    val: "".concat(ndim),
                    type: 'int',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                });
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
                });
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
                });
                var obj = type_to_matrix_type.find(function (x) { return x.type === arg_types[1].type; });
                init_var.push({
                    name: 'tmp',
                    val: "scaleM(onesM(ndim, dim), &scalar, ".concat(obj.matrix_type, ")"),
                    type: 'int',
                    ndim: ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                });
                return init_var;
            }
            else {
                return null;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '==',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'equalM';
            }
            else {
                if (arg_types[0].type == "int" && arg_types[1].type == "int") {
                    return "".concat(args[0], " == ").concat(args[1]);
                }
                else {
                    return "fabs(".concat(args[0], " - ").concat(args[1], ") < 1e-6");
                }
            }
        },
        req_arg_types: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
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
            }
            else {
                return null;
            }
        },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            }
            else {
                return ['void'];
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: 'bool',
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '~=',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return 'neM';
            }
            else {
                if (arg_types[0].type == "int" && arg_types[1].type == "int") {
                    return "".concat(args[0], " != ").concat(args[1]);
                }
                else {
                    return "fabs(".concat(args[0], " - ").concat(args[1], ") > 1e-6");
                }
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                return args;
            }
            else {
                return ['void'];
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: 'bool',
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /\&|\|/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            if (left_ismatrix && right_ismatrix) {
                if (fun_matlab == "&") {
                    return 'andM';
                }
                else {
                    return 'orM';
                }
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            var left_ismatrix = arg_types[0].ismatrix;
            var right_ismatrix = arg_types[1].ismatrix;
            return {
                type: 'bool',
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: left_ismatrix || right_ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /\&\&|\|\|/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: '~',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var ismatrix = arg_types[0].ismatrix;
            if (ismatrix) {
                return 'notM';
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            var ndim = arg_types[0].ndim;
            var dim = arg_types[0].dim;
            return {
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: arg_types[0].ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /'|\.'/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var ismatrix = arg_types[0].ismatrix;
            if (ismatrix) {
                if (fun_matlab == "'") {
                    return 'ctransposeM';
                }
                else {
                    return 'transposeM';
                }
            }
            else {
                return null;
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) {
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
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            var ndim = arg_types[0].ndim;
            var dim = arg_types[0].dim;
            return {
                type: type,
                ndim: ndim,
                dim: [dim[1], dim[0]],
                ismatrix: arg_types[0].ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    }
];
exports.builtin_functions = [
    {
        fun_matlab: 'isequal',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'isEqualM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'bool',
                ndim: 2,
                dim: [1, 1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /sin|cos|tan/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (arg_types[0].ismatrix) {
                return 'fun_matlabM';
            }
            else {
                return 'fun_matlab';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
            }
            else {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /sind|cosd|tand/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (arg_types[0].ismatrix) {
                return 'fun_matlabM';
            }
            else {
                return 'fun_matlab';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (arg_types[0].ismatrix) {
                return args;
            }
            else {
                return ["fmod((".concat(args[0], "),360) * M_PI / 180")];
            }
        },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
            }
            else {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'xcorr',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'xcorrM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var x = args[0];
            var y = null;
            var maxlag = 0;
            var scale = "none";
            var scale_opts = ["none", "biased", "unbiased", "normalized", "coeff"];
            var match = undefined;
            var i = 0;
            while (match == undefined && i < scale_opts.length) {
                match = args.find(function (x) { return x.includes(scale_opts[i]); });
                i++;
            }
            if (match != null && match != undefined) {
                if (match == "normalized") {
                    scale = "coeff";
                }
                else {
                    scale = match.replace(/'/g, '"');
                }
            }
            if (args.indexOf(match) == 3) {
                y = args[1];
                maxlag = args[2];
            }
            else if (args.indexOf(match) == 2) {
                y = args[1];
            }
            return [x, y, maxlag, scale];
        },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 4,
        n_opt_args: 3,
        //opt_arg_defaults: ['null', '0', 'none'],
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'sort',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'sortM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args) {
            var args_transformed = [];
            for (var i = 0; i < args.length; i++) {
                if (i == 0) {
                    args_transformed.push(args[i]);
                }
                else {
                    if (parseCharArg(args[i]) == 'ascend') {
                        args_transformed.push('0');
                    }
                    else if (parseCharArg(args[i]) == 'descend') {
                        args_transformed.push('1');
                    }
                }
            }
            return args_transformed;
        },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['0'],
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            var ndim = arg_types[0].ndim;
            var dim = arg_types[0].dim;
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        // [h,p,ci,stats] = ttest(___)  
        fun_matlab: 'ttest',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (arg_types.length >= 2) {
                if (arg_types[1].ismatrix) {
                    return 'ttestM_xy';
                }
            }
            return 'ttestM';
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return null; },
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['0'],
        ptr_args: function (arg_types, outs) {
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
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true
                },
                {
                    name: arg_pval,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true
                },
                {
                    name: arg_ci,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 2],
                    ismatrix: false,
                    isvector: true,
                    ispointer: true
                },
                {
                    name: 'tstat',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true
                },
                {
                    name: 'df',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true
                },
                {
                    name: 'sd',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true
                },
            ];
        },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return [
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
        ]; }
    },
    {
        fun_matlab: 'ztest',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'ztestM'; },
        req_arg_types: function (args, arg_types, outs) { return [
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
        ]; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return null; },
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_pval,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_ci,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 2],
                    ismatrix: false,
                    isvector: true,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_z,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: 'zcrit',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                }
            ];
        },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'vartest',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'vartestM'; },
        req_arg_types: function (args, arg_types, outs) { return [
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
        ]; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_pval,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_ci,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 2],
                    ismatrix: false,
                    isvector: true,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: 'chisqstat',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: 'df',
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                }
            ];
        },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'betapdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'betapdfM'; },
        req_arg_types: function (args, arg_types, outs) { return [
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
        ]; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'exppdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'exppdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 1,
        opt_arg_defaults: ['1'],
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'chi2pdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'chi2pdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'gampdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'gampdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 3,
        n_opt_args: 1,
        opt_arg_defaults: ['1'],
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'lognpdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'lognpdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 3,
        n_opt_args: 2,
        opt_arg_defaults: ['0', '1'],
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'normpdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'normpdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 3,
        n_opt_args: 2,
        opt_arg_defaults: ['0', '1'],
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'unidpdf',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'unidpdfM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var left_type = arg_types[0].type;
            var left_ndim = arg_types[0].ndim;
            var left_dim = arg_types[0].dim;
            var right_type = arg_types[1].type;
            var right_ndim = arg_types[1].ndim;
            var right_dim = arg_types[1].dim;
            return {
                type: (0, customTypes_1.binaryOpType)(left_type, right_type),
                ndim: left_ndim,
                dim: left_dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'normfit',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'normfitM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
        return_type: function (arg_types) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'unifit',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'unifitM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                },
                {
                    name: arg_b,
                    type: 'double',
                    ndim: 2,
                    dim: [1, 1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                }
            ];
        },
        return_type: function (arg_types) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'filter',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'filterM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (args.length == 3) {
                args.push("&zero");
            }
            return args;
        },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 3,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[2].type;
            var ndim = arg_types[2].ndim;
            var dim = arg_types[2].dim;
            return {
                type: "complex",
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            if (args.length == 3) {
                var init_var = [];
                var state_size = Math.max((0, helperFunctions_1.numel)(arg_types[0].dim), (0, helperFunctions_1.numel)(arg_types[1].dim));
                init_var.push({
                    name: "state_size",
                    //val: `{(int) ${state_size - 1}}`,
                    val: "{(int) fmax(getsizeM(".concat(args[0], "), getsizeM(").concat(args[1], ")) - 1}"),
                    type: 'int',
                    ndim: 1,
                    dim: [state_size],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                });
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
                });
                return init_var;
            }
            else {
                return null;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'eig',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'eigM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return null; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
                    ispointer: false,
                    isstruct: false
                },
                {
                    name: arg_evecs,
                    type: 'complex',
                    ndim: arg_types[0].ndim,
                    dim: arg_types[0].dim,
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                }
            ];
        },
        return_type: function (arg_types) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) {
            var expression = [];
            expression.push("".concat(outs[1], " = scaleM(evals, &complex_one, COMPLEX);"));
            expression.push("".concat(outs[0], " = scaleM(evecs, &complex_one, COMPLEX);"));
            return expression.join("\n");
        },
        init_before: function (args, arg_types, outs) {
            var init_var = [];
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
            });
            init_var.push({
                name: outs[0],
                val: "NULL",
                type: 'complex',
                ndim: 2,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            });
            init_var.push({
                name: outs[1],
                val: "NULL",
                type: 'complex',
                ndim: 2,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            });
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /abs|round|floor|ceil/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'fun_matlabM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            var ndim = arg_types[0].ndim;
            var dim = arg_types[0].dim;
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /\bmax\b|\bmin\b/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (outs.length > 1) {
                return 'fun_matlabV';
            }
            else if (args.length > 1) {
                if (arg_types[0].ismatrix && arg_types[1].ismatrix) {
                    return 'pairwise_fun_matlabM';
                }
            }
            return 'fun_matlabM';
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) {
            return outs[0];
        },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
            }
            else {
                return null;
            }
        },
        return_type: function (args, arg_types, outs) {
            var type = arg_types[0].type;
            return {
                type: type,
                ndim: 2,
                dim: [1, 1],
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /std|var/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (args.length == 2) {
                if (args[1] == "1") {
                    return 'popfun_matlabM';
                }
            }
            return 'fun_matlabM';
        },
        args_transform: function (args, arg_types, outs) {
            if (args.length == 3) {
                if (args[2] == "2") {
                    return ["mat"];
                }
            }
            return [args[0]];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = "double";
            if (arg_types[0].type == "complex") {
                type = "complex";
            }
            var dim = [1, 1];
            var ndim = 2;
            if (!dim.some(function (x) { return x === 1; })) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            if (args.length == 3) {
                if (args[2] == "2") {
                    var init_var = [];
                    init_var.push({
                        name: 'mat',
                        val: "transposeM(".concat(args[0], ")"),
                        type: arg_types[0].type,
                        ndim: arg_types[0].ndim,
                        dim: arg_types[0].dim.reverse(),
                        ismatrix: true,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    });
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'quantile',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'quantileM_vec'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var quantile_specifier = Number(args[1]);
            var quantiles = "vec";
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles = quantile_specifier;
            }
            else {
                var n_quantiles = (0, helperFunctions_1.numel)(arg_types[1].dim);
                if (!arg_types[1].ismatrix) {
                    quantiles = args[1];
                }
            }
            return [args[0], n_quantiles, quantiles];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var arg0_type = arg_types[0].type;
            var arg0_ndim = arg_types[0].ndim;
            var arg0_dim = arg_types[0].dim;
            var quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles = quantile_specifier;
            }
            else {
                var n_quantiles = (0, helperFunctions_1.numel)(arg_types[1].dim);
            }
            // If m is a row or column vector, returns a 1xN matrix with the specified quantiles
            // If m is a matrix of size rxc, returns a Nxc matrix with the specified quantiles of each column
            if (arg0_ndim == 2 && arg0_dim.some(function (x) { return x === 1; })) { // vector
                return {
                    type: arg0_type,
                    ndim: 2,
                    dim: [1, n_quantiles],
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
            else {
                return {
                    type: arg0_type,
                    ndim: 2,
                    dim: [n_quantiles, arg0_dim[1]],
                    ismatrix: true,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
        },
        push_main_before: function (args, arg_types, outs) {
            var quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
                var n_quantiles = quantile_specifier;
                //let expression = [];
                //expression.push(`double arr[${n_quantiles}];`);
                var step = 1 / (quantile_specifier + 1);
                var expression = "\nfor (int i = 1; ".concat(step, "*i < 1; i ++) {\n    vec[i-1] = ").concat(step, "*i;\n}\n                ");
                return expression;
            }
            else {
                return null;
            }
        },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var quantile_specifier = Number(args[1]);
            if (Number.isInteger(quantile_specifier)) {
                var init_var = [];
                init_var.push({
                    name: 'vec',
                    val: "{}",
                    type: 'double',
                    ndim: 1,
                    dim: [quantile_specifier],
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false
                });
                return init_var;
            }
            else {
                if (arg_types[1].ismatrix) {
                    var init_var = [];
                    var type = arg_types[1].type;
                    init_var.push({
                        name: 'vec',
                        val: "".concat(type.charAt(0), "_to_").concat(type.charAt(0), "(").concat(args[1], ")"),
                        type: type,
                        ndim: 1,
                        dim: [(0, helperFunctions_1.numel)(arg_types[1].dim)],
                        ismatrix: false,
                        isvector: false,
                        ispointer: true,
                        isstruct: false
                    });
                    return init_var;
                }
                return null;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /zeros|ones/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'fun_matlabM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (arg_types[0].ispointer) {
                return ['ndim', args[0]];
            }
            return ['ndim', 'dim'];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [];
            var ndim = args.length;
            if (arg_types[0].ispointer) {
                dim = arg_types[0].dim;
                ndim = arg_types[0].ndim;
            }
            else {
                if (args.length == 1) {
                    dim = [Number(args[0]), Number(args[0])];
                    ndim = 2;
                }
                else {
                    for (var _i = 0, args_1 = args; _i < args_1.length; _i++) {
                        var arg = args_1[_i];
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
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var dim = "{".concat(args.join(", "), "}");
            var ndim = args.length;
            var init_var = [];
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
                });
            }
            else {
                if (args.length == 1) {
                    dim = "{".concat(args[0], ",").concat(args[0], "}");
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
                });
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
                });
            }
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'eye',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'identityM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return [args[0]]; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [Number(args[0]), Number(args[0])];
            var ndim = 2;
            return {
                type: 'int',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'reshape',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'reshapeM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            return [args[0], 'ndim', 'dim'];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [];
            var ndim = 2;
            if (arg_types[1].ispointer) {
                dim = arg_types[1].dim;
                ndim = arg_types[1].ndim;
            }
            else {
                var vec_el = parseVectorArg(args[1]);
                ndim = vec_el.length;
                if (ndim != 0) {
                    for (var _i = 0, vec_el_1 = vec_el; _i < vec_el_1.length; _i++) {
                        var v = vec_el_1[_i];
                        var isnum = !/^[a-z].*$/.test(v);
                        if (isnum) {
                            dim.push(Number(v));
                        }
                        else {
                            dim.push(v);
                        }
                    }
                }
                else {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        /*push_main_after: (args, arg_types, outs) => {
            return `${outs[0]} = ${args[0]};`
        },*/
        init_before: function (args, arg_types, outs) {
            var init_var = [];
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
                var type = arg_types[1].type;
                init_var.push({
                    name: 'dim',
                    val: "".concat(type.charAt(0), "_to_").concat(type.charAt(0), "(").concat(args[1], ")"),
                    type: type,
                    ndim: 1,
                    dim: [(0, helperFunctions_1.numel)(arg_types[1].dim)],
                    ismatrix: false,
                    isvector: false,
                    ispointer: true,
                    isstruct: false
                });
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
                });
            }
            else {
                var dim = "{".concat(args.slice(1).join(", "), "}");
                var ndim = args.slice(1).length;
                if (args.length == 2) {
                    dim = "{".concat(args[1], ",").concat(args[1], "}");
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
                });
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
                });
            }
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'det',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'detM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) {
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
                    ispointer: true,
                    isstruct: false
                }
            ];
        },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'inv',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'invertM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: "double",
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'cell',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return null; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [];
            var ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            }
            else {
                for (var _i = 0, args_2 = args; _i < args_2.length; _i++) {
                    var arg = args_2[_i];
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
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) {
            var dim = [];
            var ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            }
            else {
                for (var _i = 0, args_3 = args; _i < args_3.length; _i++) {
                    var arg = args_3[_i];
                    dim.push(Number(arg));
                }
            }
            var numel = dim.reduce(function (a, b) { return a * b; });
            return "\nMatrix **".concat(outs[0], " = NULL;\n").concat(outs[0], " = malloc(").concat(numel, "*sizeof(*").concat(outs[0], "));\n\t        ");
        },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'strcmp',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'strcmp'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'int',
                ndim: 2,
                dim: [1, 1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'strcmpi',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'strcmpi'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 2,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'int',
                ndim: 2,
                dim: [1, 1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'struct',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        //return_type: (args, arg_types, outs) => null,
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'rmfield',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        //return_type: (args, arg_types, outs) => null,
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'builtin',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        //return_type: (args, arg_types, outs) => null,
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'all',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        //return_type: (args, arg_types, outs) => null,
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    // TO DO: make separate entry for length, length often used similarly to numel but not always
    {
        fun_matlab: /numel|length/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'getsizeM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'int',
                ndim: 2,
                dim: [1, 1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'size',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'getDimsM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return [args[0]]; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'int',
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: false,
                isvector: false,
                ispointer: true
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) {
            if (args.length > 1) {
                return "tmp_out[".concat(args[1] - 1, "]");
            }
            return null;
        },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
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
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'prod',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'error',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'permute',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'rand',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var match = args.find(function (x) { return x.includes('seed'); });
            if (match != null && match != undefined) {
                //let tmp = args[args.indexOf(match)];
                return 'randomSeed';
            }
            else {
                return 'randM';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var match = args.find(function (x) { return x.includes('seed'); });
            if (match != null && match != undefined) {
                return ['void'];
            }
            else {
                if (arg_types[0].ispointer) {
                    return ['ndim', args[0]];
                }
                return ['ndim', 'dim'];
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) {
            var match = args.find(function (x) { return x.includes('seed'); });
            if (match != null && match != undefined) {
                return null;
            }
            else {
                return outs;
            }
        },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var match = args.find(function (x) { return x.includes('seed'); });
            if (match != null && match != undefined) {
                return null;
            }
            else {
                var dim = [];
                var ndim = args.length;
                if (arg_types[0].ispointer) {
                    dim = arg_types[0].dim;
                    ndim = arg_types[0].ndim;
                }
                else {
                    if (args.length == 1) {
                        dim = [Number(args[0]), Number(args[0])];
                        ndim = 2;
                    }
                    else {
                        for (var _i = 0, args_4 = args; _i < args_4.length; _i++) {
                            var arg = args_4[_i];
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
                    ispointer: false,
                    isstruct: false
                };
            }
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var match = args.find(function (x) { return x.includes('seed'); });
            if (match != null && match != undefined) {
                return null;
            }
            else {
                var dim = "{".concat(args.join(", "), "}");
                var ndim = args.length;
                var init_var = [];
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
                    });
                }
                else {
                    if (args.length == 1) {
                        dim = "{".concat(args[0], ",").concat(args[0], "}");
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
                    });
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
                    });
                }
                return init_var;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'randi',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'randiM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            return ['ndim', 'dim', 0, args[0]];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [];
            var ndim = args.slice(1).length;
            if (args.length == 2) {
                dim = [Number(args[1]), Number(args[1])];
                ndim = 2;
            }
            else {
                for (var _i = 0, _a = args.slice(1); _i < _a.length; _i++) {
                    var arg = _a[_i];
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var dim = '';
            var ndim = args.slice(1).length;
            if (args.length == 2) {
                dim = "{".concat(args[1], ", ").concat(args[1], "}");
                ndim = 2;
            }
            else {
                dim = "{".concat(args.slice(1).join(", "), "}");
            }
            var init_var = [];
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
                });
            }
            else {
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
                });
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
                });
            }
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'randn',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'randnM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (arg_types[0].ispointer) {
                return ['ndim', args[0]];
            }
            return ['ndim', 'dim'];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = [];
            var ndim = args.length;
            if (args.length == 1) {
                dim = [Number(args[0]), Number(args[0])];
                ndim = 2;
            }
            else {
                for (var _i = 0, args_5 = args; _i < args_5.length; _i++) {
                    var arg = args_5[_i];
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var dim = "{".concat(args.join(", "), "}");
            var ndim = args.length;
            var init_var = [];
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
                });
            }
            else {
                if (args.length == 1) {
                    dim = "{".concat(args[0], ",").concat(args[0], "}");
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
                });
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
                });
            }
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'memmapfile',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (arg_types) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'median',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'medianM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'mean',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'meanM'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (args.length == 2) {
                if (args[1] == "2") {
                    return ["mat"];
                }
            }
            return [args[0]];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var type = "double";
            if (arg_types[0].type == "complex") {
                type = "complex";
            }
            var dim = [1, 1];
            var ndim = 2;
            if (!dim.some(function (x) { return x === 1; })) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            if (args.length == 2) {
                if (args[1] == "2") {
                    var init_var = [];
                    init_var.push({
                        name: 'mat',
                        val: "transposeM(".concat(args[0], ")"),
                        type: arg_types[0].type,
                        ndim: arg_types[0].ndim,
                        dim: arg_types[0].dim.reverse(),
                        ismatrix: true,
                        isvector: false,
                        ispointer: false,
                        isstruct: false
                    });
                    return init_var;
                }
            }
            return null;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'sqrt',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (arg_types[0].ismatrix) {
                return 'powerM';
            }
            else {
                return 'sqrt';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (arg_types[0].ismatrix) {
                var obj = type_to_matrix_type.find(function (x) { return x.type === 'double'; });
                return [args[0], '&exponent', obj.matrix_type.toString()];
            }
            else {
                return args;
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'complex',
                ndim: arg_types[0].ndim,
                dim: arg_types[0].dim,
                ismatrix: arg_types[0].ismatrix,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            if (arg_types[0].ismatrix) {
                var init_var = [];
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
                });
                return init_var;
            }
            return null;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /isa|orderfields/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /fullfile|fileparts|which/,
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'fun_matlab_toreplace'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'fieldnames',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'struct',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            return {
                type: 'unknown',
                ndim: 2,
                dim: [1, 1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: true
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'disp',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if ((arg_types[0].type != 'char' && arg_types[0].isvector) || arg_types[0].type == "dynamic" || arg_types[0].type.includes("_type") || args[0] == null) {
                return null; // args[0] == null occurs when calling disp(sprintf(...)) 
            }
            else if (arg_types[0].ismatrix) {
                return 'printM';
            }
            else {
                return 'printf';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            if (arg_types[0].ismatrix) {
                return args;
            }
            else {
                var format = '"\\n%d\\n"';
                //if (arg_types[0].type == 'double' || arg_types[0].type == 'complex') {
                if (arg_types[0].type == 'double') {
                    format = '"\\n%.4f\\n"';
                }
                else if (arg_types[0].type == 'complex') {
                    format = '"\\n%.4f + %.4fi\\n"';
                }
                else if (arg_types[0].type == 'int') {
                    format = '"\\n%d\\n"';
                }
                else if (arg_types[0].type == 'char') {
                    format = '"\\n%s\\n"';
                    args[0] = args[0].replace(/'/g, '"');
                }
                if (arg_types[0].type == 'complex') {
                    return [format, "creal(".concat(args[0], ")"), "cimag(".concat(args[0], ")")];
                }
                else {
                    return [format, args[0]];
                }
                //return [format, args[0]];
                //args.unshift(format);
                //return args;
            }
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        //push_main_before: (args, arg_types, outs) => null,
        push_main_before: function (args, arg_types, outs) {
            if (arg_types[0].type == "dynamic") {
                return "switch(".concat(args[0], ".type) {\n\tcase 0:\n\tprintf(\"\\n%d\\n\", ").concat(args[0], ".data.ival);\n\tbreak;\n        \n\tcase 1:\n\tprintf(\"\\n%f\\n\", ").concat(args[0], ".data.dval);\n\tbreak;\n        \n\tcase 2:\n\tprintf(\"\\n%f\\n\", ").concat(args[0], ".data.cval);\n\tbreak;\n        \n\tcase 3:\n\tprintf(\"\\n%s\\n\", ").concat(args[0], ".data.chval);\n\tbreak;\n}");
            }
            else if (arg_types[0].type.includes("_type")) {
                return "unknown_printf(&".concat(args[0], ");");
            }
            else if (arg_types[0].type != 'char' && arg_types[0].isvector) {
                var format = '"\\n%d\\n"';
                if (arg_types[0].type == 'double') {
                    format = '"\\n%.4f\\n"';
                }
                else if (arg_types[0].type == 'complex') {
                    format = '"\\n%.4f + %.4fi\\n"';
                }
                else if (arg_types[0].type == 'int') {
                    format = '"\\n%d\\n"';
                }
                return "for (int i = 0 ; i < ".concat(arg_types[0].dim[0], " ; i++) {\nprintf(").concat(format, ", ").concat(args[0], "[i]);\n}"); // to do: move free to generatecode.ts  free(${args[0]});`;       
            }
            else {
                return null;
            }
        },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'mod',
        fun_c: function (args, arg_types, outs, fun_matlab) { return "".concat(args[0], " % ").concat(args[1]); },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return null; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /real|imag/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            return 'cfun_matlab';
        },
        req_arg_types: function (args, arg_types, outs) { return [
            {
                type: 'complex',
                ndim: 1,
                dim: [1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: false
            }
        ]; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'sprintf',
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (args[0] == "disp") {
                return 'printf';
            }
            else {
                return 'sprintf';
            }
        },
        args_transform: function (args, arg_types, outs) {
            if (args[0] == "disp") {
                args.shift();
                for (var i = 0; i < args.length; i++) {
                    args[i] = args[i].replace(/'/g, '"');
                }
                if (args.length == 1) {
                    return ['"\\n%s\\n"', args[0]];
                }
                args[0] = args[0].replace(/stdout/g, '"\\n%s\\n"');
                return args;
            }
            var buffer = 'str';
            if (outs.length > 0) {
                buffer = outs[0];
            }
            for (var i = 0; i < args.length; i++) {
                args[i] = args[i].replace(/'/g, '"');
            }
            if (args.length == 1) {
                return [buffer, '"\\n%s\\n"', args[0]];
            }
            args[0] = args[0].replace(/stdout/g, '"\\n%s\\n"');
            args.unshift(buffer);
            return args;
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        req_arg_types: function (args, arg_types, outs) { return [
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
        ]; },
        outs_transform: function (args, arg_types, outs) { return null; },
        n_req_args: 1,
        n_opt_args: 0,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) { return null; },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            if (outs.length > 0) {
                var init_var = [];
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
                });
                return init_var;
            }
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /fft|ifft/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            if (args.length == 2) {
                if (arg_types[1].type == 'int') {
                    return 'fun_matlabV';
                }
            }
            return 'fun_matlabM';
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return args; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = arg_types[0].dim;
            var ndim = arg_types[0].ndim;
            if (args.length == 2) {
                console.log("WARNING: fftM dimensions adjusted");
            }
            return {
                type: 'double',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'stft',
        fun_c: function (args, arg_types, outs, fun_matlab) { return 'stftV'; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) {
            var win_type = 1;
            var num_coef = '128';
            var inc = '24';
            var win_size = '80';
            var match = args.find(function (x) { return x.includes('Window'); });
            if (match != null && match != undefined) {
                var tmp = args[args.indexOf(match) + 1];
                var match11 = tmp.match(/(\S+)\(/);
                var tmp2 = match11[1];
                var re = new RegExp("".concat(tmp2, "\\((.*)[,)]"));
                var match12 = tmp.match(re);
                win_size = match12[1];
                if (tmp2 === "hanning") {
                    win_type = 1;
                }
                else if (tmp2 === "hamming") {
                    win_type = 2;
                }
                else if (tmp2 === "rectangle") {
                    win_type = 3;
                }
            }
            var match2 = args.find(function (x) { return x.includes('OverlapLength'); });
            if (match2 != null && match2 != undefined) {
                inc = 'inc';
            }
            var match3 = args.find(function (x) { return x.includes('FFTLength'); });
            if (match3 != null && match3 != undefined) {
                num_coef = 'num_coef';
            }
            return [args[0], win_size, inc, num_coef, win_type];
        },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            var dim = arg_types[0].dim;
            var ndim = arg_types[0].ndim;
            return {
                type: 'double',
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false
            };
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        init_before: function (args, arg_types, outs) {
            var init_var = [];
            var num_coef = '128';
            var inc = '24';
            var win_size = '80';
            var match = args.find(function (x) { return x.includes('Window'); });
            if (match != null && match != undefined) {
                var tmp = args[args.indexOf(match) + 1];
                var match11 = tmp.match(/(\S+)\(/);
                var tmp2 = match11[1];
                var re = new RegExp("".concat(tmp2, "\\((.*)[,)]"));
                var match12 = tmp.match(re);
                win_size = match12[1];
            }
            var match2 = args.find(function (x) { return x.includes('OverlapLength'); });
            var overlap = '0';
            if (match2 != null && match2 != undefined) {
                overlap = args[args.indexOf(match2) + 1];
            }
            var match3 = args.find(function (x) { return x.includes('FFTLength'); });
            if (match3 != null && match3 != undefined) {
                num_coef = args[args.indexOf(match3) + 1];
                num_coef = num_coef.concat("/2");
            }
            init_var.push({
                name: 'inc',
                val: "".concat(win_size, " - ").concat(overlap),
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
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: /hamming|hanning/,
        fun_c: function (args, arg_types, outs, fun_matlab) {
            var match = args.find(function (x) { return x.includes("periodic"); });
            if (match !== null && match !== undefined) {
                return 'periodicfun_matlab';
            }
            else {
                return 'fun_matlab';
            }
        },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return [args[0]]; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
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
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) { return null; },
        //init_before: (args, arg_types, outs) => null,
        init_before: function (args, arg_types, outs) {
            var init_var = [];
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
            }
            else {
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
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'tic',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return null; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            if (outs.length == 1) {
                return {
                    type: 'time_t',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
            else {
                return null;
            }
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) {
            return 'tstart = time(0);';
        },
        init_before: function (args, arg_types, outs) {
            // time_t tstart, tend;
            var init_var = [];
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
            });
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
            });
            return init_var;
        },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    },
    {
        fun_matlab: 'toc',
        fun_c: function (args, arg_types, outs, fun_matlab) { return null; },
        req_arg_types: function (args, arg_types, outs) { return null; },
        args_transform: function (args, arg_types, outs) { return null; },
        arg_types_transform: function (args, arg_types, outs) { return null; },
        outs_transform: function (args, arg_types, outs) { return outs[0]; },
        n_req_args: null,
        n_opt_args: null,
        opt_arg_defaults: null,
        ptr_args: function (arg_types, outs) { return null; },
        return_type: function (args, arg_types, outs) {
            if (outs.length == 1) {
                return {
                    type: 'double',
                    ndim: 1,
                    dim: [1],
                    ismatrix: false,
                    isvector: false,
                    ispointer: false,
                    isstruct: false
                };
            }
            else {
                return null;
            }
        },
        push_main_before: function (args, arg_types, outs) { return null; },
        push_main_after: function (args, arg_types, outs) {
            var expression = [];
            expression.push('tend = time(0);');
            if (outs.length == 1) {
                expression.push("double ".concat(outs[0], " = difftime(tend, tstart)"));
            }
            return 'tend = time(0);';
            // difftime(tend, tstart)
        },
        init_before: function (args, arg_types, outs) { return null; },
        tmp_out_transform: function (args, arg_types, outs) { return null; },
        push_alias_tbl: function (args, arg_types, outs) { return null; }
    }
];
//# sourceMappingURL=builtinFunctions.js.map