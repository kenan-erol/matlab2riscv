var fs = require('fs');
import * as g from "./generated";
import { 
    gotoPreorderSucc, 
    gotoPreorderSucc_SkipFunctionDef, 
    findEntryFunction
} from "./treeTraversal";
import { numel } from "./helperFunctions";
import { binaryOpType, CustomFunction, VarType, Type, Alias, type_to_matrix_type } from "./customTypes";
import { operatorMapping, builtin_functions } from "./builtinFunctions";
import { filterByScope, findBuiltin } from "./helperFunctions";
import Parser = require("tree-sitter");
//import Matlab = require("tree-sitter-matlab");
import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");

let parser = new Parser() as g.Parser;
parser.setLanguage(Matlab);

let dummy_count = 0;
let fun_obj = null;
let varargin = [];
let varargin_flag = false;

// Type inference
// -----------------------------------------------------------------------------

export function findVarScope(filename, node, block_idxs, current_code, debug) {
    if (debug == 1) {
        console.log("findVarScope");
    }

    let entire_scope = block_idxs.find(x => x[2] == 0);
    let good_blocks = [];
    if (current_code == filename) {
    //if (current_code == "main") {
        good_blocks = block_idxs.filter(function(e) { return e[2] >= 1 });
    } else {
        good_blocks = block_idxs.filter(function(e) { return e[2] == -1 });
    }

    let scope = good_blocks.filter(function(e) { return e[0] <= node.startIndex && e[1] >= node.endIndex } );
    scope = scope[scope.length - 1];
    if (scope != null && scope != undefined) {
        return scope;
    } 

    //if (current_code == filename) {
    if (current_code == "main") {
        let fundef_blocks = block_idxs.filter(function(e) { return e[2] == -1 });
        if (fundef_blocks.length != 0) {
            if (node.startIndex >= entire_scope[0] && node.endIndex < fundef_blocks[0][0]) {
                return [entire_scope[0], fundef_blocks[0][0], 0];
            }
        } 
        return [entire_scope[0], entire_scope[1], 0];
    }
}


function typeInference(filename, file, custom_functions, classes, debug) {
    if (debug == 1) {
        console.log("typeInference");
    }
    
    let class_method = false;
    var var_types: VarType[] = [];
    let block_idxs = [];
    let alias_tbl: Alias[] = [];
    const sourceCode = fs.readFileSync(file, "utf8");
    let tree = parser.parse(sourceCode);
    let entry_fun_node = findEntryFunction(tree, debug);
    if (entry_fun_node != null) {
        
        tree = parser.parse(entry_fun_node.bodyNode.text);
        
        let obj = custom_functions.find(x => x.name == entry_fun_node.nameNode.text);
        if (obj != null) {
            [, custom_functions, block_idxs] = getFunctionReturnType(filename, obj.name, obj.arg_types, [], custom_functions, custom_functions, classes, file, alias_tbl, debug, 0);
            obj = custom_functions.find(x => x.name == entry_fun_node.nameNode.text);
            /*custom_functions = custom_functions.filter(x => x.name != entry_fun_node.nameNode.text);
            for (let i = 0; i < obj.arg_types.length; i++) {
                obj.arg_types[i] = var_types.find(x=>x.name == obj.arg_types[i].name);
            }
            obj.var_types = var_types;
            custom_functions.push(obj);*/
            return [obj.var_types, custom_functions, block_idxs];
        }
        
        let class_name = file.match(/(?<=@)(.*?)(?=\/)/);
        if (class_name != null) {
            let class_obj = classes.find(x => x.name === class_name[0]);
            if (class_obj != null && class_obj != undefined) {
                obj = class_obj.methods.find(x => x.name == entry_fun_node.nameNode.text);
            }
            class_method = true;
        }
        var_types = var_types.concat(obj.arg_types);
        for (let i = 0; i < var_types.length; i++) {
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
    
    [var_types, custom_functions, block_idxs] = inferTypeFromAssignment(filename, tree, var_types, custom_functions, classes, file, alias_tbl, debug, []);

    if (entry_fun_node != null) {
        
        if (class_method) {
            let class_name = file.match(/(?<=@)(.*?)(?=\/)/);
            let class_obj = classes.find(x => x.name === class_name[0]);
            let obj = class_obj.methods.find(x => x.name == entry_fun_node.nameNode.text);
            class_obj.methods = class_obj.methods.filter(x => x.name != entry_fun_node.nameNode.text);
            for (let i = 0; i < obj.arg_types.length; i++) {
                obj.arg_types[i] = var_types.find(x=>x.name == obj.arg_types[i].name);
            }
            obj.var_types = var_types;
            class_obj.methods.push(obj);
        } else {
            let obj = custom_functions.find(x => x.name == entry_fun_node.nameNode.text);
            custom_functions = custom_functions.filter(x => x.name != entry_fun_node.nameNode.text);
            for (let i = 0; i < obj.arg_types.length; i++) {
                obj.arg_types[i] = var_types.find(x=>x.name == obj.arg_types[i].name);
            }
            obj.var_types = var_types;
            custom_functions.push(obj);
        }
        
    }
    return [var_types, custom_functions, block_idxs];
}
// to do: add tree_idx = [node.startIndex, node.endIndex]
// then after traversing tree make sure each variable's scope is updated to scope + tree_idx[0] 
function inferTypeFromAssignment(filename, tree, var_types, custom_functions, classes, file, alias_tbl, debug, block_idxs) {
    if (debug == 1) {
        console.log("inferTypeFromAssignment");
    }
    let text = tree.rootNode.text;
    //let block_idxs = [];
    let fun_flag = false;
    let fun_def_flag = false;
    let scaler = 0;
    if (block_idxs.length > 0) {
        // examining a function definition
        fun_flag = true;
        scaler = block_idxs[0][0];
    }
    let block_level = 0;
    let cursor = tree.walk();
    do {
        const c = cursor as g.TypedTreeCursor;
       
        switch (c.nodeType) {
            case g.SyntaxType.Module: {
                fun_def_flag = false;
                let node = c.currentNode;
                block_idxs.push([node.startIndex, node.endIndex, 0]); // 0 indicates entire program
                break;
            }
            case g.SyntaxType.Block: {
                let node = c.currentNode;
                if (block_idxs[block_idxs.length - 1][0] < node.startIndex && node.endIndex < block_idxs[block_idxs.length - 1][1]) {
                    block_level = block_level + 1;
                } else {
                    let prev_blocks = block_idxs.filter(function(e) { return e[1] < node.startIndex });
                    if (prev_blocks.length != 0) {
                        let prev_block = prev_blocks.reduce((max, block) => max[1] > block[1] ? max : block);
                        block_level = prev_block[2];
                    } 
                }
                if (fun_def_flag) {
                    block_level = -1;
                }
                block_idxs.push([node.startIndex, node.endIndex, block_level]); // 1 for regular blocks
                break;
            }
            case g.SyntaxType.FunctionDefinition: {
                fun_def_flag = true;
                let node = c.currentNode;
                fun_obj = custom_functions.find(x => x.name === node.nameNode.text);
                block_idxs.push([node.startIndex, node.endIndex, -1]); // 2 for function def blocks 
                break;
            }
            case g.SyntaxType.Comment: { // This helps update the varargin argument types of function definitions
                let node = c.currentNode;
                let [, , , , , , cf, vt] = inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = cf;
                var_types = vt;
                break;
            }
        }
    //} while(gotoPreorderSucc_SkipFunctionDef(cursor, debug));
    } while(gotoPreorderSucc(cursor, debug));
    
    cursor = tree.walk();
    do {
        const c = cursor as g.TypedTreeCursor;
        switch (c.nodeType) {
            case g.SyntaxType.Attribute: {
                let node = c.currentNode;
                let scope = findVarScope(filename, node, block_idxs, "main", debug);
                for (let i = 0; i < classes.length; i ++) {
                    let property = classes[i].properties.find(x => x.name === node.attributeNode.text);
                    if (property !== null && property !== undefined) {
                        let class_property_count = 0;
                        for (let j = 0; j < classes[i].properties.length; j ++) {
                            let object_node_text = node.objectNode.text;
                            object_node_text = object_node_text.replace("(", "\\(");
                            object_node_text = object_node_text.replace(")", "\\)");
                            let re = new RegExp(`\\b${object_node_text}\\.${classes[i].properties[j].name}\\b`);
                            if (re.test(text)) {
                                class_property_count += 1;
                            }
                        }
                        if (class_property_count >= 2) {
                            let v1 = var_types.find(x => x.name == node.objectNode.text);
                            if (v1 != null && v1 != undefined) {
                                scope = v1.scope;
                            }
                            var_types = var_types.filter(x => x.name != node.objectNode.text);
                            var_types = var_types.filter(x => x.name != node.text);
                            var_types.push({ 
                                name: node.objectNode.text, 
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
                                name: node.text, 
                                type: property.type, 
                                ndim: property.ndim, 
                                dim: property.dim, 
                                ismatrix: property.ismatrix,
                                isvector: numel(property.dim) > 1 && !property.ismatrix,
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
    //} while(gotoPreorderSucc_SkipFunctionDef(cursor, debug));
    } while(gotoPreorderSucc(cursor, debug));
    
    let count = 0;
    cursor = tree.walk();
    do {
        const c = cursor as g.TypedTreeCursor;
        switch (c.nodeType) {
            case g.SyntaxType.Assignment: {
                let node = c.currentNode;
                // If LHS is an identifier, type is same as RHS
                let [type, ndim, dim, ismatrix, ispointer, isstruct, cf, vt] = inferType(filename, node.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                
                let [l_type, l_ndim, l_dim, l_ismatrix, l_ispointer, l_isstruct, , ] = inferType(filename, node.leftNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                if (type_to_matrix_type.findIndex(x=>x.type == l_type) != -1 && type_to_matrix_type.findIndex(x=>x.type == type) == -1) {
                //if ((l_type != null && l_type != "unknown") && type == null) {
                    type = l_type;
                    ndim = l_ndim;
                    dim = l_dim;
                    ismatrix = l_ismatrix;
                    ispointer = l_ispointer;
                    isstruct = l_isstruct;
                }
                
                custom_functions = cf;
                var_types = vt;
                let scope = findVarScope(filename, node, block_idxs, "main", debug);
                if (node.leftNode.type == g.SyntaxType.Identifier || node.leftNode.type == g.SyntaxType.Attribute) {
                    let name = node.leftNode.text;
                    let v1 = filterByScope(var_types, name, node, 1);
                    if (v1.length > 0) {
                        count = count + 1;
                        v1 = v1[v1.length - 1];
                        if (scope[2] == v1.scope[2]) { // same block level
                            var_types = var_types.filter(function(e) { return JSON.stringify(e) !== JSON.stringify(v1)}); // replace if already in var_types
                            
                            let re = new RegExp(`\\b${v1.name}\\b`);
                            if (re.test(node.rightNode.text)) {
                                let next_node = node.parent.nextNamedSibling;
                                if (next_node == null) {
                                    next_node = node.parent.parent;
                                    if (next_node.type == g.SyntaxType.Module) {
                                        next_node = node.rightNode;
                                    }
                                }

                                v1.scope = [v1.scope[0], next_node.startIndex - 1, v1.scope[2]];
                                var_types.push(v1);
                                let v2 = {
                                    name: v1.name,
                                    type: type,
                                    ndim: ndim,
                                    dim: dim,
                                    ismatrix: ismatrix,
                                    isvector: numel(dim) > 1 && !ismatrix,
                                    ispointer: ispointer, //type == 'char' || ismatrix,
                                    isstruct: isstruct,
                                    scope: [next_node.startIndex, scope[1], scope[2]],
                                    initialized: true
                                }
                                var_types.push(v2);
                            } else {
                                v1.scope = [v1.scope[0], node.startIndex - 1, v1.scope[2]];
                                var_types.push(v1);
                                
                                let v2 = {
                                    name: v1.name,
                                    type: type,
                                    ndim: ndim,
                                    dim: dim,
                                    ismatrix: ismatrix,
                                    isvector: numel(dim) > 1 && !ismatrix,
                                    ispointer: ispointer, //type == 'char' || ismatrix,
                                    isstruct: isstruct,
                                    scope: [node.startIndex, scope[1], scope[2]],
                                    initialized: false //true
                                }
                                var_types.push(v2);
                            }
                        }
                    } else {
                        v1 = { 
                            name: node.leftNode.text, 
                            type: type, 
                            ndim: ndim, 
                            dim: dim, 
                            ismatrix: ismatrix,
                            isvector: numel(dim) > 1 && !ismatrix,
                            ispointer: ispointer, //type == 'char' || ismatrix,
                            isstruct: isstruct,
                            initialized: false,
                            scope: scope
                        };
                        var_types.push(v1);
                    }
                // If LHS is subscript, type is matrix
                } else if (node.leftNode.type == g.SyntaxType.CallOrSubscript || node.leftNode.type == g.SyntaxType.CellSubscript ) {
                //} else if (node.leftNode.type == g.SyntaxType.CallOrSubscript) {
                    let name = node.leftNode.valueNode.text;
                    let v3 = filterByScope(var_types, name, node, 0);
                    if (node.leftNode.type == g.SyntaxType.CellSubscript) {
                        var_types = var_types.filter(x => x.name != node.leftNode.text);
                        var_types.push({
                            name: node.leftNode.text,
                            type: type, 
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
                            
                    if (v3 != null && v3 != undefined) {
                        var_types = var_types.filter(function(e) { return JSON.stringify(e) !== JSON.stringify(v3)}); // replace if already in var_types
                        //var_types = var_types.filter(x => x.name != `${name}_init`);
                        let obj2 = var_types.find(x => x.name === `${name}_init`);
                        if (obj2 == null || obj2 == undefined) {
                            var_types.push({
                                name: `${name}_init`,
                                type: v3.type,
                                ndim: v3.ndim,
                                dim: v3.dim,
                                ismatrix: v3.ismatrix,
                                isvector: v3.isvector,
                                ispointer: v3.ispointer,
                                isstruct: false,
                                initialized: false,
                                scope: v3.scope
                            });
                        }
                        
                        v3.type = type;
                        
                        let obj = var_types.find(x => x.name === `${name}_childtype`);
                        if (obj != null && obj != undefined) {
                            let child_ndim = obj.ndim;
                            let child_dim = obj.dim;
                            let child_ismatrix = obj.ismatrix;
                            
                            if (dim.toString() != obj.dim.toString()) {
                                child_ndim = "unknown";
                                child_dim = "unknown";
                            }
                            if (ismatrix != obj.ismatrix) {
                                child_ismatrix = "unknown";
                            }
                            
                            var_types = var_types.filter(function(e) { return e.name !== `${name}_childtype`}); // replace if al
                            var_types.push({
                                name: `${name}_childtype`,
                                type: binaryOpType(type, obj.type),
                                ndim: child_ndim,
                                dim: child_dim,
                                ismatrix: child_ismatrix,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: false,
                                scope: null
                            });
                            
                        } else {
                            var_types.push({
                                name: `${name}_childtype`,
                                type: type,
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
                    } else {
                            v3 = { 
                                name: name, 
                                type: type, 
                                ndim: 2,
                                dim: [1,1],
                                ismatrix: true,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: false,
                                scope: scope
                            };
                    }
                    var_types.push(v3);
                }
                break;
            }
            case g.SyntaxType.ForStatement: {
                let node = c.currentNode;
                // If LHS is an identifier, type is same as RHS
                let [type, ndim, dim, ismatrix, ispointer, isstruct, cf, vt] = inferType(filename, node.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = cf;
                var_types = vt;
                let scope = findVarScope(filename, node, block_idxs, "main", debug);
                if (node.leftNode.type == g.SyntaxType.Identifier) {
                    let name = node.leftNode.text;
                    let v1 = filterByScope(var_types, name, node, 1);
                    if (v1.length > 0) {
                        count = count + 1;
                        v1 = v1[v1.length - 1];
                        
                        if (scope[2] == v1.scope[2]) { // same block level
                            var_types = var_types.filter(function(e) { return JSON.stringify(e) !== JSON.stringify(v1)}); // replace if already in var_types
                            
                            v1.scope = [v1.scope[0], node.startIndex - 1, v1.scope[2]];
                            var_types.push(v1);
                            
                            let v2 = {
                                name: v1.name,
                                type: type,
                                ndim: ndim,
                                dim: dim,
                                ismatrix: ismatrix,
                                isvector: numel(dim) > 1 && !ismatrix,
                                ispointer: ispointer, //type == 'char' || ismatrix,
                                isstruct: isstruct,
                                scope: [node.startIndex, scope[1], scope[2]],
                                initialized: false //true
                            }
                            var_types.push(v2);
                            
                        } else {
                            v1 = { 
                                name: name, 
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
                            var_types.push(v1);
                        }
                    } else {
                        v1 = { 
                            name: name, 
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
                        var_types.push(v1);
                    }
                }
                break;
            }
            case g.SyntaxType.CallOrSubscript: { // This helps update the argument types of function definitions
                let node = c.currentNode;
                let [, , , , , , cf, vt] = inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = cf;
                var_types = vt;
                break;
            }
        }
    } while(gotoPreorderSucc_SkipFunctionDef(cursor, debug));
    return [var_types, custom_functions, block_idxs];
}

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
    
    let obj = fun_dictionary.find(x => x.name === fun_name);
    //console.log(obj.file);
    
    
    if (obj != null) {
        let tree2 = parser.parse(obj.def_node.bodyNode.text);
        for (let i = 0; i < arg_types.length; i++) {
            arg_types[i].scope = [0, obj.def_node.endIndex - obj.def_node.startIndex, -1]; // "transpose" since passing adjusted tree
            arg_types[i].initialized = true;
        }
        // for some reason calling inferTypeFromAssignment modifies the value of arg_types
        const tmp_arg_types = JSON.parse(JSON.stringify(arg_types));
        const tmp_var_arg_types = JSON.parse(JSON.stringify(obj.var_arg_types));
        let typestr = '';
        for  (let i = 0; i < tmp_arg_types.length; i++) { 
            typestr = typestr.concat(tmp_arg_types[i].type);
        }
        let block_idxs = [[0, obj.def_node.endIndex - obj.def_node.startIndex, 0]];
        
        //let [var_types2, c, var_types] = inferTypeFromAssignment(filename, tree2, arg_types, custom_functions, classes, file, alias_tbl, debug, block_idxs);
        let [var_types2, c, block_idxs2] = inferTypeFromAssignment("main", tree2, arg_types.concat(var_types), custom_functions, classes, obj.file, alias_tbl, debug, block_idxs);
        //let [var_types2, c, var_types] = inferTypeFromAssignment(filename, tree2, arg_types.concat(var_types), custom_functions, classes, file, alias_tbl, debug, block_idxs);
        for (let i = 0; i < var_types2.length; i++) {
            if (var_types2[i].scope != null) {
                if (var_types2[i].scope[0] == 0) {
                    var_types2[i].scope[0] += obj.def_node.startIndex;
                } else {
                    var_types2[i].scope[0] += obj.def_node.bodyNode.startIndex;
                }
                if (var_types2[i].scope[1] == obj.def_node.endIndex - obj.def_node.startIndex) {
                    var_types2[i].scope[1] += obj.def_node.startIndex;
                } else {
                    var_types2[i].scope[1] += obj.def_node.bodyNode.startIndex;
                }
                if (var_types2[i].scope[2] == -1) {
                    var_types2[i].scope[2] = 0;
                }
            }
        }
        
        for (let i = 0; i < tmp_arg_types.length; i++) {
            tmp_arg_types[i] = var_types2.find(x=>x.name == tmp_arg_types[i].name);
        }
        
        if (!isclass) {
            fun_dictionary = c; // may need to change for classes
            //obj = fun_dictionary.find(x => x.name === fun_name);
        }
        custom_functions = c;
	    let obj2 = obj.arg_type_dic.find(x=>x.arg_type_id == typestr);
        //let obj3 = obj.arg_type_dic.filter(x=>JSON.stringify(x.var_types) == JSON.stringify(var_types2));
        let return_node = obj.def_node.return_variableNode;
        if (return_node != undefined) {
            return_node = return_node.firstChild;
            // If multiple return values, use pointers
            if (return_node.type == g.SyntaxType.Matrix) {
                let all_types = [];
                let ptr_arg_types = [];
                for (let i = 0; i < return_node.namedChildCount; i++) {
                    let return_var = return_node.namedChildren[i];
                    let [return_type, return_ndim, return_dim, return_ismatrix, return_ispointer, return_isstruct, c, vt] = inferType(filename, return_var, var_types2, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    var_types2 = vt;
                    /*if (obj.ptr_arg_types != null) {
                        return_type = binaryOpType(return_type, obj.ptr_arg_types[i].type);
                    }*/
                    ptr_arg_types.push({
        		        type: return_type,
        		        ndim: return_ndim,
        		        dim: return_dim,
        		        ismatrix: return_ismatrix,
        		        isvector: numel(return_dim) > 1 && !return_ismatrix,
        		        ispointer: true,
                        isstruct: return_isstruct
        		    });
                }
                
                if (obj2 == null || obj2 == undefined) {
                    obj.arg_type_dic = obj.arg_type_dic.filter(x=>x.arg_type_id != typestr);
        	        obj.arg_type_dic.push({
            		    arg_type_id: typestr,
            		    arg_types: tmp_arg_types,
            		    ptr_arg_types: ptr_arg_types,
            		    return_type: null,
            		    var_types: var_types2
            		})
            		
        	    }
                const v1: CustomFunction = { 
                    name: obj.name,
                    arg_types: tmp_arg_types,
                    var_arg_types: tmp_var_arg_types,
                    return_type: null,
                    outs_transform: (outs) => null,
                    external: obj.external,
                    file: obj.file,
                    def_node: obj.def_node,
                    ptr_arg_types: ptr_arg_types,
                    ptr_args: (arg_types, outs) => {
                        let ptr_args = [];
                        for (let i = 0; i < return_node.namedChildCount; i++) {
                            let return_var = return_node.namedChildren[i];
                            //let [return_type, return_ndim, return_dim, return_ismatrix, return_ispointer, return_isstruct, c, var_types] = inferType(filename, return_var, var_types2, custom_functions, classes, file, alias_tbl, debug);
                            //all_types.push(return_type);
                            all_types.push(ptr_arg_types[i].type);
                            custom_functions = c;
                            let return_name = return_var.text;
                            if (outs.length > i) {
                                return_name = outs[i];
                            }
                            ptr_args.push({
            			        name: return_name,
            			        type: ptr_arg_types[i].type, //return_type,
            			        ndim: ptr_arg_types[i].ndim, //return_ndim,
            			        dim: ptr_arg_types[i].dim, //return_dim,
            			        ismatrix: ptr_arg_types[i].ismatrix, //return_ismatrix,
            			        isvector: ptr_arg_types[i].isvector, //numel(return_dim) > 1 && !return_ismatrix,
            			        ispointer: true,
                                isstruct: ptr_arg_types[i].isstruct, //return_isstruct
            			    });
                        }
            			return ptr_args;
            		},
            		var_types: var_types2,
            		arg_type_dic: obj.arg_type_dic
                };
                if (!all_types.includes("unknown")) {
                    fun_dictionary = fun_dictionary.filter(function(e) { return e.name !== fun_name });
                    fun_dictionary.push(v1);
                }
                return [v1.return_type, fun_dictionary, block_idxs2];

            // If single return value, don't use pointers 
            } else {
                let [type, ndim, dim, ismatrix, ispointer, isstruct, c, vt] = inferType(filename, return_node, var_types2, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = c;
                var_types2 = vt;
                let return_type = {
                    type: type,
                    ndim: ndim,
                    dim: dim,
                    ismatrix: ismatrix,
                    isvector: numel(dim) > 1 && !ismatrix,
                    ispointer: ispointer,
                    isstruct: isstruct
                };
                
                if (obj2 == null || obj2 == undefined) {
                    obj.arg_type_dic = obj.arg_type_dic.filter(x=>x.arg_type_id != typestr);
        	        obj.arg_type_dic.push({
            		    arg_type_id: typestr,
            		    arg_types: tmp_arg_types,
            		    ptr_arg_types: null,
            		    return_type: return_type,
            		    var_types: var_types2
            		})
        	    }
        	    //console.log("UNICORNS!!!!!");
        	    //console.log(obj.var_arg_types);
        	    //console.log("------------------------");
                const v1: CustomFunction = { 
                    name: obj.name, 
                    arg_types: tmp_arg_types,
                    var_arg_types: tmp_var_arg_types,
                    // arg_types.push(tmp_arg_types)
                    outs_transform: (outs) => outs,
                    return_type: {
                        type: type,
                        ndim: ndim,
                        dim: dim,
                        ismatrix: ismatrix,
                        isvector: numel(dim) > 1 && !ismatrix,
                        ispointer: ispointer,
                        isstruct: isstruct
                    },
                    ptr_args: (arg_types, outs) => null,
                    ptr_arg_types: null,
                    external: obj.external,
                    file: obj.file,
                    def_node: obj.def_node,
                    var_types: var_types2,
            		arg_type_dic: obj.arg_type_dic
                };
                if (type !== "unknown") {
                    fun_dictionary = fun_dictionary.filter(function(e) { return e.name !== fun_name });
                    fun_dictionary.push(v1);
                }
                return [v1.return_type, fun_dictionary, block_idxs2];
                
            }
        } else {
            
            if (obj2 == null || obj2 == undefined) {
                obj.arg_type_dic = obj.arg_type_dic.filter(x=>x.arg_type_id != typestr);
    	        obj.arg_type_dic.push({
        		    arg_type_id: typestr,
        		    arg_types: tmp_arg_types,
        		    ptr_arg_types: null,
        		    return_type: null,
        		    var_types: var_types2
        		})
    	    }
            const v1: CustomFunction = { 
                name: obj.name,
                arg_types: tmp_arg_types,
                var_arg_types: tmp_var_arg_types,
                outs_transform: (outs) => outs,
                return_type: null,
                //ptr_param: null, 
                //ptr_declaration: null,
                ptr_args: (arg_types, outs) => null,
                ptr_arg_types: null,
                external: obj.external,
                file: obj.file,
                def_node: obj.def_node,
                var_types: var_types2,
                arg_type_dic: obj.arg_type_dic
            };
            if (arg_types[0].type !== "unknown") {
                fun_dictionary = fun_dictionary.filter(function(e) { return e.name !== fun_name });
                fun_dictionary.push(v1);
            }
            return [v1.return_type, fun_dictionary, block_idxs2];
            
        }
    }
}

function inferTypeByName(name, node, var_types, custom_functions, alias_tbl, debug) {
    if (debug == 1) {
        console.log("inferTypeByName");
    }
    
    let obj1 = filterByScope(var_types, name, node, 0);
    if (obj1 != null && obj1 != undefined) {
        return [obj1.type, obj1.ndim, obj1.dim, obj1.ismatrix, obj1.isvector, obj1.ispointer, obj1.isstruct, custom_functions];
    }
    
    let obj2 = filterByScope(alias_tbl, name, node, 0);
    if (obj2 != null && obj2 != undefined) {
        let obj3 = var_types.find(x => x.name === obj2.tmp_var);
        if (obj3 != null && obj3 != undefined) {
            return [obj3.type, obj3.ndim, obj3.dim, obj3.ismatrix, obj3.isvector, obj3.ispointer, obj3.isstruct, custom_functions];
        }
    }
    
    if (name == "INT_MAX" || name == "INT_MIN") {
        return ['int', 1, [1], false, false, false, false, custom_functions];
    }
    
    return [null, null, null, null, null, null, null, null];
    
}

function inferType(filename, node, var_types, custom_functions, classes, file, alias_tbl, debug) {
    /*console.log("INFERTYPE");
    if (node == undefined) {
        console.log("UNDEFINED");
    } else {
        console.log(node.text);
        console.log(node.type);
    }
    console.log("-----------------------");*/
    
    
    
    if (debug == 1) {
        console.log("inferType");
    }
    // var unknown_type = ['unknown', null, null, null, null, null, custom_functions];
    // var unknown_type = ['unknown', 2, [1, 1], false, false, false, custom_functions];
    let unknown_type = [`${node.text}_type`, 2, [`${node.text}_dim0`, `${node.text}_dim1`], null, null, null, custom_functions, var_types];
    
    //let obj1 = alias_tbl.find(x => x.name === node.text);
    let obj1 = filterByScope(alias_tbl, node.text, node, 0);
    
    if (obj1 != null && obj1 != undefined) {
        let obj2 = var_types.find(x => x.name === obj1.tmp_var);
        if (obj2 != null && obj2 != undefined) {
            return [obj2.type, obj2.ndim, obj2.dim, obj2.ismatrix, obj2.ispointer, obj2.isstruct, custom_functions, var_types];
        }
    }
        
        
    switch (node.type) {
        case g.SyntaxType.Comment: {
            if (fun_obj != null) {
                if (fun_obj.var_arg_types == null) {
                    if (node.text.includes("Varargin")) {
                        varargin_flag = true;
                    }
                    if (varargin_flag) {
                        let match = node.text.match(/(\w*)\: ([a-zA-Z]*)( matrix)?/);
                        if (match != null) {
                            if ( varargin.findIndex( x => x.name==match[1] ) == -1 ) {
                                if (match[3] != undefined) {
                                    varargin.push({
                                        name: match[1],
                                        type: match[2],
                                        ndim: `${match[1]}_ndim`,
                                        dim: [`${match[1]}_dim0`, `${match[1]}_dim1`],
                                        ismatrix: true,
                                        isvector: false,
                                        ispointer: false,
                                        isstruct: false,
                                        initialized: true,
                                        scope: [0, fun_obj.def_node.endIndex - fun_obj.def_node.startIndex, 0]
                                    })
                                } else {
                                    varargin.push({
                                        name: match[1],
                                        type: match[2],
                                        ndim: 1,
                                        dim: [1],
                                        ismatrix: false,
                                        isvector: false,
                                        ispointer: false,
                                        isstruct: false,
                                        initialized: true,
                                        scope: [0, fun_obj.def_node.endIndex - fun_obj.def_node.startIndex, 0]
                                    })
                                }
                            }
                            
                        }
                    }
                    
                    if (node.text.includes("Outputs")) {
                        varargin_flag = false;
                        fun_obj.var_arg_types = varargin;
                        custom_functions.filter(x=>x.name != fun_obj.name);
                        custom_functions.push(fun_obj);
                    }
                }
            }
            return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
            break;
        }
            
        case g.SyntaxType.ParenthesizedExpression: {
            return inferType(filename, node.firstNamedChild, var_types, custom_functions, classes, file, alias_tbl, debug);
            break;
        }
        
        // Named types
        case g.SyntaxType.Ellipsis: {
            //return ['ellipsis', 2, [1, 1], false, false, false, custom_functions];
            return ['ellipsis', 1, [1], false, false, false, custom_functions, var_types];
            break
        }
        case (g.SyntaxType.True || g.SyntaxType.False): {
            //return ['bool',  2, [1, 1], false, false, false, custom_functions];
            return ['bool',  1, [1], false, false, false, custom_functions, var_types];
            break
        }
        case g.SyntaxType.Float: {
            //return ['float',  2, [1, 1], false, false, false, custom_functions];
            return ['double',  1, [1], false, false, false, custom_functions, var_types];
            break
        }
        case g.SyntaxType.Integer: {
            //return ['int',  2, [1, 1], false, false, false, custom_functions];
            return ['int',  1, [1], false, false, false, custom_functions, var_types];
            break
        }
        case g.SyntaxType.Complex: {
            //return ['complex',  2, [1, 1], false, false, false, custom_functions];
            return ['complex',  1, [1], false, false, false, custom_functions, var_types];
            break
        }
        case g.SyntaxType.String: {
            //return ['char',  2, [1, 1], false, true, false, custom_functions];
            return ['char',  2, [1, node.text.length], false, true, false, custom_functions, var_types];
            //return ['char',  1, [node.text.length], false, true, false, custom_functions];
            break
        }
        case g.SyntaxType.Cell:
        case g.SyntaxType.Matrix: {
            let row = 0;
            let col = 0;
            //let nrows = 0;
            //let ncols = 0;
            let nrows = '0';
            let ncols = '0';
            for (let i=0; i<node.childCount; i++) {
                if (node.children[i].type === ";") {
                    row += 1;
                    col = 0;
                }
                else if (node.children[i].isNamed) {
                    
                    if (row == 0) {
                        let [type, ndim, dim,,,, c, vt] = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                        custom_functions = c;
                        var_types = vt;
                        if (dim == null) {
                            console.error("ERROR IN CELL/MATRIX: CHILD DIM IS NULL");
                        } else {
                            if (ndim > 1) {
                                //ncols += dim[1];
                                ncols += `+${dim[1]}`;
                            } else {
                                //ncols += dim[0];
                                ncols += `+${dim[0]}`;
                            }
                        }
                    }
                    if (col == 0) {
                        let [type, ndim, dim,,,, c, vt] = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                        custom_functions = c;
                        var_types = vt;
                        //nrows += dim[0];
                        if (dim == null) {
                            console.error("ERROR IN CELL/MATRIX: CHILD DIM IS NULL");
                        } else {
                            nrows += `+${dim[0]}`;
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
            let children_types = [];
            let children_ndim = [];
            let children_dim = [];
            let children_ismatrix = [];
            
            for (let child of node.namedChildren) {
                let [child_type, child_ndim, child_dim, child_ismatrix,,, c, vt] = inferType(filename, child, var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = c;
                var_types = vt;
                children_types.push(child_type);
                children_ndim.push(child_ndim);
                children_dim.push(child_dim);
                children_ismatrix.push(child_ismatrix);
            }

            let type = 'unknown';
            let child_ndim = 1;
            let child_dim = [1];
            let child_ismatrix = false;
            
            if (children_types.every(val => val === children_types[0])) {
                type = children_types[0];
                
            } else if (children_types.every(val => ['int','double','complex'].includes(val))) {
                
                if (children_types.includes('complex')) {
                    type = 'complex';
                } else if (children_types.includes('double')) {
                    type = 'double';
                } else if (children_types.includes('int')) {
                    type = 'int'; 
                }
            } else {
                type = 'heterogeneous';
            }
            
            if (children_dim.every(x=>x!=null)) {
                if (children_ndim.every(x => x === children_ndim[0])) {
                    child_ndim = children_ndim[0];
                }
                if (children_dim.every(x => x.toString() === children_dim[0].toString())) {
                    child_dim = children_dim[0];
                }
                if (children_ismatrix.every(x => x === children_ismatrix[0])) {
                    child_ismatrix = children_ismatrix[0];
                }
            }
            
            if (node.type == g.SyntaxType.Cell && node.parent.type == g.SyntaxType.Assignment) {
                var_types.push({
                    name: `${node.parent.leftNode.text}_childtype`,
                    type: type,
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
            
            //return [type, 2, [nrows, ncols], true, true, false, custom_functions];
            return [type, 2, [nrows, ncols], true, false, false, custom_functions, var_types];
            break;
        }
            
        case g.SyntaxType.ComparisonOperator:
        case g.SyntaxType.BooleanOperator:
        case g.SyntaxType.TransposeOperator:
        case g.SyntaxType.UnaryOperator:
        case g.SyntaxType.BinaryOperator: {
            let [args, arg_types, outs] = parseFunctionCallNode(node);
            let obj = findBuiltin(operatorMapping, node.operatorNode.type);
            let return_type = obj.return_type(args, arg_types, outs);
            let arg_types_transform = obj.arg_types_transform(args, arg_types, outs);
            if (arg_types_transform != null) {
                return_type = obj.return_type(args, arg_types_transform, outs);
                for (let i = 0; i < arg_types_transform.length; i++) {
                    let obj2 = filterByScope(var_types, arg_types_transform[i].name, node, 0);
                    if (obj2 != null) {
                        arg_types_transform[i].scope = obj2.scope;
                        var_types = var_types.filter(x => JSON.stringify(x) !== JSON.stringify(obj2));
                    } else {
                        //arg_types_transform[i].scope = findVarScope(filename, node, block_idxs, "main", debug);
                    }
                    var_types.push(arg_types_transform[i]);
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
        case g.SyntaxType.Attribute: {
            // First check if class method
            let [type,,,,,, c, vt] = inferType(filename, node.objectNode, var_types, custom_functions, classes, file, alias_tbl, debug);
            custom_functions = c;
            var_types = vt;
            let obj = classes.find(x => x.name === type);
            if (obj !== null && obj !== undefined) {
                let obj2 = obj.methods.find(x => x.name === node.attributeNode.text);
                if (obj2 != null && obj2 != undefined) {
                    if (obj2.return_type == null) {

                        return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                    } else {

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

                let obj3 = obj.properties.find(x => x.name === node.attributeNode.text);
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
            obj = filterByScope(var_types, node.text, node, 0);
            if (obj != null) {
                return [obj.type, obj.ndim, obj.dim, obj.ismatrix, obj.ispointer, obj.isstruct, custom_functions, var_types];
            } else {
                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
            }
            break;
            
        }
        // Identifier
        case g.SyntaxType.Identifier: {
            if (node.text == "INT_MAX" || node.text == "INT_MIN") {
                return ['int', 1, [1], false, false, false, custom_functions, var_types];
            }
            let obj = filterByScope(var_types, node.text, node, 0);
            //let obj = var_types.find(x => (x.name == node.text) && (node.startIndex >= x.scope[0]) && (node.endIndex <= x.scope[1]));
            //let obj = var_types.filter(function(e) { return e.name == node.text });
            if (obj != null) {
                return [obj.type, obj.ndim, obj.dim, obj.ismatrix, obj.ispointer, obj.isstruct, custom_functions, var_types];
            } else {
                //return ['unknown', 2, [1, 1], false, false, false, custom_functions];
                return unknown_type;
            }
            break;
        }
        // TO DO: FIX THIS
        case g.SyntaxType.CellSubscript: {
            let [parent_type, parent_ndim, parent_dim,parent_ismatrix,,parent_isstruct, cf, vt] = inferType(filename, node.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug);
            custom_functions = cf;
            var_types = vt;
            let dim = [];
            let dim_idx = 0;
            for (let i=2; i<node.childCount; i++) {
            //for (let i=1; i<node.namedChildCount; i++) {
                if (node.children[i].isNamed) {
                    dim_idx += 1;
                    //var [child_type,,child_dim,child_ismatrix,,, c, var_types] = inferType(filename, node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                    var [child_type,,child_dim,child_ismatrix,,, c, vt2] = inferType(filename, node.children[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                    /*console.log("NODE CHILD");
                    console.log(node.text);
                    console.log(node.children[i].text);
                    console.log(child_type);*/
                    custom_functions = c;
                    var_types = vt2;
                    //dim.push(child_dim[1]);
                    if (child_dim == null) {
                        console.error("ERROR IN CELLSUB: CHILD DIM IS NULL");
                    } else {
                        if (child_dim.length > 1) {
                            dim.push(child_dim[1]);
                        } else {
                            dim.push(child_dim[0]);
                        }
                    }
                } else if (node.children[i].type == ":") {
                    if (node.namedChildCount == 1) {
                        dim.push(numel(parent_dim));
                    } else {
                        dim.push(parent_dim[dim_idx]);
                        dim_idx += 1;
                    }
                }
            }

            if (dim.length==1 && dim[0] == 1) {
                dim = [1,1];
            }
            
            let ismatrix = !dim.every(val => val === 1);
            let obj = var_types.find(x => x.name === `${node.valueNode.text}_childtype`);
            
            if (obj != null && obj!= undefined) {
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
        
        case g.SyntaxType.CallOrSubscript: {
            let [args, arg_types, outs] = parseFunctionCallNode(node);
            let obj = classes.find(x => x.name === node.valueNode.text);
            // Is a class (treat as structure)
            if (obj != null) {
                return [obj.name, 2, [1,1], false, false, true, custom_functions, var_types];
            }
            else if (args.length > 0) {
                // Is a function call
                let filename2 = file.match(/((?<=\/)([^\/]*?)(?=\.m))|(^([^\/]*?)(?=\.m))/);
                // Avoid recursive function calls
                if (filename2[0] != node.valueNode.text) {
                    //let obj1 = classes.find(x => x.name === arg_types[0].type);
                    
                    let obj1 = classes.find(x => arg_types.some(y => y.type == x.name));
                    let obj2 = custom_functions.find(x => x.name === node.valueNode.text);
                    let obj3 = findBuiltin(builtin_functions, node.valueNode.text);
                    
                    if (obj1 != null && obj1 != undefined) {
                        let obj = obj1.methods.find(x => x.name === node.valueNode.text);
                        
                        if (obj != null && obj != undefined) {
                            for (let i=0; i < arg_types.length; i++) {
                                obj.arg_types[i].type = arg_types[i].type;
                                obj.arg_types[i].ndim = arg_types[i].ndim;
                                obj.arg_types[i].dim = arg_types[i].dim;
                                obj.arg_types[i].ismatrix = arg_types[i].ismatrix;
                                obj.arg_types[i].isstruct = arg_types[i].isstruct;
                                obj.arg_types[i].ispointer = arg_types[i].ispointer;
                            }
                            let return_type = null;
                            //[return_type, obj1.methods] = getFunctionReturnType(filename, node.valueNode.text, obj.arg_types, obj1.methods, custom_functions, classes, file, alias_tbl, debug); 
                            //[return_type, obj1.methods] = getFunctionReturnType(filename, node.valueNode.text, obj.arg_types, var_types, obj1.methods, custom_functions, classes, file, alias_tbl, debug, 1); 
                            [return_type, obj1.methods, ] = getFunctionReturnType(filename, node.valueNode.text, obj.arg_types, [], obj1.methods, custom_functions, classes, file, alias_tbl, debug, 1); 
                            
                            if (return_type == null) {
                                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                            } else {
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
                        let class_name = obj2.file.match(/(?<=@)(.*?)(?=\/)/);
                        let flag = true;
                        if (class_name != null) {
                            //if (arg_types[0].type != class_name) {
                            if (arg_types.every(x => x.type != class_name)) {
                                flag = false;
                            }
                        }
                        if (flag == true) {
                            //TO DO: figure out way to deal with varargin
                            for (let i=0; i < obj2.arg_types.length; i++) {
                            //for (let i=0; i < arg_types.length; i++) {
                                if (arg_types[i].type !== "unknown" && arg_types[i].type !== null) {
                                    obj2.arg_types[i].type = arg_types[i].type;
                                    obj2.arg_types[i].ndim = arg_types[i].ndim;
                                    obj2.arg_types[i].dim = arg_types[i].dim;
                                    obj2.arg_types[i].ismatrix = arg_types[i].ismatrix;
                                    obj2.arg_types[i].isstruct = arg_types[i].isstruct;
                                    obj2.arg_types[i].ispointer = arg_types[i].ispointer;
                                }
                            }
                            let var_types3 = [];
                            if (obj2.var_arg_types != null) {
                                var_types3 = obj2.var_arg_types;
                            }
                            
                            let return_type = null;
                            //[return_type, custom_functions] = getFunctionReturnType(filename, node.valueNode.text, obj2.arg_types, var_types, custom_functions, custom_functions, classes, file, alias_tbl, debug, 0);
                            [return_type, custom_functions, ] = getFunctionReturnType(filename, node.valueNode.text, obj2.arg_types, var_types3, custom_functions, custom_functions, classes, file, alias_tbl, debug, 0);
                            
                            if (return_type == null) {
                                return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                            } else {
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
                        let return_type = obj3.return_type(args, arg_types, outs);
                        if (return_type == null) {
                            return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                        } else {
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
                } else {
                    return ['unknown', 2, [1, 1], false, false, false, custom_functions, var_types];
                }
            }
            let [parent_type,,,parent_ismatrix,,parent_isstruct, c, vt] = inferType(filename, node.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug);
            custom_functions = c;
            var_types = vt;
            
            // Is a subscript
            if (parent_ismatrix || parent_isstruct) {
                let dim = [];
                for (let i=1; i<node.namedChildCount; i++) {
                    let [,,child_dim,,,, c, vt] = inferType(filename, node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    var_types = vt;
                    if (child_dim == null) {
                        console.error("ERROR IN CALLORSUB: CHILD DIM IS NULL");
                    } else {
                        if (child_dim.length > 1) {
                            dim.push(child_dim[1]);
                        } else {
                            dim.push(child_dim[0]);
                        }
                    }
                }
                
                if (dim.length==1 && dim[0] == 1) {
                    dim = [1,1];
                }
                if (dim.every(val => val === 1)) {
                    return [parent_type, dim.length, dim, false, false, parent_isstruct, custom_functions, var_types];
                } else {
                    //return [parent_type, dim.length, dim, true, true, parent_isstruct, custom_functions];
                    return [parent_type, dim.length, dim, true, false, parent_isstruct, custom_functions, var_types];
                }
            }
            return ["unknown", 1, [1], false, false, false, custom_functions, var_types];
            
            break;
        }
        
        case g.SyntaxType.Slice: {

            let children_types = [];
            let children_vals = []
            
            for (let i=0; i<node.namedChildCount; i++) {
                let child = node.namedChildren[i];
                let [child_type,,,,,, c, vt] = inferType(filename, child, var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = c;
                var_types = vt;
                
                if (child_type == "keyword") {
                    
                    let [,ndim,dim,,,, c, vt] = inferType(filename, node.parent.valueNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                    if (dim != null) {
                        custom_functions = c;
                        var_types = vt;
                        let firstNode = node.parent.namedChildren[1];
                        let current_dim = 0;
                        let dummyNode = node;
                        while (JSON.stringify(dummyNode) != JSON.stringify(firstNode)) {
                            dummyNode = dummyNode.previousNamedSibling;
                            current_dim += 1;
                        }
                        
                        children_vals.push(dim[current_dim]);
                    } else {
                        console.error("ERROR IN SLICE: PARENT DIM IS NULL");
                        children_vals.push(1);
                    }
                    children_types.push('int');
                } else {
                    children_vals.push(Number(child.text));
                    children_types.push(child_type);
                }
                
            }
            
            
            var type = 'unknown';
            if (children_types.every(val => ['int','double','complex'].includes(val))) {
                
                if (children_types.includes('complex')) {
                    type = 'complex';
                } else if (children_types.includes('double')) {
                    type = 'double';
                } else if (children_types.includes('int')) {
                    type = 'int'; 
                }
            }
            
            let start = children_vals[0];
            var stop = children_vals[1];
            var step = 1;
                
            if (children_vals.length == 3) {
                stop = children_vals[2];
                step = children_vals[1];
            }
            
            let len = Math.floor((stop-start)/step) + 1;
            
            //let len = Math.round((stop-start)/step) + 1;
            // TO DO: Maybe change so that it's only pointer, not a matrix and
            // represented in generateCode by creating an array, not a matrix
            //return [type, 2, [1, len], true, true, false, custom_functions];
            return [type, 2, [1, len], false, false, false, custom_functions, var_types];
        }
        
        case g.SyntaxType.Keyword: {
            return ['keyword', 1, [1], false, false, false, custom_functions, var_types]
        }
        
        // Default
        default: return ['unknown', 1, [1], false, false, false, custom_functions, var_types];
    }
    
    // Return args, arg_types, outs from function
    function parseFunctionCallNode(node) {
        if (debug == 1) {
            console.log("parseFunctionCallNode");
        }
        if (node.parent.type == g.SyntaxType.Assignment) {
            return parseFunctionCallNode(node.parent);
        } else {
            switch (node.type) {
                case g.SyntaxType.Assignment: {
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
            let args = [];
            let arg_types: Type[] = [];
            switch (right_node.type) {
                case g.SyntaxType.CallOrSubscript: {
                    for (let i = 1; i < right_node.namedChildCount; i++) {
                        //if (transformNode(right_node.namedChildren[i]) != undefined) {
                        //    args.push(transformNode(right_node.namedChildren[i]));   
                        //} else {
                            args.push(right_node.namedChildren[i].text);
                        //}
                        let [type, ndim, dim, ismatrix, ispointer, isstruct, c, vt] = inferType(filename, right_node.namedChildren[i], var_types, custom_functions, classes, file, alias_tbl, debug);
                        custom_functions = c;
                        var_types = vt;
                        arg_types.push({
                            type: type, 
                            ndim: ndim, 
                            dim: dim, 
                            ismatrix: ismatrix, 
                            isvector: numel(dim) > 1 && !ismatrix,
                            ispointer: ispointer,
                            isstruct: isstruct
                        });
                    }
                    break;
                }
                case g.SyntaxType.ComparisonOperator:
                case g.SyntaxType.BooleanOperator:
                case g.SyntaxType.BinaryOperator: {
                    let [l_type, l_ndim, l_dim, l_ismatrix, l_ispointer, l_isstruct, c1, vt1] = inferType(filename, right_node.leftNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c1;
                    var_types = vt1;
                    let [r_type, r_ndim, r_dim, r_ismatrix, r_ispointer, r_isstruct, c2, vt2] = inferType(filename, right_node.rightNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c2;
                    var_types = vt2;
                    arg_types.push({
                        type: l_type, 
                        ndim: l_ndim, 
                        dim: l_dim, 
                        ismatrix: l_ismatrix, 
                        isvector: numel(l_dim) > 1 && !l_ismatrix,
                        ispointer: l_ispointer,
                        isstruct: l_isstruct
                    });
                    arg_types.push({
                        type: r_type, 
                        ndim: r_ndim, 
                        dim: r_dim, 
                        ismatrix: r_ismatrix, 
                        isvector: numel(r_dim) > 1 && !r_ismatrix,
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
                case g.SyntaxType.UnaryOperator:
                case g.SyntaxType.TransposeOperator: {
                    let [type, ndim, dim, ismatrix, ispointer, isstruct, c, vt] = inferType(filename, right_node.argumentNode, var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    var_types = vt;
                    arg_types.push({
                        type: type, 
                        ndim: ndim, 
                        dim: dim, 
                        ismatrix: ismatrix, 
                        isvector: numel(dim) > 1 && !ismatrix,
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
            
            let outs = [];
            if (left_node == null) {
            } else if (left_node.type == g.SyntaxType.Matrix) {
                for (let child of left_node.namedChildren) {
                    //if (transformNode(child) != undefined) {
                    //    outs.push(transformNode(child));   
                    //} else {
                        outs.push(child.text);
                    //}
                }
            } else {
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
    
export {Type, VarType, typeInference, inferType, inferTypeByName, inferTypeFromAssignment, getFunctionReturnType};