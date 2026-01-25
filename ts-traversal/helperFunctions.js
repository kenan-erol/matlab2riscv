"use strict";
exports.__esModule = true;
exports.parseFunctionDefNode = exports.getClasses = exports.getClassFolders = exports.getNonClassFilesInPath = exports.getFilesInPath = exports.writeToFile = exports.initVar = exports.numel = exports.transformNodeByName = exports.findLastSubscript = exports.pushAliasTbl = exports.filterByScope = exports.generateTmpVar = exports.extractSingularMat = exports.findBuiltin = exports.isInitialized = void 0;
//const fs = require("graceful-fs");
var fs = require('fs');
var gracefulFs = require('graceful-fs');
gracefulFs.gracefulify(fs);
var path = require("path");
var glob = require("glob");
var identifyCustomFunctions_1 = require("./identifyCustomFunctions");
var typeInference_1 = require("./typeInference");
var treeTraversal_1 = require("./treeTraversal");
var modifyCode_1 = require("./modifyCode");
var Parser = require("tree-sitter");
var Matlab = require("tree-sitter-matlab");
//import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");
var parser = new Parser();
parser.setLanguage(Matlab);
function compare(a, b) {
    //console.log("compare");
    if (a.scope[0] < b.scope[0]) {
        return -1;
    }
    if (a.scope[0] > b.scope[0]) {
        return 1;
    }
    return 0;
}
function isInitialized(name, node, type, fun_params) {
    //console.log("isiinit");
    var scope = (0, typeInference_1.findVarScope)(fun_params.filename, node, fun_params.block_idxs, fun_params.current_code, fun_params.debug);
    var var_type = filterByScope(fun_params.var_types, name, node, 0);
    var all_var_type = fun_params.var_types.filter(function (x) { return x.name == name && x.scope[2] == scope[2]; });
    all_var_type.sort(compare);
    var flag1 = false;
    var flag2 = false;
    if (var_type != null && var_type != undefined) {
        var idx_1 = all_var_type.findIndex(function (x) { return JSON.stringify(x) == JSON.stringify(var_type); });
        flag1 = (var_type.initialized && (var_type.ismatrix || var_type.type == type));
        if (idx_1 > 0) {
            if (all_var_type[idx_1 - 1].initialized && (var_type.ismatrix || var_type.type == all_var_type[idx_1 - 1].type)) {
                flag1 = true;
                var alias_obj = fun_params.alias_tbl.find(function (x) { return x.name == name && all_var_type[idx_1 - 1].scope[0] >= x.scope[0] && all_var_type[idx_1 - 1].scope[1] <= x.scope[1]; });
                if (alias_obj != null && alias_obj != undefined) {
                    fun_params.alias_tbl.push({
                        name: name,
                        tmp_var: alias_obj.tmp_var,
                        scope: var_type.scope
                    });
                    name = alias_obj.tmp_var;
                }
            }
            if (all_var_type[idx_1 - 1].initialized && all_var_type.length > 1) {
                flag2 = true;
            }
        }
    }
    return [name, var_type, flag1, flag2, fun_params];
}
exports.isInitialized = isInitialized;
function findBuiltin(builtin_funs, name) {
    return builtin_funs.find(function (x) {
        var found = -1;
        if (x.fun_matlab instanceof RegExp) {
            found = name.search(x.fun_matlab);
        }
        else {
            if (x.fun_matlab == name) {
                found = 1;
            }
        }
        return found !== -1;
    });
}
exports.findBuiltin = findBuiltin;
function extractSingularMat(mat, var_type, node, fun_params) {
    //console.log("extract");
    var obj = fun_params.alias_tbl.find(function (x) { return x.name === mat && x.tmp_var.includes("[0]") &&
        node.startIndex > x.scope[0] && node.endIndex < x.scope[1]; });
    if (obj == null || obj == undefined) {
        var tmp_var = generateTmpVar("tmp", fun_params.tmp_tbl);
        fun_params.var_types.push({
            name: tmp_var,
            type: var_type.type,
            ndim: 1,
            dim: [1],
            ismatrix: false,
            isvector: false,
            ispointer: true,
            isstruct: false,
            initialized: true,
            scope: (0, typeInference_1.findVarScope)(fun_params.filename, node, fun_params.block_idxs, fun_params.current_code, fun_params.debug)
        });
        fun_params.var_types.push({
            name: "".concat(tmp_var, "[0]"),
            type: var_type.type,
            ndim: 1,
            dim: [1],
            ismatrix: false,
            isvector: false,
            ispointer: false,
            isstruct: false,
            initialized: true,
            scope: (0, typeInference_1.findVarScope)(fun_params.filename, node, fun_params.block_idxs, fun_params.current_code, fun_params.debug)
        });
        fun_params.alias_tbl = pushAliasTbl(mat, "".concat(tmp_var, "[0]"), node, fun_params);
        var _a = (0, modifyCode_1.pushToMain)("".concat(var_type.type, " * ").concat(tmp_var, " = ").concat(var_type.type.charAt(0), "_to_").concat(var_type.type.charAt(0), "(").concat(mat, ");"), fun_params), main_function = _a[0], function_definitions = _a[1];
        fun_params.main_function = main_function;
        fun_params.function_definitions = function_definitions;
        return ["".concat(tmp_var, "[0]"), fun_params];
    }
    else {
        return [obj.tmp_var, fun_params];
    }
}
exports.extractSingularMat = extractSingularMat;
function generateTmpVar(name, tmp_tbl) {
    var obj = tmp_tbl.find(function (x) { return x.name === name; });
    if (obj != null && obj != undefined) {
        obj.count = obj.count + 1;
        tmp_tbl = tmp_tbl.filter(function (e) { return e.name !== obj.name; });
        tmp_tbl.push(obj);
    }
    else {
        tmp_tbl.push({
            name: name,
            count: 1
        });
    }
    return "".concat(tmp_tbl[tmp_tbl.length - 1].name).concat(tmp_tbl[tmp_tbl.length - 1].count);
}
exports.generateTmpVar = generateTmpVar;
function filterByScope(obj, name, node, find_or_filter) {
    var obj2 = obj.filter(function (x) { return x.scope != null; });
    obj2 = obj2.filter(function (x) { return x.name == name && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]; });
    /*if (name == "c") {
        console.log("filterByScope");
        console.log(node.text);
        console.log([node.startIndex, node.endIndex]);
        console.log(obj.filter(x=>x.name=="c"));
        console.log(obj2);
        console.log("--------------------------");
    }*/
    if (find_or_filter == 0) {
        if (obj2.length > 1) {
            return obj2.reduce(function (prev, curr) {
                return (prev.scope[1] - prev.scope[0]) < (curr.scope[1] - curr.scope[0]) ? prev : curr;
            });
        }
        else if (obj2.length == 1) {
            return obj2[0];
        }
        return null;
        //return obj.find(x => x.name == name && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]);
    }
    else if (find_or_filter == 1) {
        //return obj.filter(x => x.name == name && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]);
        return obj2;
    }
}
exports.filterByScope = filterByScope;
function pushAliasTbl(lhs, rhs, node, fun_params) {
    var scope = (0, typeInference_1.findVarScope)(fun_params.filename, node, fun_params.block_idxs, fun_params.current_code, fun_params.debug);
    var obj = filterByScope(fun_params.var_types, lhs, node, 0);
    /*if (obj !== null && obj !== undefined) {
        scope = obj.scope;
    }*/
    fun_params.alias_tbl = fun_params.alias_tbl.filter(function (e) {
        return (e.name !== lhs) ||
            ((e.name == lhs) && (e.scope[0] !== scope[0]) && (e.scope[1] !== scope[1]));
    });
    fun_params.alias_tbl.push({
        name: lhs,
        tmp_var: rhs,
        scope: scope
    });
    return fun_params.alias_tbl;
}
exports.pushAliasTbl = pushAliasTbl;
function findLastSubscript(node, fun_params) {
    //console.log("findlast");
    // for a valueNode with text "m" finds all nodes of form m(...) = ...
    // helps with figuring out when to insert writeM(m) code since we want to do that
    // after all values have been assigned to the matrix m
    //let matches = [];
    var subscript_text = null;
    var subscript_idx = null;
    var re = new RegExp("".concat(node.text, "\\(([\\s\\w+\\-\\*]*)\\)(=| =)"));
    var scope = (0, typeInference_1.findVarScope)(fun_params.filename, node, fun_params.block_idxs, fun_params.current_code, fun_params.debug);
    var obj = filterByScope(fun_params.var_types, node.text, node, 0);
    if (obj !== null && obj !== undefined) {
        scope = obj.scope;
    }
    var cursor = fun_params.tree.walk();
    do {
        var c = cursor;
        var m = c.currentNode.text.match(re);
        if (c.currentNode.type == "assignment" /* g.SyntaxType.Assignment */) {
            if ((m != null) && (c.currentNode.startIndex >= scope[0]) && (c.currentNode.endIndex <= scope[1])) {
                //matches.push(m[0]);
                subscript_text = m[0];
                subscript_idx = c.currentNode.endIndex;
            }
        }
    } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, fun_params.debug));
    //return matches;
    return [subscript_text, subscript_idx];
}
exports.findLastSubscript = findLastSubscript;
function transformNodeByName(var_name, node, alias_tbl) {
    var obj = filterByScope(alias_tbl, var_name, node, 0);
    if (obj != null) {
        return obj.tmp_var;
    }
    return var_name;
}
exports.transformNodeByName = transformNodeByName;
function numel(x) {
    if (x == null) {
        //console.error("ERROR IN NUMEL: X IS NULL");
        return null;
    }
    var ans = x.reduce(function (a, b) { return a * b; });
    if (!x.some(function (x) { return isNaN(x); })) {
        //return `${ans}`;
        return ans;
    }
    var ans2 = "(".concat(x[0], ")");
    for (var i = 1; i < x.length; i++) {
        ans2 = ans2.concat("*(".concat(x[i], ")"));
    }
    return ans2;
}
exports.numel = numel;
function initVar(var_name, var_val, var_type) {
    var expression = '';
    if (var_type.type.includes("_type") && !var_type.ismatrix) {
        expression = "struct generic_val ".concat(var_name, ";");
        expression.concat("\nfillVal(&".concat(var_name, ", ").concat(var_type, ", &").concat(var_val, ");"));
        return expression;
    }
    if (var_type.isvector && var_val == null) {
        return "".concat(var_type.type, " *").concat(var_name, " = NULL;\n").concat(var_name, " = malloc(").concat(numel(var_type.dim), "*sizeof(*").concat(var_name, "));");
    }
    if (var_type.ismatrix && var_type.ispointer) {
        expression = "Matrix ** ".concat(var_name);
    }
    else if (var_type.ismatrix) {
        expression = "Matrix * ".concat(var_name);
    }
    else if (var_type.isvector) {
        if (var_type.dim.includes(NaN)) {
            expression = "".concat(var_type.type, " ").concat(var_name, "[]");
        }
        else {
            expression = "".concat(var_type.type, " ").concat(var_name, "[").concat(numel(var_type.dim), "]");
        }
    }
    else if (var_type.ispointer) {
        expression = "".concat(var_type.type, " * ").concat(var_name);
    }
    else {
        expression = "".concat(var_type.type, " ").concat(var_name);
    }
    if (var_val !== null) {
        expression = expression.concat(" = ".concat(var_val, ";"));
    }
    else {
        if (var_type.ismatrix) {
            expression = expression.concat(" = NULL;");
        }
        else {
            expression = expression.concat(";");
        }
    }
    return expression;
}
exports.initVar = initVar;
function writeToFile(out_folder, filename, generated_code) {
    if (!fs.existsSync(out_folder)) {
        fs.mkdirSync(out_folder);
    }
    fs.writeFile(path.join(out_folder, filename), generated_code, function (err) {
        if (err) {
            console.error(err);
            return;
        }
    });
}
exports.writeToFile = writeToFile;
function getFilesInPath(src) {
    var files = glob.sync(src + '/**/*.m');
    return files;
}
exports.getFilesInPath = getFilesInPath;
;
function getNonClassFilesInPath(src) {
    var files1 = glob.sync(src + '/!(@)**/*.m');
    var files2 = glob.sync(src + '/*.m');
    return files1.concat(files2);
}
exports.getNonClassFilesInPath = getNonClassFilesInPath;
;
// https://www.mathworks.com/help/matlab/matlab_oop/organizing-classes-in-folders.html
function getClassFolders(src) {
    var folders = glob.sync(src + '/@**');
    return folders;
}
exports.getClassFolders = getClassFolders;
;
function getClasses(src, all_files, debug) {
    var _a;
    var folders = getClassFolders(src);
    var classes = [];
    var custom_functions = [];
    // Loop 1
    for (var _i = 0, folders_1 = folders; _i < folders_1.length; _i++) {
        var folder = folders_1[_i];
        var class_name = folder.substr(folder.indexOf("@") + 1);
        var files = getFilesInPath(folder);
        var methods = [];
        var properties = [];
        for (var _b = 0, files_1 = files; _b < files_1.length; _b++) {
            var file = files_1[_b];
            var sourceCode = fs.readFileSync(file, "utf8");
            var tree = parser.parse(sourceCode);
            methods = (0, identifyCustomFunctions_1.identifyCustomFunctions)(tree, methods, all_files, file, [], debug)[0];
            custom_functions = custom_functions.concat(methods.filter(function (x) { return !x.file.includes("@"); }));
            methods = methods.filter(function (x) { return x.file.includes("@"); });
            var filename = path.parse(file).name;
            if (filename == class_name) {
                var cursor = tree.walk();
                do {
                    var c_1 = cursor;
                    if (c_1.currentNode.type == "class_definition" /* g.SyntaxType.ClassDefinition */) {
                        var node = c_1.currentNode;
                        var properties_tree = parser.parse(node.propertiesNode.text);
                        _a = (0, typeInference_1.inferTypeFromAssignment)("main", properties_tree, [], [], [], file, [], debug, []), properties = _a[0];
                    }
                } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, debug));
            }
        }
        var _loop_1 = function (m) {
            methods = methods.filter(function (x) { return x.name != m.name; });
            for (var i = 0; i < m.arg_types.length; i++) {
                if (m.arg_types[i].name.includes("obj")) {
                    m.arg_types[i] = {
                        name: m.arg_types[i].name,
                        type: class_name,
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: true,
                        initialized: false,
                        scope: m.arg_types[i].scope
                    };
                }
            }
            methods.push(m);
        };
        for (var _c = 0, methods_1 = methods; _c < methods_1.length; _c++) {
            var m = methods_1[_c];
            _loop_1(m);
        }
        // Placeholder
        var c = {
            name: class_name,
            properties: properties,
            methods: methods,
            folder: folder
        };
        classes.push(c);
    }
    // Loop 2
    var classes2 = [];
    var _loop_2 = function (c1) {
        var files = getFilesInPath(c1.folder);
        var methods = c1.methods;
        var _loop_3 = function (file) {
            var _f, _g;
            // Update placeholders
            var filename = path.parse(file).name;
            if (filename == c1.name) {
                var constructor_method_1 = c1.methods.find(function (x) { return x.name == c1.name; });
                var classdef_file_methods = methods.filter(function (x) { return x.file == constructor_method_1.file; });
                for (var _h = 0, classdef_file_methods_1 = classdef_file_methods; _h < classdef_file_methods_1.length; _h++) {
                    var m = classdef_file_methods_1[_h];
                    var var_types = [];
                    var def_node = parseFunctionDefNode(m.def_node);
                    var tree = parser.parse(def_node.bodyNode.text);
                    var arg_types = m.arg_types;
                    for (var i = 0; i < arg_types.length; i++) {
                        arg_types[i].scope = [def_node.bodyNode.startIndex, def_node.bodyNode.endIndex, 0];
                    }
                    if (m.name == c1.name) {
                        var_types.push({
                            name: def_node.return_variableNode.firstNamedChild.text,
                            type: c1.name,
                            ndim: 1,
                            dim: [1],
                            ismatrix: false,
                            isvector: false,
                            ispointer: false,
                            isstruct: true,
                            initialized: false,
                            scope: [def_node.bodyNode.startIndex, def_node.bodyNode.endIndex, 0]
                        });
                    }
                    //[, methods] = getFunctionReturnType(filename, m.name, arg_types, [], methods, methods, classes, file, [], debug, 1);
                    _f = (0, typeInference_1.getFunctionReturnType)(filename, m.name, arg_types, [], methods, custom_functions, classes, file, [], debug, 1), methods = _f[1];
                }
            }
            else {
                //[, methods] = typeInference("main", file, methods, classes, debug);
                _g = (0, typeInference_1.typeInference)("main", file, custom_functions, classes, debug), custom_functions = _g[1];
            }
        };
        for (var _e = 0, files_2 = files; _e < files_2.length; _e++) {
            var file = files_2[_e];
            _loop_3(file);
        }
        var c = {
            name: c1.name,
            methods: methods,
            properties: c1.properties,
            folder: c1.folder
        };
        classes2.push(c);
    };
    for (var _d = 0, classes_1 = classes; _d < classes_1.length; _d++) {
        var c1 = classes_1[_d];
        _loop_2(c1);
    }
    return classes2;
}
exports.getClasses = getClasses;
;
function parseFunctionDefNode(node) {
    //console.log("parsefun");
    switch (node.type) {
        case "ERROR" /* g.SyntaxType.ERROR */: {
            var types = [];
            for (var _i = 0, _a = node.namedChildren; _i < _a.length; _i++) {
                var child = _a[_i];
                types.push(child.type);
            }
            if (types.includes("parameters" /* g.SyntaxType.Parameters */)) {
                var param_idx = types.indexOf("parameters" /* g.SyntaxType.Parameters */);
                var parametersNode = node.namedChildren[param_idx];
                var nameNode = node.namedChildren[param_idx - 1];
                var bodyNode = node.namedChildren[param_idx + 1];
                var return_variableNode = undefined;
                var return_idx = types.indexOf("return_value" /* g.SyntaxType.ReturnValue */);
                if (return_idx != null && return_idx != undefined) {
                    return_variableNode = node.namedChildren[return_idx];
                }
                //return [return_variableNode, nameNode, parametersNode, bodyNode];
                return {
                    type: "function_definition" /* g.SyntaxType.FunctionDefinition */,
                    return_variableNode: return_variableNode,
                    nameNode: nameNode,
                    parametersNode: parametersNode,
                    bodyNode: bodyNode,
                    startIndex: node.startIndex,
                    endIndex: node.endIndex
                };
            }
            else {
                return null;
            }
            break;
        }
        case "function_definition" /* g.SyntaxType.FunctionDefinition */: {
            //return [node.return_variableNode, node.nameNode, node.parametersNode, node.bodyNode];
            if (node.return_variableNode == undefined && node.namedChildren[0].type == "return_value" /* g.SyntaxType.ReturnValue */) {
                node.return_variableNode = node.namedChildren[0];
            }
            /*let types = [];
            for (let child of node.namedChildren) {
                types.push(child.type);
            }
            console.log(node.nextNamedSibling.text);
            if (types.includes(g.SyntaxType.Parameters)) {
                let param_idx = types.indexOf(g.SyntaxType.Parameters);
                var parametersNode = node.namedChildren[param_idx];
                var nameNode = node.namedChildren[param_idx - 1];
                var bodyNode = node.namedChildren[param_idx + 1];
                var return_variableNode = undefined;
                let return_idx = types.indexOf(g.SyntaxType.ReturnValue);
                if (return_idx != null && return_idx != undefined) {
                    return_variableNode = node.namedChildren[return_idx];
                }
                //return [return_variableNode, nameNode, parametersNode, bodyNode];
                return {
                    type: g.SyntaxType.FunctionDefinition,
                    return_variableNode: return_variableNode,
                    nameNode: nameNode,
                    parametersNode: parametersNode,
                    bodyNode: bodyNode
                };
            } else {
                return null;
            }*/
            return node;
            break;
        }
        default: {
            return null;
            break;
        }
    }
}
exports.parseFunctionDefNode = parseFunctionDefNode;
//# sourceMappingURL=helperFunctions.js.map