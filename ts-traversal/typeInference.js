"use strict";
exports.__esModule = true;
exports.getFunctionReturnType = exports.inferTypeFromAssignment = exports.inferTypeByName = exports.inferType = exports.typeInference = exports.findVarScope = void 0;
var fs = require('fs');
var treeTraversal_1 = require("./treeTraversal");
var helperFunctions_1 = require("./helperFunctions");
var customTypes_1 = require("./customTypes");
var builtinFunctions_1 = require("./builtinFunctions");
var helperFunctions_2 = require("./helperFunctions");
var Parser = require("tree-sitter");
//import Matlab = require("tree-sitter-matlab");
var Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");
var parser = new Parser();
parser.setLanguage(Matlab);
var dummy_count = 0;
var fun_obj = null;
var varargin = [];
var varargin_flag = false;
// Type inference
// -----------------------------------------------------------------------------
function findVarScope(filename, node, block_idxs, current_code, debug) {
    if (debug == 1) {
        console.log("findVarScope");
    }
    var entire_scope = block_idxs.find(function (x) { return x[2] == 0; });
    var good_blocks = [];
    if (current_code == filename) {
        //if (current_code == "main") {
        good_blocks = block_idxs.filter(function (e) { return e[2] >= 1; });
    }
    else {
        good_blocks = block_idxs.filter(function (e) { return e[2] == -1; });
    }
    var scope = good_blocks.filter(function (e) { return e[0] <= node.startIndex && e[1] >= node.endIndex; });
    scope = scope[scope.length - 1];
    if (scope != null && scope != undefined) {
        return scope;
    }
    //if (current_code == filename) {
    if (current_code == "main") {
        var fundef_blocks = block_idxs.filter(function (e) { return e[2] == -1; });
        if (fundef_blocks.length != 0) {
            if (node.startIndex >= entire_scope[0] && node.endIndex < fundef_blocks[0][0]) {
                return [entire_scope[0], fundef_blocks[0][0], 0];
            }
        }
        return [entire_scope[0], entire_scope[1], 0];
    }
}
exports.findVarScope = findVarScope;
function typeInference(filename, file, custom_functions, classes, debug) {
    var _a, _b;
    if (debug == 1) {
        console.log("typeInference");
    }
    var class_method = false;
    var var_types = [];
    var block_idxs = [];
    var alias_tbl = [];
    var sourceCode = fs.readFileSync(file, "utf8");
    var tree = parser.parse(sourceCode);
    var entry_fun_node = (0, treeTraversal_1.findEntryFunction)(tree, debug);
    if (entry_fun_node != null) {
        tree = parser.parse(entry_fun_node.bodyNode.text);
        var obj = custom_functions.find(function (x) { return x.name == entry_fun_node.nameNode.text; });
        if (obj != null) {
            _a = getFunctionReturnType(filename, obj.name, obj.arg_types, [], custom_functions, custom_functions, classes, file, alias_tbl, debug, 0), custom_functions = _a[1], block_idxs = _a[2];
            obj = custom_functions.find(function (x) { return x.name == entry_fun_node.nameNode.text; });
            /*custom_functions = custom_functions.filter(x => x.name != entry_fun_node.nameNode.text);
            for (let i = 0; i < obj.arg_types.length; i++) {
                obj.arg_types[i] = var_types.find(x=>x.name == obj.arg_types[i].name);
            }
            obj.var_types = var_types;
            custom_functions.push(obj);*/
            return [obj.var_types, custom_functions, block_idxs];
        }
        var class_name_1 = file.match(/(?<=@)(.*?)(?=\/)/);
        if (class_name_1 != null) {
            var class_obj = classes.find(function (x) { return x.name === class_name_1[0]; });
            if (class_obj != null && class_obj != undefined) {
                obj = class_obj.methods.find(function (x) { return x.name == entry_fun_node.nameNode.text; });
            }
            class_method = true;
        }
        var_types = var_types.concat(obj.arg_types);
        for (var i = 0; i < var_types.length; i++) {
            var_types[i].scope = [entry_fun_node.startIndex, entry_fun_node.endIndex, 0];
        }
        /*let class_name = file.match(/(?<=@)(.*?)(?=\/)/);
        if (class_name != null) {
            let class_obj = classes.find(x => x.name === class_name[0]);
            // Assumes first argument to method is class object
            var_types.push({
                name: entry_fun_node.parametersNode.namedChildren[0].text,
                type: class_name[0],
                ndim: 2,
                dim: [1,1],
                ismatrix: false,
                isvector: false,
                ispointer: false,
                isstruct: true,
                initialized: false,
                //scope: findVarScope("main", entry_fun_node, block_idxs, "main", debug) // come back here
                scope: findVarScope("main", entry_fun_node, [[entry_fun_node.startIndex, entry_fun_node.endIndex, 0]], "main", debug) // come back here
            });
            for (let i = 0; i < class_obj.properties.length; i ++) {
                let property = JSON.parse(JSON.stringify(class_obj.properties[i]));
                property.name = `${entry_fun_node.parametersNode.namedChildren[0].text}.${property.name}`;
                var_types.push(property);
            }
            //var_types = var_types.concat(class_obj.properties);
            //console.log(var_types);
            //console.log("----------------------------");
        }*/
    }
    _b = inferTypeFromAssignment(filename, tree, var_types, custom_functions, classes, file, alias_tbl, debug, []), var_types = _b[0], custom_functions = _b[1], block_idxs = _b[2];
    if (entry_fun_node != null) {
        if (class_method) {
            var class_name_2 = file.match(/(?<=@)(.*?)(?=\/)/);
            var class_obj = classes.find(function (x) { return x.name === class_name_2[0]; });
            var obj_1 = class_obj.methods.find(function (x) { return x.name == entry_fun_node.nameNode.text; });
            class_obj.methods = class_obj.methods.filter(function (x) { return x.name != entry_fun_node.nameNode.text; });
            var _loop_1 = function (i) {
                obj_1.arg_types[i] = var_types.find(function (x) { return x.name == obj_1.arg_types[i].name; });
            };
            for (var i = 0; i < obj_1.arg_types.length; i++) {
                _loop_1(i);
            }
            obj_1.var_types = var_types;
            class_obj.methods.push(obj_1);
        }
        else {
            var obj_2 = custom_functions.find(function (x) { return x.name == entry_fun_node.nameNode.text; });
            custom_functions = custom_functions.filter(function (x) { return x.name != entry_fun_node.nameNode.text; });
            var _loop_2 = function (i) {
                obj_2.arg_types[i] = var_types.find(function (x) { return x.name == obj_2.arg_types[i].name; });
            };
            for (var i = 0; i < obj_2.arg_types.length; i++) {
                _loop_2(i);
            }
            obj_2.var_types = var_types;
            custom_functions.push(obj_2);
        }
    }
    return [var_types, custom_functions, block_idxs];
}
exports.typeInference = typeInference;
// to do: add tree_idx = [node.startIndex, node.endIndex]
// then after traversing tree make sure each variable's scope is updated to scope + tree_idx[0] 
function inferTypeFromAssignment(filename, tree, var_types, custom_functions, classes, file, alias_tbl, debug, block_idxs) {
    if (debug == 1) {
        console.log("inferTypeFromAssignment");
    }
    var text = tree.rootNode.text;
    //let block_idxs = [];
    var fun_flag = false;
    var fun_def_flag = false;
    var scaler = 0;
    if (block_idxs.length > 0) {
        // examining a function definition
        fun_flag = true;
        scaler = block_idxs[0][0];
    }
    var block_level = 0;
    var cursor = tree.walk();
    var _loop_3 = function () {
        var c = cursor;
        switch (c.nodeType) {
            case "module" /* g.SyntaxType.Module */: {
                fun_def_flag = false;
                var node = c.currentNode;
                block_idxs.push([node.startIndex, node.endIndex, 0]); // 0 indicates entire program
                break;
            }
            case "block" /* g.SyntaxType.Block */: {
                var node_1 = c.currentNode;
                if (block_idxs[block_idxs.length - 1][0] < node_1.startIndex && node_1.endIndex < block_idxs[block_idxs.length - 1][1]) {
                    block_level = block_level + 1;
                }
                else {
                    var prev_blocks = block_idxs.filter(function (e) { return e[1] < node_1.startIndex; });
                    if (prev_blocks.length != 0) {
                        var prev_block = prev_blocks.reduce(function (max, block) { return max[1] > block[1] ? max : block; });
                        block_level = prev_block[2];
                    }
                }
                if (fun_def_flag) {
                    block_level = -1;
                }
                block_idxs.push([node_1.startIndex, node_1.endIndex, block_level]); // 1 for regular blocks
                break;
            }
            case "function_definition" /* g.SyntaxType.FunctionDefinition */: {
                fun_def_flag = true;
                var node_2 = c.currentNode;
                fun_obj = custom_functions.find(function (x) { return x.name === node_2.nameNode.text; });
                block_idxs.push([node_2.startIndex, node_2.endIndex, -1]); // 2 for function def blocks 
                break;
            }
            case "comment" /* g.SyntaxType.Comment */: { // This helps update the varargin argument types of function definitions
                var node = c.currentNode;
                var _a = inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug), cf = _a[6], vt = _a[7];
                custom_functions = cf;
                var_types = vt;
                break;
            }
        }
    };
    do {
        _loop_3();
    } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, debug));
    cursor = tree.walk();
    var _loop_4 = function () {
        var c = cursor;
        switch (c.nodeType) {
            case "attribute" /* g.SyntaxType.Attribute */: {
                var node_3 = c.currentNode;
                var scope = findVarScope(filename, node_3, block_idxs, "main", debug);
                for (var i = 0; i < classes.length; i++) {
                    var property = classes[i].properties.find(function (x) { return x.name === node_3.attributeNode.text; });
                    if (property !== null && property !== undefined) {
                        var class_property_count = 0;
                        for (var j = 0; j < classes[i].properties.length; j++) {
                            var object_node_text = node_3.objectNode.text;
                            object_node_text = object_node_text.replace("(", "\\(");
                            object_node_text = object_node_text.replace(")", "\\)");
                            var re = new RegExp("\\b".concat(object_node_text, "\\.").concat(classes[i].properties[j].name, "\\b"));
                            if (re.test(text)) {
                                class_property_count += 1;
                            }
                        }
                        if (class_property_count >= 2) {
                            var v1 = var_types.find(function (x) { return x.name == node_3.objectNode.text; });
                            if (v1 != null && v1 != undefined) {
                                scope = v1.scope;
                            }
                            var_types = var_types.filter(function (x) { return x.name != node_3.objectNode.text; });
                            var_types = var_types.filter(function (x) { return x.name != node_3.text; });
                            var_types.push({
                                name: node_3.objectNode.text,
                                type: classes[i].name,
                                ndim: 1,
                                dim: [1],
                                ismatrix: false,
                                isvector: false,
                                ispointer: false,
                                isstruct: true,
                                initialized: false,
                                scope: scope
                            });
                            var_types.push({
                                name: node_3.text,
                                type: property.type,
                                ndim: property.ndim,
                                dim: property.dim,
                                ismatrix: property.ismatrix,
                                isvector: (0, helperFunctions_1.numel)(property.dim) > 1 && !property.ismatrix,
                                ispointer: property.ispointer,
                                isstruct: property.isstruct,
                                initialized: false,
                                scope: scope
                            });
                        }
                    }
                }
            }
        }
    };
    do {
        _loop_4();
    } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, debug));
    var count = 0;
    cursor = tree.walk();
    var _loop_5 = function () {
        var c = cursor;
        switch (c.nodeType) {
            case "assignment" /* g.SyntaxType.Assignment */: {
                var node_4 = c.currentNode;
                // If LHS is an identifier, type is same as RHS
                var _b = inferType(filename, node_4.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug), type_1 = _b[0], ndim = _b[1], dim = _b[2], ismatrix = _b[3], ispointer = _b[4], isstruct = _b[5], cf = _b[6], vt = _b[7];
                var _c = inferType(filename, node_4.leftNode, var_types, custom_functions, classes, file, alias_tbl, debug), l_type_1 = _c[0], l_ndim = _c[1], l_dim = _c[2], l_ismatrix = _c[3], l_ispointer = _c[4], l_isstruct = _c[5];
                if (customTypes_1.type_to_matrix_type.findIndex(function (x) { return x.type == l_type_1; }) != -1 && customTypes_1.type_to_matrix_type.findIndex(function (x) { return x.type == type_1; }) == -1) {
                    //if ((l_type != null && l_type != "unknown") && type == null) {
                    type_1 = l_type_1;
                    ndim = l_ndim;
                    dim = l_dim;
                    ismatrix = l_ismatrix;
                    ispointer = l_ispointer;
                    isstruct = l_isstruct;
                }
                custom_functions = cf;
                var_types = vt;
                var scope = findVarScope(filename, node_4, block_idxs, "main", debug);
                if (node_4.leftNode.type == "identifier" /* g.SyntaxType.Identifier */ || node_4.leftNode.type == "attribute" /* g.SyntaxType.Attribute */) {
                    var name_1 = node_4.leftNode.text;
                    var v1_1 = (0, helperFunctions_2.filterByScope)(var_types, name_1, node_4, 1);
                    if (v1_1.length > 0) {
                        count = count + 1;
                        v1_1 = v1_1[v1_1.length - 1];
                        if (scope[2] == v1_1.scope[2]) { // same block level
                            var_types = var_types.filter(function (e) { return JSON.stringify(e) !== JSON.stringify(v1_1); }); // replace if already in var_types
                            var re = new RegExp("\\b".concat(v1_1.name, "\\b"));
                            if (re.test(node_4.rightNode.text)) {
                                var next_node = node_4.parent.nextNamedSibling;
                                if (next_node == null) {
                                    next_node = node_4.parent.parent;
                                    if (next_node.type == "module" /* g.SyntaxType.Module */) {
                                        next_node = node_4.rightNode;
                                    }
                                }
                                v1_1.scope = [v1_1.scope[0], next_node.startIndex - 1, v1_1.scope[2]];
                                var_types.push(v1_1);
                                var v2 = {
                                    name: v1_1.name,
                                    type: type_1,
                                    ndim: ndim,
                                    dim: dim,
                                    ismatrix: ismatrix,
                                    isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                                    ispointer: ispointer,
                                    isstruct: isstruct,
                                    scope: [next_node.startIndex, scope[1], scope[2]],
                                    initialized: true
                                };
                                var_types.push(v2);
                            }
                            else {
                                v1_1.scope = [v1_1.scope[0], node_4.startIndex - 1, v1_1.scope[2]];
                                var_types.push(v1_1);
                                var v2 = {
                                    name: v1_1.name,
                                    type: type_1,
                                    ndim: ndim,
                                    dim: dim,
                                    ismatrix: ismatrix,
                                    isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                                    ispointer: ispointer,
                                    isstruct: isstruct,
                                    scope: [node_4.startIndex, scope[1], scope[2]],
                                    initialized: false //true
                                };
                                var_types.push(v2);
                            }
                        }
                    }
                    else {
                        v1_1 = {
                            name: node_4.leftNode.text,
                            type: type_1,
                            ndim: ndim,
                            dim: dim,
                            ismatrix: ismatrix,
                            isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                            ispointer: ispointer,
                            isstruct: isstruct,
                            initialized: false,
                            scope: scope
                        };
                        var_types.push(v1_1);
                    }
                    // If LHS is subscript, type is matrix
                }
                else if (node_4.leftNode.type == "call_or_subscript" /* g.SyntaxType.CallOrSubscript */ || node_4.leftNode.type == "cell_subscript" /* g.SyntaxType.CellSubscript */) {
                    //} else if (node.leftNode.type == g.SyntaxType.CallOrSubscript) {
                    var name_2 = node_4.leftNode.valueNode.text;
                    var v3_1 = (0, helperFunctions_2.filterByScope)(var_types, name_2, node_4, 0);
                    if (node_4.leftNode.type == "cell_subscript" /* g.SyntaxType.CellSubscript */) {
                        var_types = var_types.filter(function (x) { return x.name != node_4.leftNode.text; });
                        var_types.push({
                            name: node_4.leftNode.text,
                            type: type_1,
                            ndim: ndim,
                            dim: dim,
                            ismatrix: ismatrix,
                            isvector: false,
                            ispointer: false,
                            isstruct: false,
                            initialized: false,
                            scope: scope
                        });
                    }
                    if (v3_1 != null && v3_1 != undefined) {
                        var_types = var_types.filter(function (e) { return JSON.stringify(e) !== JSON.stringify(v3_1); }); // replace if already in var_types
                        //var_types = var_types.filter(x => x.name != `${name}_init`);
                        var obj2 = var_types.find(function (x) { return x.name === "".concat(name_2, "_init"); });
                        if (obj2 == null || obj2 == undefined) {
                            var_types.push({
                                name: "".concat(name_2, "_init"),
                                type: v3_1.type,
                                ndim: v3_1.ndim,
                                dim: v3_1.dim,
                                ismatrix: v3_1.ismatrix,
                                isvector: v3_1.isvector,
                                ispointer: v3_1.ispointer,
                                isstruct: false,
                                initialized: false,
                                scope: v3_1.scope
                            });
                        }
                        v3_1.type = type_1;
                        var obj = var_types.find(function (x) { return x.name === "".concat(name_2, "_childtype"); });
                        if (obj != null && obj != undefined) {
                            var child_ndim = obj.ndim;
                            var child_dim = obj.dim;
                            var child_ismatrix = obj.ismatrix;
                            if (dim.toString() != obj.dim.toString()) {
                                child_ndim = "unknown";
                                child_dim = "unknown";
                            }
                            if (ismatrix != obj.ismatrix) {
                                child_ismatrix = "unknown";
                            }
                            var_types = var_types.filter(function (e) { return e.name !== "".concat(name_2, "_childtype"); }); // replace if al
                            var_types.push({
                                name: "".concat(name_2, "_childtype"),
                                type: (0, customTypes_1.binaryOpType)(type_1, obj.type),
                                ndim: child_ndim,
                                dim: child_dim,
                                ismatrix: child_ismatrix,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: false,
                                scope: null
                            });
                        }
                        else {
                            var_types.push({
                                name: "".concat(name_2, "_childtype"),
                                type: type_1,
                                ndim: ndim,
                                dim: dim,
                                ismatrix: ismatrix,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: false,
                                scope: null
                            });
                        }
                    }
                    else {
                        v3_1 = {
                            name: name_2,
                            type: type_1,
                            ndim: 2,
                            dim: [1, 1],
                            ismatrix: true,
                            isvector: false,
                            ispointer: false,
                            isstruct: false,
                            initialized: false,
                            scope: scope
                        };
                    }
                    var_types.push(v3_1);
                }
                break;
            }
            case "for_statement" /* g.SyntaxType.ForStatement */: {
                var node = c.currentNode;
                // If LHS is an identifier, type is same as RHS
                var _d = inferType(filename, node.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug), type = _d[0], ndim = _d[1], dim = _d[2], ismatrix = _d[3], ispointer = _d[4], isstruct = _d[5], cf = _d[6], vt = _d[7];
                custom_functions = cf;
                var_types = vt;
                var scope = findVarScope(filename, node, block_idxs, "main", debug);
                if (node.leftNode.type == "identifier" /* g.SyntaxType.Identifier */) {
                    var name_3 = node.leftNode.text;
                    var v1_2 = (0, helperFunctions_2.filterByScope)(var_types, name_3, node, 1);
                    if (v1_2.length > 0) {
                        count = count + 1;
                        v1_2 = v1_2[v1_2.length - 1];
                        if (scope[2] == v1_2.scope[2]) { // same block level
                            var_types = var_types.filter(function (e) { return JSON.stringify(e) !== JSON.stringify(v1_2); }); // replace if already in var_types
                            v1_2.scope = [v1_2.scope[0], node.startIndex - 1, v1_2.scope[2]];
                            var_types.push(v1_2);
                            var v2 = {
                                name: v1_2.name,
                                type: type,
                                ndim: ndim,
                                dim: dim,
                                ismatrix: ismatrix,
                                isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                                ispointer: ispointer,
                                isstruct: isstruct,
                                scope: [node.startIndex, scope[1], scope[2]],
                                initialized: false //true
                            };
                            var_types.push(v2);
                        }
                        else {
                            v1_2 = {
                                name: name_3,
                                type: type,
                                ndim: 1,
                                dim: [1],
                                ismatrix: false,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: false,
                                scope: scope
                            };
                            var_types.push(v1_2);
                        }
                    }
                    else {
                        v1_2 = {
                            name: name_3,
                            type: type,
                            ndim: 1,
                            dim: [1],
                            ismatrix: false,
                            isvector: false,
                            ispointer: false,
                            isstruct: false,
                            initialized: false,
                            scope: scope
                        };
                        var_types.push(v1_2);
                    }
                }
                break;
            }
            case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */: { // This helps update the argument types of function definitions
                var node = c.currentNode;
                var _e = inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug), cf = _e[6], vt = _e[7];
                custom_functions = cf;
                var_types = vt;
                break;
            }
        }
    };
    do {
        _loop_5();
    } while ((0, treeTraversal_1.gotoPreorderSucc_SkipFunctionDef)(cursor, debug));
    return [var_types, custom_functions, block_idxs];
}
exports.inferTypeFromAssignment = inferTypeFromAssignment;
function getFunctionReturnType(filename, fun_name, arg_types, var_types, fun_dictionary, custom_functions, classes, file, alias_tbl, debug, isclass) {
    // Update custom_functions with info on function return type
    if (debug == 1) {
        console.log("getFunctionReturnType");
    }
    //console.log("GETFUNRETURNTYPE");
    //console.log(var_types);
    //console.log(isclass);
    //console.log(fun_name);
    //console.log(file);
    var obj = fun_dictionary.find(function (x) { return x.name === fun_name; });
    //console.log(obj.file);
    if (obj != null) {
        var tree2 = parser.parse(obj.def_node.bodyNode.text);
        for (var i = 0; i < arg_types.length; i++) {
            arg_types[i].scope = [0, obj.def_node.endIndex - obj.def_node.startIndex, -1]; // "transpose" since passing adjusted tree
            arg_types[i].initialized = true;
        }
        // for some reason calling inferTypeFromAssignment modifies the value of arg_types
        var tmp_arg_types_1 = JSON.parse(JSON.stringify(arg_types));
        var tmp_var_arg_types = JSON.parse(JSON.stringify(obj.var_arg_types));
        var typestr_1 = '';
        for (var i = 0; i < tmp_arg_types_1.length; i++) {
            typestr_1 = typestr_1.concat(tmp_arg_types_1[i].type);
        }
        var block_idxs = [[0, obj.def_node.endIndex - obj.def_node.startIndex, 0]];
        //let [var_types2, c, var_types] = inferTypeFromAssignment(filename, tree2, arg_types, custom_functions, classes, file, alias_tbl, debug, block_idxs);
        var _a = inferTypeFromAssignment("main", tree2, arg_types.concat(var_types), custom_functions, classes, obj.file, alias_tbl, debug, block_idxs), var_types2 = _a[0], c_1 = _a[1], block_idxs2 = _a[2];
        //let [var_types2, c, var_types] = inferTypeFromAssignment(filename, tree2, arg_types.concat(var_types), custom_functions, classes, file, alias_tbl, debug, block_idxs);
        for (var i = 0; i < var_types2.length; i++) {
            if (var_types2[i].scope != null) {
                if (var_types2[i].scope[0] == 0) {
                    var_types2[i].scope[0] += obj.def_node.startIndex;
                }
                else {
                    var_types2[i].scope[0] += obj.def_node.bodyNode.startIndex;
                }
                if (var_types2[i].scope[1] == obj.def_node.endIndex - obj.def_node.startIndex) {
                    var_types2[i].scope[1] += obj.def_node.startIndex;
                }
                else {
                    var_types2[i].scope[1] += obj.def_node.bodyNode.startIndex;
                }
                if (var_types2[i].scope[2] == -1) {
                    var_types2[i].scope[2] = 0;
                }
            }
        }
        var _loop_6 = function (i) {
            tmp_arg_types_1[i] = var_types2.find(function (x) { return x.name == tmp_arg_types_1[i].name; });
        };
        for (var i = 0; i < tmp_arg_types_1.length; i++) {
            _loop_6(i);
        }
        if (!isclass) {
            fun_dictionary = c_1; // may need to change for classes
            //obj = fun_dictionary.find(x => x.name === fun_name);
        }
        custom_functions = c_1;
        var obj2 = obj.arg_type_dic.find(function (x) { return x.arg_type_id == typestr_1; });
        //let obj3 = obj.arg_type_dic.filter(x=>JSON.stringify(x.var_types) == JSON.stringify(var_types2));
        var return_node_1 = obj.def_node.return_variableNode;
        if (return_node_1 != undefined) {
            return_node_1 = return_node_1.firstChild;
            // If multiple return values, use pointers
            if (return_node_1.type == "matrix" /* g.SyntaxType.Matrix */) {
                var all_types_1 = [];
                var ptr_arg_types_1 = [];
                for (var i = 0; i < return_node_1.namedChildCount; i++) {
                    var return_var = return_node_1.namedChildren[i];
                    var _b = inferType(filename, return_var, var_types2, custom_functions, classes, file, alias_tbl, debug), return_type = _b[0], return_ndim = _b[1], return_dim = _b[2], return_ismatrix = _b[3], return_ispointer = _b[4], return_isstruct = _b[5], c_2 = _b[6], vt = _b[7];
                    custom_functions = c_2;
                    var_types2 = vt;
                    /*if (obj.ptr_arg_types != null) {
                        return_type = binaryOpType(return_type, obj.ptr_arg_types[i].type);
                    }*/
                    ptr_arg_types_1.push({
                        type: return_type,
                        ndim: return_ndim,
                        dim: return_dim,
                        ismatrix: return_ismatrix,
                        isvector: (0, helperFunctions_1.numel)(return_dim) > 1 && !return_ismatrix,
                        ispointer: true,
                        isstruct: return_isstruct
                    });
                }
                if (obj2 == null || obj2 == undefined) {
                    obj.arg_type_dic = obj.arg_type_dic.filter(function (x) { return x.arg_type_id != typestr_1; });
                    obj.arg_type_dic.push({
                        arg_type_id: typestr_1,
                        arg_types: tmp_arg_types_1,
                        ptr_arg_types: ptr_arg_types_1,
                        return_type: null,
                        var_types: var_types2
                    });
                }
                var v1 = {
                    name: obj.name,
                    arg_types: tmp_arg_types_1,
                    var_arg_types: tmp_var_arg_types,
                    return_type: null,
                    outs_transform: function (outs) { return null; },
                    external: obj.external,
                    file: obj.file,
                    def_node: obj.def_node,
                    ptr_arg_types: ptr_arg_types_1,
                    ptr_args: function (arg_types, outs) {
                        var ptr_args = [];
                        for (var i = 0; i < return_node_1.namedChildCount; i++) {
                            var return_var = return_node_1.namedChildren[i];
                            //let [return_type, return_ndim, return_dim, return_ismatrix, return_ispointer, return_isstruct, c, var_types] = inferType(filename, return_var, var_types2, custom_functions, classes, file, alias_tbl, debug);
                            //all_types.push(return_type);
                            all_types_1.push(ptr_arg_types_1[i].type);
                            custom_functions = c_1;
                            var return_name = return_var.text;
                            if (outs.length > i) {
                                return_name = outs[i];
                            }
                            ptr_args.push({
                                name: return_name,
                                type: ptr_arg_types_1[i].type,
                                ndim: ptr_arg_types_1[i].ndim,
                                dim: ptr_arg_types_1[i].dim,
                                ismatrix: ptr_arg_types_1[i].ismatrix,
                                isvector: ptr_arg_types_1[i].isvector,
                                ispointer: true,
                                isstruct: ptr_arg_types_1[i].isstruct
                            });
                        }
                        return ptr_args;
                    },
                    var_types: var_types2,
                    arg_type_dic: obj.arg_type_dic
                };
                if (!all_types_1.includes("unknown")) {
                    fun_dictionary = fun_dictionary.filter(function (e) { return e.name !== fun_name; });
                    fun_dictionary.push(v1);
                }
                return [v1.return_type, fun_dictionary, block_idxs2];
                // If single return value, don't use pointers 
            }
            else {
                var _c = inferType(filename, return_node_1, var_types2, custom_functions, classes, file, alias_tbl, debug), type = _c[0], ndim = _c[1], dim = _c[2], ismatrix = _c[3], ispointer = _c[4], isstruct = _c[5], c_3 = _c[6], vt = _c[7];
                custom_functions = c_3;
                var_types2 = vt;
                var return_type = {
                    type: type,
                    ndim: ndim,
                    dim: dim,
                    ismatrix: ismatrix,
                    isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                    ispointer: ispointer,
                    isstruct: isstruct
                };
                if (obj2 == null || obj2 == undefined) {
                    obj.arg_type_dic = obj.arg_type_dic.filter(function (x) { return x.arg_type_id != typestr_1; });
                    obj.arg_type_dic.push({
                        arg_type_id: typestr_1,
                        arg_types: tmp_arg_types_1,
                        ptr_arg_types: null,
                        return_type: return_type,
                        var_types: var_types2
                    });
                }
                //console.log("UNICORNS!!!!!");
                //console.log(obj.var_arg_types);
                //console.log("------------------------");
                var v1 = {
                    name: obj.name,
                    arg_types: tmp_arg_types_1,
                    var_arg_types: tmp_var_arg_types,
                    // arg_types.push(tmp_arg_types)
                    outs_transform: function (outs) { return outs; },
                    return_type: {
                        type: type,
                        ndim: ndim,
                        dim: dim,
                        ismatrix: ismatrix,
                        isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                        ispointer: ispointer,
                        isstruct: isstruct
                    },
                    ptr_args: function (arg_types, outs) { return null; },
                    ptr_arg_types: null,
                    external: obj.external,
                    file: obj.file,
                    def_node: obj.def_node,
                    var_types: var_types2,
                    arg_type_dic: obj.arg_type_dic
                };
                if (type !== "unknown") {
                    fun_dictionary = fun_dictionary.filter(function (e) { return e.name !== fun_name; });
                    fun_dictionary.push(v1);
                }
                return [v1.return_type, fun_dictionary, block_idxs2];
            }
        }
        else {
            if (obj2 == null || obj2 == undefined) {
                obj.arg_type_dic = obj.arg_type_dic.filter(function (x) { return x.arg_type_id != typestr_1; });
                obj.arg_type_dic.push({
                    arg_type_id: typestr_1,
                    arg_types: tmp_arg_types_1,
                    ptr_arg_types: null,
                    return_type: null,
                    var_types: var_types2
                });
            }
            var v1 = {
                name: obj.name,
                arg_types: tmp_arg_types_1,
                var_arg_types: tmp_var_arg_types,
                outs_transform: function (outs) { return outs; },
                return_type: null,
                //ptr_param: null, 
                //ptr_declaration: null,
                ptr_args: function (arg_types, outs) { return null; },
                ptr_arg_types: null,
                external: obj.external,
                file: obj.file,
                def_node: obj.def_node,
                var_types: var_types2,
                arg_type_dic: obj.arg_type_dic
            };
            if (arg_types[0].type !== "unknown") {
                fun_dictionary = fun_dictionary.filter(function (e) { return e.name !== fun_name; });
                fun_dictionary.push(v1);
            }
            return [v1.return_type, fun_dictionary, block_idxs2];
        }
    }
}
exports.getFunctionReturnType = getFunctionReturnType;
function inferTypeByName(name, node, var_types, custom_functions, alias_tbl, debug) {
    if (debug == 1) {
        console.log("inferTypeByName");
    }
    var obj1 = (0, helperFunctions_2.filterByScope)(var_types, name, node, 0);
    if (obj1 != null && obj1 != undefined) {
        return [obj1.type, obj1.ndim, obj1.dim, obj1.ismatrix, obj1.isvector, obj1.ispointer, obj1.isstruct, custom_functions];
    }
    var obj2 = (0, helperFunctions_2.filterByScope)(alias_tbl, name, node, 0);
    if (obj2 != null && obj2 != undefined) {
        var obj3 = var_types.find(function (x) { return x.name === obj2.tmp_var; });
        if (obj3 != null && obj3 != undefined) {
            return [obj3.type, obj3.ndim, obj3.dim, obj3.ismatrix, obj3.isvector, obj3.ispointer, obj3.isstruct, custom_functions];
        }
    }
    if (name == "INT_MAX" || name == "INT_MIN") {
        return ['int', 1, [1], false, false, false, false, custom_functions];
    }
    return [null, null, null, null, null, null, null, null];
}
exports.inferTypeByName = inferTypeByName;
function inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug) {
    /*console.log("INFERTYPE");
    if (node == undefined) {
        console.log("UNDEFINED");
    } else {
        console.log(node.text);
        console.log(node.type);
    }
    console.log("-----------------------");*/
    var _a, _b;
    if (debug == 1) {
        console.log("inferType");
    }
    // var unknown_type = ['unknown', null, null, null, null, null, custom_functions];
    // var unknown_type = ['unknown', 2, [1, 1], false, false, false, custom_functions];
    var unknown_type = ["".concat(node.text, "_type"), 2, ["".concat(node.text, "_dim0"), "".concat(node.text, "_dim1")], null, null, null, custom_functions, var_types];
    //let obj1 = alias_tbl.find(x => x.name === node.text);
    var obj1 = (0, helperFunctions_2.filterByScope)(alias_tbl, node.text, node, 0);
    if (obj1 != null && obj1 != undefined) {
        var obj2 = var_types.find(function (x) { return x.name === obj1.tmp_var; });
        if (obj2 != null && obj2 != undefined) {
            return [obj2.type, obj2.ndim, obj2.dim, obj2.ismatrix, obj2.ispointer, obj2.isstruct, custom_functions, var_types];
        }
    }
    switch (node.type) {
        case "comment" /* g.SyntaxType.Comment */: {
            if (fun_obj != null) {
                if (fun_obj.var_arg_types == null) {
                    if (node.text.includes("Varargin")) {
                        varargin_flag = true;
                    }
                    if (varargin_flag) {
                        var match_1 = node.text.match(/(\w*)\: ([a-zA-Z]*)( matrix)?/);
                        if (match_1 != null) {
                            if (varargin.findIndex(function (x) { return x.name == match_1[1]; }) == -1) {
                                if (match_1[3] != undefined) {
                                    varargin.push({
                                        name: match_1[1],
                                        type: match_1[2],
                                        ndim: "".concat(match_1[1], "_ndim"),
                                        dim: ["".concat(match_1[1], "_dim0"), "".concat(match_1[1], "_dim1")],
                                        ismatrix: true,
                                        isvector: false,
                                        ispointer: false,
                                        isstruct: false,
                                        initialized: true,
                                        scope: [0, fun_obj.def_node.endIndex - fun_obj.def_node.startIndex, 0]
                                    });
                                }
                                else {
                                    varargin.push({
                                        name: match_1[1],
                                        type: match_1[2],
                                        ndim: 1,
                                        dim: [1],
                                        ismatrix: false,
                                        isvector: false,
                                        ispointer: false,
                                        isstruct: false,
                                        initialized: true,
                                        scope: [0, fun_obj.def_node.endIndex - fun_obj.def_node.startIndex, 0]
                                    });
                                }
                            }
                        }
                    }
                    if (node.text.includes("Outputs")) {
                        varargin_flag = false;
                        fun_obj.var_arg_types = varargin;
                        custom_functions.filter(function (x) { return x.name != fun_obj.name; });
                        custom_functions.push(fun_obj);
                    }
                }
            }
            return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
            break;
        }
        case "parenthesized_expression" /* g.SyntaxType.ParenthesizedExpression */: {
            return inferType(filename, node.firstNamedChild, var_types, custom_functions, classes, file, alias_tbl, debug);
            break;
        }
        // Named types
        case "ellipsis" /* g.SyntaxType.Ellipsis */: {
            //return ['ellipsis', 2, [1, 1], false, false, false, custom_functions];
            return ['ellipsis', 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case ("true" /* g.SyntaxType.True */ || "false" /* g.SyntaxType.False */): {
            //return ['bool',  2, [1, 1], false, false, false, custom_functions];
            return ['bool', 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case "float" /* g.SyntaxType.Float */: {
            //return ['float',  2, [1, 1], false, false, false, custom_functions];
            return ['double', 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case "integer" /* g.SyntaxType.Integer */: {
            //return ['int',  2, [1, 1], false, false, false, custom_functions];
            return ['int', 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case "complex" /* g.SyntaxType.Complex */: {
            //return ['complex',  2, [1, 1], false, false, false, custom_functions];
            return ['complex', 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case "string" /* g.SyntaxType.String */: {
            //return ['char',  2, [1, 1], false, true, false, custom_functions];
            return ['char', 2, [1, node.text.length], false, true, false, custom_functions, var_types];
            //return ['char',  1, [node.text.length], false, true, false, custom_functions];
            break;
        }
        case "cell" /* g.SyntaxType.Cell */:
        case "matrix" /* g.SyntaxType.Matrix */: {
            var row = 0;
            var col = 0;
            //let nrows = 0;
            //let ncols = 0;
            var nrows = '0';
            var ncols = '0';
            for (var i = 0; i < node.childCount; i++) {
                if (node.children[i].type === ";") {
                    row += 1;
                    col = 0;
                }
                else if (node.children[i].isNamed) {
                    if (row == 0) {
                        var _c = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug), type_2 = _c[0], ndim = _c[1], dim = _c[2], c_4 = _c[6], vt = _c[7];
                        custom_functions = c_4;
                        var_types = vt;
                        if (dim == null) {
                            console.error("ERROR IN CELL/MATRIX: CHILD DIM IS NULL");
                        }
                        else {
                            if (ndim > 1) {
                                //ncols += dim[1];
                                ncols += "+".concat(dim[1]);
                            }
                            else {
                                //ncols += dim[0];
                                ncols += "+".concat(dim[0]);
                            }
                        }
                    }
                    if (col == 0) {
                        var _d = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug), type_3 = _d[0], ndim = _d[1], dim = _d[2], c_5 = _d[6], vt = _d[7];
                        custom_functions = c_5;
                        var_types = vt;
                        //nrows += dim[0];
                        if (dim == null) {
                            console.error("ERROR IN CELL/MATRIX: CHILD DIM IS NULL");
                        }
                        else {
                            nrows += "+".concat(dim[0]);
                        }
                    }
                    col += 1;
                }
            }
            if (/^[^a-zA-Z]+$/.test(nrows)) {
                nrows = eval(nrows);
            }
            if (/^[^a-zA-Z]+$/.test(ncols)) {
                ncols = eval(ncols);
            }
            var children_types_1 = [];
            var children_ndim_1 = [];
            var children_dim_1 = [];
            var children_ismatrix_1 = [];
            for (var _i = 0, _e = node.namedChildren; _i < _e.length; _i++) {
                var child = _e[_i];
                var _f = inferType(filename, child, var_types, custom_functions, classes, file, alias_tbl, debug), child_type_1 = _f[0], child_ndim_1 = _f[1], child_dim_1 = _f[2], child_ismatrix_1 = _f[3], c_6 = _f[6], vt = _f[7];
                custom_functions = c_6;
                var_types = vt;
                children_types_1.push(child_type_1);
                children_ndim_1.push(child_ndim_1);
                children_dim_1.push(child_dim_1);
                children_ismatrix_1.push(child_ismatrix_1);
            }
            var type_4 = 'unknown';
            var child_ndim = 1;
            var child_dim_2 = [1];
            var child_ismatrix_2 = false;
            if (children_types_1.every(function (val) { return val === children_types_1[0]; })) {
                type_4 = children_types_1[0];
            }
            else if (children_types_1.every(function (val) { return ['int', 'double', 'complex'].includes(val); })) {
                if (children_types_1.includes('complex')) {
                    type_4 = 'complex';
                }
                else if (children_types_1.includes('double')) {
                    type_4 = 'double';
                }
                else if (children_types_1.includes('int')) {
                    type_4 = 'int';
                }
            }
            else {
                type_4 = 'heterogeneous';
            }
            if (children_dim_1.every(function (x) { return x != null; })) {
                if (children_ndim_1.every(function (x) { return x === children_ndim_1[0]; })) {
                    child_ndim = children_ndim_1[0];
                }
                if (children_dim_1.every(function (x) { return x.toString() === children_dim_1[0].toString(); })) {
                    child_dim_2 = children_dim_1[0];
                }
                if (children_ismatrix_1.every(function (x) { return x === children_ismatrix_1[0]; })) {
                    child_ismatrix_2 = children_ismatrix_1[0];
                }
            }
            if (node.type == "cell" /* g.SyntaxType.Cell */ && node.parent.type == "assignment" /* g.SyntaxType.Assignment */) {
                var_types.push({
                    name: "".concat(node.parent.leftNode.text, "_childtype"),
                    type: type_4,
                    ndim: child_ndim,
                    dim: child_dim_2,
                    ismatrix: child_ismatrix_2,
                    isvector: false,
                    ispointer: false,
                    isstruct: false,
                    initialized: false,
                    scope: null
                });
            }
            //return [type, 2, [nrows, ncols], true, true, false, custom_functions];
            return [type_4, 2, [nrows, ncols], true, false, false, custom_functions, var_types];
            break;
        }
        case "comparison_operator" /* g.SyntaxType.ComparisonOperator */:
        case "boolean_operator" /* g.SyntaxType.BooleanOperator */:
        case "transpose_operator" /* g.SyntaxType.TransposeOperator */:
        case "unary_operator" /* g.SyntaxType.UnaryOperator */:
        case "binary_operator" /* g.SyntaxType.BinaryOperator */: {
            var _g = parseFunctionCallNode(node), args = _g[0], arg_types = _g[1], outs = _g[2];
            var obj = (0, helperFunctions_2.findBuiltin)(builtinFunctions_1.operatorMapping, node.operatorNode.type);
            var return_type = obj.return_type(args, arg_types, outs);
            var arg_types_transform = obj.arg_types_transform(args, arg_types, outs);
            if (arg_types_transform != null) {
                return_type = obj.return_type(args, arg_types_transform, outs);
                var _loop_7 = function (i) {
                    var obj2 = (0, helperFunctions_2.filterByScope)(var_types, arg_types_transform[i].name, node, 0);
                    if (obj2 != null) {
                        arg_types_transform[i].scope = obj2.scope;
                        var_types = var_types.filter(function (x) { return JSON.stringify(x) !== JSON.stringify(obj2); });
                    }
                    else {
                        //arg_types_transform[i].scope = findVarScope(filename, node, block_idxs, "main", debug);
                    }
                    var_types.push(arg_types_transform[i]);
                };
                for (var i = 0; i < arg_types_transform.length; i++) {
                    _loop_7(i);
                }
                //console.log("ARG TYPE TRANSFORM");
                //console.log(obj.arg_types_transform(args, arg_types, outs));
                //console.log("RETURN TYPE");
                //console.log(obj.return_type(args, arg_types, outs));
            }
            return [
                return_type.type,
                return_type.ndim,
                return_type.dim,
                return_type.ismatrix,
                return_type.ispointer,
                return_type.isstruct,
                custom_functions,
                var_types
            ];
            break;
        }
        // Attribute
        case "attribute" /* g.SyntaxType.Attribute */: {
            // First check if class method
            var _h = inferType(filename, node.objectNode, var_types, custom_functions, classes, file, alias_tbl, debug), type_5 = _h[0], c_7 = _h[6], vt = _h[7];
            custom_functions = c_7;
            var_types = vt;
            var obj = classes.find(function (x) { return x.name === type_5; });
            if (obj !== null && obj !== undefined) {
                var obj2 = obj.methods.find(function (x) { return x.name === node.attributeNode.text; });
                if (obj2 != null && obj2 != undefined) {
                    if (obj2.return_type == null) {
                        return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                    }
                    else {
                        return [
                            obj2.return_type.type,
                            obj2.return_type.ndim,
                            obj2.return_type.dim,
                            obj2.return_type.ismatrix,
                            obj2.return_type.ispointer,
                            obj2.return_type.isstruct,
                            custom_functions,
                            var_types
                        ];
                    }
                }
                var obj3 = obj.properties.find(function (x) { return x.name === node.attributeNode.text; });
                if (obj3 != null && obj3 != undefined) {
                    return [
                        obj3.type,
                        obj3.ndim,
                        obj3.dim,
                        obj3.ismatrix,
                        obj3.ispointer,
                        obj3.isstruct,
                        custom_functions,
                        var_types
                    ];
                }
                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
            }
            // If not class method, treat like an identifier (field of a struct)
            //let obj = var_types.find(x => x.name === node.text);
            obj = (0, helperFunctions_2.filterByScope)(var_types, node.text, node, 0);
            if (obj != null) {
                return [obj.type, obj.ndim, obj.dim, obj.ismatrix, obj.ispointer, obj.isstruct, custom_functions, var_types];
            }
            else {
                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
            }
            break;
        }
        // Identifier
        case "identifier" /* g.SyntaxType.Identifier */: {
            if (node.text == "INT_MAX" || node.text == "INT_MIN") {
                return ['int', 1, [1], false, false, false, custom_functions, var_types];
            }
            var obj = (0, helperFunctions_2.filterByScope)(var_types, node.text, node, 0);
            //let obj = var_types.find(x => (x.name == node.text) && (node.startIndex >= x.scope[0]) && (node.endIndex <= x.scope[1]));
            //let obj = var_types.filter(function(e) { return e.name == node.text });
            if (obj != null) {
                return [obj.type, obj.ndim, obj.dim, obj.ismatrix, obj.ispointer, obj.isstruct, custom_functions, var_types];
            }
            else {
                //return ['unknown', 2, [1, 1], false, false, false, custom_functions];
                return unknown_type;
            }
            break;
        }
        // TO DO: FIX THIS
        case "cell_subscript" /* g.SyntaxType.CellSubscript */: {
            var _j = inferType(filename, node.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug), parent_type = _j[0], parent_ndim = _j[1], parent_dim = _j[2], parent_ismatrix = _j[3], parent_isstruct = _j[5], cf = _j[6], vt = _j[7];
            custom_functions = cf;
            var_types = vt;
            var dim = [];
            var dim_idx = 0;
            for (var i = 2; i < node.childCount; i++) {
                //for (let i=1; i<node.namedChildCount; i++) {
                if (node.children[i].isNamed) {
                    dim_idx += 1;
                    //var [child_type,,child_dim,child_ismatrix,,, c, var_types] = inferType(filename, node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                    var _k = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug), child_type = _k[0], child_dim = _k[2], child_ismatrix = _k[3], c = _k[6], vt2 = _k[7];
                    /*console.log("NODE CHILD");
                    console.log(node.text);
                    console.log(node.children[i].text);
                    console.log(child_type);*/
                    custom_functions = c;
                    var_types = vt2;
                    //dim.push(child_dim[1]);
                    if (child_dim == null) {
                        console.error("ERROR IN CELLSUB: CHILD DIM IS NULL");
                    }
                    else {
                        if (child_dim.length > 1) {
                            dim.push(child_dim[1]);
                        }
                        else {
                            dim.push(child_dim[0]);
                        }
                    }
                }
                else if (node.children[i].type == ":") {
                    if (node.namedChildCount == 1) {
                        dim.push((0, helperFunctions_1.numel)(parent_dim));
                    }
                    else {
                        dim.push(parent_dim[dim_idx]);
                        dim_idx += 1;
                    }
                }
            }
            if (dim.length == 1 && dim[0] == 1) {
                dim = [1, 1];
            }
            var ismatrix = !dim.every(function (val) { return val === 1; });
            var obj = var_types.find(function (x) { return x.name === "".concat(node.valueNode.text, "_childtype"); });
            if (obj != null && obj != undefined) {
                if (obj.dim != "unknown") {
                    dim = obj.dim;
                }
                if (obj.ismatrix != "unknown") {
                    ismatrix = obj.ismatrix;
                }
            }
            //return [child_type, dim.length, dim, ismatrix, false, false, custom_functions, var_types];
            return ["unknown", dim.length, dim, ismatrix, false, false, custom_functions, var_types];
            break;
        }
        case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */: {
            var _l = parseFunctionCallNode(node), args = _l[0], arg_types_1 = _l[1], outs = _l[2];
            var obj = classes.find(function (x) { return x.name === node.valueNode.text; });
            // Is a class (treat as structure)
            if (obj != null) {
                return [obj.name, 2, [1, 1], false, false, true, custom_functions, var_types];
            }
            else if (args.length > 0) {
                // Is a function call
                var filename2 = file.match(/((?<=\/)([^\/]*?)(?=\.m))|(^([^\/]*?)(?=\.m))/);
                // Avoid recursive function calls
                if (filename2[0] != node.valueNode.text) {
                    //let obj1 = classes.find(x => x.name === arg_types[0].type);
                    var obj1_1 = classes.find(function (x) { return arg_types_1.some(function (y) { return y.type == x.name; }); });
                    var obj2 = custom_functions.find(function (x) { return x.name === node.valueNode.text; });
                    var obj3 = (0, helperFunctions_2.findBuiltin)(builtinFunctions_1.builtin_functions, node.valueNode.text);
                    if (obj1_1 != null && obj1_1 != undefined) {
                        var obj_3 = obj1_1.methods.find(function (x) { return x.name === node.valueNode.text; });
                        if (obj_3 != null && obj_3 != undefined) {
                            for (var i = 0; i < arg_types_1.length; i++) {
                                obj_3.arg_types[i].type = arg_types_1[i].type;
                                obj_3.arg_types[i].ndim = arg_types_1[i].ndim;
                                obj_3.arg_types[i].dim = arg_types_1[i].dim;
                                obj_3.arg_types[i].ismatrix = arg_types_1[i].ismatrix;
                                obj_3.arg_types[i].isstruct = arg_types_1[i].isstruct;
                                obj_3.arg_types[i].ispointer = arg_types_1[i].ispointer;
                            }
                            var return_type = null;
                            //[return_type, obj1.methods] = getFunctionReturnType(filename, node.valueNode.text, obj.arg_types, obj1.methods, custom_functions, classes, file, alias_tbl, debug); 
                            //[return_type, obj1.methods] = getFunctionReturnType(filename, node.valueNode.text, obj.arg_types, var_types, obj1.methods, custom_functions, classes, file, alias_tbl, debug, 1); 
                            _a = getFunctionReturnType(filename, node.valueNode.text, obj_3.arg_types, [], obj1_1.methods, custom_functions, classes, file, alias_tbl, debug, 1), return_type = _a[0], obj1_1.methods = _a[1];
                            if (return_type == null) {
                                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                            }
                            else {
                                return [
                                    return_type.type,
                                    return_type.ndim,
                                    return_type.dim,
                                    return_type.ismatrix,
                                    return_type.ispointer,
                                    return_type.isstruct,
                                    custom_functions,
                                    var_types
                                ];
                            }
                        }
                    }
                    if (obj2 != null && obj2 != undefined) {
                        // make sure class method receives arg of class type
                        var class_name_3 = obj2.file.match(/(?<=@)(.*?)(?=\/)/);
                        var flag = true;
                        if (class_name_3 != null) {
                            //if (arg_types[0].type != class_name) {
                            if (arg_types_1.every(function (x) { return x.type != class_name_3; })) {
                                flag = false;
                            }
                        }
                        if (flag == true) {
                            //TO DO: figure out way to deal with varargin
                            for (var i = 0; i < obj2.arg_types.length; i++) {
                                //for (let i=0; i < arg_types.length; i++) {
                                if (arg_types_1[i].type !== "unknown" && arg_types_1[i].type !== null) {
                                    obj2.arg_types[i].type = arg_types_1[i].type;
                                    obj2.arg_types[i].ndim = arg_types_1[i].ndim;
                                    obj2.arg_types[i].dim = arg_types_1[i].dim;
                                    obj2.arg_types[i].ismatrix = arg_types_1[i].ismatrix;
                                    obj2.arg_types[i].isstruct = arg_types_1[i].isstruct;
                                    obj2.arg_types[i].ispointer = arg_types_1[i].ispointer;
                                }
                            }
                            var var_types3 = [];
                            if (obj2.var_arg_types != null) {
                                var_types3 = obj2.var_arg_types;
                            }
                            var return_type = null;
                            //[return_type, custom_functions] = getFunctionReturnType(filename, node.valueNode.text, obj2.arg_types, var_types, custom_functions, custom_functions, classes, file, alias_tbl, debug, 0);
                            _b = getFunctionReturnType(filename, node.valueNode.text, obj2.arg_types, var_types3, custom_functions, custom_functions, classes, file, alias_tbl, debug, 0), return_type = _b[0], custom_functions = _b[1];
                            if (return_type == null) {
                                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                            }
                            else {
                                return [
                                    return_type.type,
                                    return_type.ndim,
                                    return_type.dim,
                                    return_type.ismatrix,
                                    return_type.ispointer,
                                    return_type.isstruct,
                                    custom_functions,
                                    var_types
                                ];
                            }
                        }
                    }
                    if (obj3 != null && obj3 != undefined) {
                        var return_type = obj3.return_type(args, arg_types_1, outs);
                        if (return_type == null) {
                            return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                        }
                        else {
                            return [
                                return_type.type,
                                return_type.ndim,
                                return_type.dim,
                                return_type.ismatrix,
                                return_type.ispointer,
                                return_type.isstruct,
                                custom_functions,
                                var_types
                            ];
                        }
                    }
                }
                else {
                    return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                }
            }
            var _m = inferType(filename, node.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug), parent_type = _m[0], parent_ismatrix = _m[3], parent_isstruct = _m[5], c_8 = _m[6], vt = _m[7];
            custom_functions = c_8;
            var_types = vt;
            // Is a subscript
            if (parent_ismatrix || parent_isstruct) {
                var dim = [];
                for (var i = 1; i < node.namedChildCount; i++) {
                    var _o = inferType(filename, node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug), child_dim_3 = _o[2], c_9 = _o[6], vt_1 = _o[7];
                    custom_functions = c_9;
                    var_types = vt_1;
                    if (child_dim_3 == null) {
                        console.error("ERROR IN CALLORSUB: CHILD DIM IS NULL");
                    }
                    else {
                        if (child_dim_3.length > 1) {
                            dim.push(child_dim_3[1]);
                        }
                        else {
                            dim.push(child_dim_3[0]);
                        }
                    }
                }
                if (dim.length == 1 && dim[0] == 1) {
                    dim = [1, 1];
                }
                if (dim.every(function (val) { return val === 1; })) {
                    return [parent_type, dim.length, dim, false, false, parent_isstruct, custom_functions, var_types];
                }
                else {
                    //return [parent_type, dim.length, dim, true, true, parent_isstruct, custom_functions];
                    return [parent_type, dim.length, dim, true, false, parent_isstruct, custom_functions, var_types];
                }
            }
            return ["unknown", 1, [1], false, false, false, custom_functions, var_types];
            break;
        }
        case "slice" /* g.SyntaxType.Slice */: {
            var children_types = [];
            var children_vals = [];
            for (var i = 0; i < node.namedChildCount; i++) {
                var child = node.namedChildren[i];
                var _p = inferType(filename, child, var_types, custom_functions, classes, file, alias_tbl, debug), child_type_2 = _p[0], c_10 = _p[6], vt = _p[7];
                custom_functions = c_10;
                var_types = vt;
                if (child_type_2 == "keyword") {
                    var _q = inferType(filename, node.parent.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug), ndim = _q[1], dim = _q[2], c_11 = _q[6], vt_2 = _q[7];
                    if (dim != null) {
                        custom_functions = c_11;
                        var_types = vt_2;
                        var firstNode = node.parent.namedChildren[1];
                        var current_dim = 0;
                        var dummyNode = node;
                        while (JSON.stringify(dummyNode) != JSON.stringify(firstNode)) {
                            dummyNode = dummyNode.previousNamedSibling;
                            current_dim += 1;
                        }
                        children_vals.push(dim[current_dim]);
                    }
                    else {
                        console.error("ERROR IN SLICE: PARENT DIM IS NULL");
                        children_vals.push(1);
                    }
                    children_types.push('int');
                }
                else {
                    children_vals.push(Number(child.text));
                    children_types.push(child_type_2);
                }
            }
            var type = 'unknown';
            if (children_types.every(function (val) { return ['int', 'double', 'complex'].includes(val); })) {
                if (children_types.includes('complex')) {
                    type = 'complex';
                }
                else if (children_types.includes('double')) {
                    type = 'double';
                }
                else if (children_types.includes('int')) {
                    type = 'int';
                }
            }
            var start = children_vals[0];
            var stop = children_vals[1];
            var step = 1;
            if (children_vals.length == 3) {
                stop = children_vals[2];
                step = children_vals[1];
            }
            var len = Math.floor((stop - start) / step) + 1;
            //let len = Math.round((stop-start)/step) + 1;
            // TO DO: Maybe change so that it's only pointer, not a matrix and
            // represented in generateCode by creating an array, not a matrix
            //return [type, 2, [1, len], true, true, false, custom_functions];
            return [type, 2, [1, len], false, false, false, custom_functions, var_types];
        }
        case "keyword" /* g.SyntaxType.Keyword */: {
            return ['keyword', 1, [1], false, false, false, custom_functions, var_types];
        }
        // Default
        default: return ['unknown', 1, [1], false, false, false, custom_functions, var_types];
    }
    // Return args, arg_types, outs from function
    function parseFunctionCallNode(node) {
        if (debug == 1) {
            console.log("parseFunctionCallNode");
        }
        if (node.parent.type == "assignment" /* g.SyntaxType.Assignment */) {
            return parseFunctionCallNode(node.parent);
        }
        else {
            switch (node.type) {
                case "assignment" /* g.SyntaxType.Assignment */: {
                    var left_node = node.leftNode;
                    var right_node = node.rightNode;
                    break;
                }
                default: {
                    var left_node = null;
                    var right_node = node;
                    break;
                }
            }
            var args = [];
            var arg_types = [];
            switch (right_node.type) {
                case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */: {
                    for (var i = 1; i < right_node.namedChildCount; i++) {
                        //if (transformNode(right_node.namedChildren[i]) != undefined) {
                        //    args.push(transformNode(right_node.namedChildren[i]));   
                        //} else {
                        args.push(right_node.namedChildren[i].text);
                        //}
                        var _a = inferType(filename, right_node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug), type_6 = _a[0], ndim = _a[1], dim = _a[2], ismatrix = _a[3], ispointer = _a[4], isstruct = _a[5], c_12 = _a[6], vt = _a[7];
                        custom_functions = c_12;
                        var_types = vt;
                        arg_types.push({
                            type: type_6,
                            ndim: ndim,
                            dim: dim,
                            ismatrix: ismatrix,
                            isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                            ispointer: ispointer,
                            isstruct: isstruct
                        });
                    }
                    break;
                }
                case "comparison_operator" /* g.SyntaxType.ComparisonOperator */:
                case "boolean_operator" /* g.SyntaxType.BooleanOperator */:
                case "binary_operator" /* g.SyntaxType.BinaryOperator */: {
                    var _b = inferType(filename, right_node.leftNode, var_types, custom_functions, classes, file, alias_tbl, debug), l_type = _b[0], l_ndim = _b[1], l_dim = _b[2], l_ismatrix = _b[3], l_ispointer = _b[4], l_isstruct = _b[5], c1 = _b[6], vt1 = _b[7];
                    custom_functions = c1;
                    var_types = vt1;
                    var _c = inferType(filename, right_node.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug), r_type = _c[0], r_ndim = _c[1], r_dim = _c[2], r_ismatrix = _c[3], r_ispointer = _c[4], r_isstruct = _c[5], c2 = _c[6], vt2_1 = _c[7];
                    custom_functions = c2;
                    var_types = vt2_1;
                    arg_types.push({
                        type: l_type,
                        ndim: l_ndim,
                        dim: l_dim,
                        ismatrix: l_ismatrix,
                        isvector: (0, helperFunctions_1.numel)(l_dim) > 1 && !l_ismatrix,
                        ispointer: l_ispointer,
                        isstruct: l_isstruct
                    });
                    arg_types.push({
                        type: r_type,
                        ndim: r_ndim,
                        dim: r_dim,
                        ismatrix: r_ismatrix,
                        isvector: (0, helperFunctions_1.numel)(r_dim) > 1 && !r_ismatrix,
                        ispointer: r_ispointer,
                        isstruct: r_isstruct
                    });
                    //if (transformNode(right_node.leftNode) != undefined) {
                    //    args.push(transformNode(right_node.leftNode));   
                    //} else {
                    args.push(right_node.leftNode.text);
                    //}
                    //if (transformNode(right_node.rightNode) != undefined) {
                    //    args.push(transformNode(right_node.rightNode));   
                    //} else {
                    args.push(right_node.rightNode.text);
                    //}
                    break;
                }
                case "unary_operator" /* g.SyntaxType.UnaryOperator */:
                case "transpose_operator" /* g.SyntaxType.TransposeOperator */: {
                    var _d = inferType(filename, right_node.argumentNode, var_types, custom_functions, classes, file, alias_tbl, debug), type_7 = _d[0], ndim = _d[1], dim = _d[2], ismatrix = _d[3], ispointer = _d[4], isstruct = _d[5], c_13 = _d[6], vt = _d[7];
                    custom_functions = c_13;
                    var_types = vt;
                    arg_types.push({
                        type: type_7,
                        ndim: ndim,
                        dim: dim,
                        ismatrix: ismatrix,
                        isvector: (0, helperFunctions_1.numel)(dim) > 1 && !ismatrix,
                        ispointer: ispointer,
                        isstruct: isstruct
                    });
                    //if (transformNode(right_node.argumentNode) != undefined) {
                    //    args.push(transformNode(right_node.argumentNode));   
                    //} else {
                    args.push(right_node.argumentNode.text);
                    //}
                    break;
                }
            }
            var outs = [];
            if (left_node == null) {
            }
            else if (left_node.type == "matrix" /* g.SyntaxType.Matrix */) {
                for (var _i = 0, _e = left_node.namedChildren; _i < _e.length; _i++) {
                    var child = _e[_i];
                    //if (transformNode(child) != undefined) {
                    //    outs.push(transformNode(child));   
                    //} else {
                    outs.push(child.text);
                    //}
                }
            }
            else {
                //if (transformNode(left_node) != undefined) {
                //    outs.push(transformNode(left_node));   
                //} else {
                outs.push(left_node.text);
                //}
            }
            return [args, arg_types, outs];
        }
    }
}
exports.inferType = inferType;
//# sourceMappingURL=typeInference.js.map