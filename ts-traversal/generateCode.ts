const fs = require("fs");
var path = require("path");
import * as ts from "typescript";
import * as g from "./generated";
import { Type, VarType, TmpVar, Alias, MainQueue, type_to_matrix_type, type_to_cell_type } from "./customTypes";
import { pushToMain, insertMain, replaceMain } from "./modifyCode";
import { 
    parseFunctionDefNode, 
    numel, 
    generateTmpVar, 
    pushAliasTbl, 
    writeToFile, 
    filterByScope, 
    findLastSubscript,
    transformNodeByName,
    initVar,
    extractSingularMat,
    findBuiltin,
    isInitialized
} from "./helperFunctions";
import { inferType, inferTypeByName, findVarScope } from "./typeInference";
//import { sub2idx, rowMajorFlatIdx, slice2list, matrix2list } from "./convertSubscript";
import { sub2idx, slice2list, matrix2list } from "./convertSubscript";
import { 
    gotoPreorderSucc,
    gotoPreorderSucc_OnlyMajorTypes, 
    fileIsFunction,
    findEntryFunction
} from "./treeTraversal";
import { operatorMapping, builtin_functions } from "./builtinFunctions";
let builtin_funs = builtin_functions;
let printed_classes = [];
let dummy_count = 0;
// Main
export function generateCode(filename, tree, out_folder, custom_functions, classes, var_types, block_idxs, file, debug) {
    if (debug == 1) {
        console.log("generateCode");
    }
    
    let tmp_var_types = var_types;
    let entry_fun_node = findEntryFunction(tree, debug);
    
    let loop_iterators = [];
    
    var function_definitions = [];
    var function_declarations = [];

    var numCellStruct = 0;
    var numUnknownType = 0;
    
    var generated_code = [];
    var main_function = [];
    
    var header = [];
    
    var lhs_flag = false;
    
    var varargin_flag = false;
    var varargin = [];
    var n_args = 0;
    var fun_obj = null;
    
    var if_flag = false;
    var link = [`// Link
#include <stdio.h>
#include <stdbool.h>
#include <complex.h>
#include <string.h>
#include <matrix.h>
#include "../convertSubscript.h"
#include "./${filename}.h"`];
    //#include <${filename}.h>
    var cursor_adjust = false;
    var current_code = "main";
    var extract_singular_mat = false;
    
    let tmp_tbl: TmpVar[] = [];
    
    let alias_tbl: Alias[] = [];
    
    let main_queue: MainQueue[] = [];
    
    let block_level = 1;
    const MAXCHAR = 20;
    
    let fun_params = {
        filename: filename,
        file: file,
        tree: tree,
        entry_fun_node: entry_fun_node,
        out_folder: out_folder,
        custom_functions: custom_functions,
        classes: classes,
        var_types: var_types,
        alias_tbl: alias_tbl,
        block_idxs: block_idxs,
        debug: debug,
        main_function: [],
        function_definitions: [],
        function_declarations: [],
        current_code: "main",
        block_level: block_level,
        tmp_tbl: tmp_tbl
    }
    
    
    
    // Update function parameters
    // -----------------------------------------------------------------------------
    function updateFunParams(update_or_extract) {
        if (update_or_extract == 0) {
            fun_params = {
                filename: filename,
                file: file,
                tree: tree,
                entry_fun_node: entry_fun_node,
                out_folder: out_folder,
                custom_functions: custom_functions,
                classes: classes,
                var_types: tmp_var_types,
                alias_tbl: alias_tbl,
                block_idxs: block_idxs,
                debug: debug,
                main_function: main_function,
                function_definitions: function_definitions,
                function_declarations: function_declarations,
                current_code: current_code,
                block_level: block_level,
                tmp_tbl: tmp_tbl
            }
        } else {
            custom_functions = fun_params.custom_functions
            classes = fun_params.classes;
            tmp_var_types = fun_params.var_types;
            alias_tbl = fun_params.alias_tbl;
            main_function = fun_params.main_function;
            function_definitions = fun_params.function_definitions;
            function_declarations = fun_params.function_declarations;
            tmp_tbl = fun_params.tmp_tbl;
        }
    }
    
    
    // Main
    // -----------------------------------------------------------------------------
    function main() {
        if (debug == 1) {
            console.log("main");
        }
            
        let cursor = tree.walk();
        do {
            const c = cursor as g.TypedTreeCursor;
            let node = c.currentNode;
            current_code = "main";
            tmp_var_types = var_types;
            fun_obj = null;
            switch (node.type) {
                case g.SyntaxType.FunctionDefinition: {
                    let node2 = parseFunctionDefNode(node);
                    fun_obj = custom_functions.find(x => x.name === node2.nameNode.text);
                    n_args = fun_obj.arg_types.length;
                    if (fun_obj.var_arg_types != null) {
                        n_args -= 1;
                    }
                    
                    varargin = [];
                    current_code = node.nameNode.text;
                    if (entry_fun_node != null) {
                        if (node.nameNode.text == entry_fun_node.nameNode.text) {
                            current_code = "main";
                        }
                    }
                    
                    let obj = custom_functions.find(x => x.name === current_code);
                    if (obj != null && obj != undefined) {
                        tmp_var_types = obj.var_types;
                        if (tmp_var_types == null) {
                            tmp_var_types = [];
                        }
                    }
                    printFunctionDefDeclare(node);
                    break;
                }
                case g.SyntaxType.Comment:
                case g.SyntaxType.ExpressionStatement: {
                    let expression = transformNode(file, node);
                    if (expression != ";" && expression != null) {
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(expression, fun_params);
                    }
                    break;
                }
                case g.SyntaxType.IfStatement:
                case g.SyntaxType.WhileStatement:
                case g.SyntaxType.ForStatement: {
                    transformNode(file, node);
                    break;
                }
            }
            
            if (current_code == "main") {
                var_types = tmp_var_types;
            } else {
                let obj = custom_functions.find(x => x.name === current_code);
                if (obj != null && obj != undefined) {
                    obj.var_types = tmp_var_types;
                    custom_functions = custom_functions.filter(function(e) { return e.name !== obj.name });
                    custom_functions.push(obj);
                }
            }
        } while(gotoPreorderSucc_OnlyMajorTypes(cursor, debug));
        
        /*if (current_code == "main") {
            var_types = tmp_var_types;
        } else {
            let obj = custom_functions.find(x => x.name === current_code);
            if (obj != null && obj != undefined) {
                obj.var_types = tmp_var_types;
                custom_functions = custom_functions.filter(function(e) { return e.name !== obj.name });
                custom_functions.push(obj);
            }
        }*/
    }
    
    function transformNode(file, node) {
        let transformed_node = transformNodeInternal(file, node);
        let expression = transformed_node;
        
        if (extract_singular_mat == true) {
            let flag = transformed_node != null;
            if (node.parent.type == g.SyntaxType.Assignment) {
                if (node.parent.leftNode.text == node.text || node.parent.rightNode.text == node.text) {
                    flag = false;
                }
            }
            /*if (node.parent.type == g.SyntaxType.CallOrSubscript) {
                flag = false;
            }*/
    
            // if 1x1 matrix "flatten" to regular int, double, or complex
            if (flag) {
                let var_type = filterByScope(tmp_var_types, transformed_node, node, 0);
                if (var_type != null && var_type != undefined) {
                    if (var_type.ismatrix && var_type.dim.every(x => x === 1)) {
                        updateFunParams(0);
                        [expression, fun_params] = extractSingularMat(transformed_node, var_type, node, fun_params);
                        updateFunParams(1);
                    }
                }
            }
        }
        return expression;
    }
    // Transform node
    // -----------------------------------------------------------------------------
    function transformNodeInternal(file, node) {
        if (debug == 1) {
            console.log("transformNode");
        }
        
        /*console.log("TRANSFORM");
        console.log(node.text);
        console.log(node.type);
        console.log("----------------------------");*/
        
        // at each iteration, check each element of mainQueue, if condition true then push expression
        let idx = 0;
        let mq_len = main_queue.length;
        for (let i = 0; i < mq_len; i++) {
            let result = ts.transpile(main_queue[idx].condition);
            let runnalbe = eval(result);
            if (runnalbe) {
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(main_queue[idx].expression, fun_params); 
                main_queue.splice(idx,1);
                idx = idx - 1;
            }
        
            idx = idx + 1;
        }
        
        switch (node.type) {
            // Comments
            // TO DO: multiline comments
            case g.SyntaxType.Comment: {
                return node.text.replace("%","//");
                break;
            }
                
            case g.SyntaxType.IfStatement: {
                if_flag = true;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("if (" + transformNode(file, node.conditionNode) + ") {", fun_params);
                if_flag = false;
                block_level += 1;
                for (let i=1; i<node.namedChildCount; i++) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(transformNode(file, node.namedChildren[i]), fun_params);
                }
                block_level -= 1;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("\n}", fun_params);
                
                return null
                break;
            } 
            
            case g.SyntaxType.WhileStatement: {
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("while (" + transformNode(file, node.conditionNode) + ") {", fun_params);
                block_level += 1;
                for (let i=1; i<node.namedChildCount; i++) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(transformNode(file, node.namedChildren[i]), fun_params);
                }
                block_level -= 1;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("\n}", fun_params);
                
                return null
                break;
            }
                
            case g.SyntaxType.ForStatement: {
                let expression = [];
                let tmp_iter = generateTmpVar("iter", tmp_tbl);
                let type = 'int';
                //let tmp_iter = node.leftNode.text;
                if (node.rightNode.type == g.SyntaxType.Slice) {
                    let [,,dim,,,,,] = inferType(filename, node.rightNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    var children = [];
                    for (let i = 0; i < node.rightNode.namedChildCount; i ++) {
                        extract_singular_mat = true;
                        children.push(transformNode(file, node.rightNode.namedChildren[i]));
                        extract_singular_mat = false;
                        
                    }
                    let obj = tmp_var_types.find(x => x.name === node.leftNode.text);
                    //let type = 'int';
                    if (node.rightNode.namedChildCount == 3) {
                        [type,,,,,,,] = inferType(filename, node.rightNode.namedChildren[1], tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    }
                    
                    //expression.push(`for (${type} ${tmp_iter} = ${children[0]};`);
                    loop_iterators.push(tmp_iter);
                    updateFunParams(0);
                    if (type == "int") {
                        alias_tbl = pushAliasTbl(node.leftNode.text, tmp_iter, node.bodyNode, fun_params);
                    }
                    
                    tmp_var_types.push({
                        name: tmp_iter,
                        type: type,
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: findVarScope(filename, node, block_idxs, current_code, debug)
                    });
                    
                    let start = children[0];
                    let stop = children[1];
                    let step = 1;
                    
                    if (children.length == 3) {
                        stop = children[2];
                        step = children[1];
                        if (dim.some(x=>isNaN(x))) {
                            dim = [1, `(int) floor((${stop}-${start})/${step}) + 1`];
                            //dim = [1, `(int) round((${stop}-${start})/${step}) + 1`];
                        }
                        if (type == "int") {
                            expression.push(`for (${type} ${tmp_iter} = ${children[0]};`);
                            expression.push(`${tmp_iter} <= ${children[2]};`);
                            expression.push(`${tmp_iter} += ${children[1]}`);
                        } else {
                            expression.push(`for (${type} ${tmp_iter} = 0;`);
                            expression.push(`${tmp_iter} < ${dim[1]};`);
                            expression.push(`${tmp_iter}++`);
                        }
                        
                    } else {
                        expression.push(`for (${type} ${tmp_iter} = ${children[0]};`);
                        expression.push(`${tmp_iter} <= ${children[1]};`);
                        expression.push(`${tmp_iter}++`);
                    }
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(expression.join(" ") + ") {", fun_params);
                    
                } else if (node.rightNode.type == g.SyntaxType.Matrix) {
                    let tmp_var1 = generateTmpVar("mat", tmp_tbl); // the matrix
                    //var tmp_var2 = generateTmpVar("tmp", tmp_tbl); // the iterating variable
                    let [type, ndim, dim,,,, c,] = inferType(filename, node.rightNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    let obj = type_to_matrix_type.find(x => x.type === type);
                    if (obj != null) {
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(initializeMatrix(node.rightNode, tmp_var1), fun_params);
                    }
                    
                    // node.leftNode now equal to value of matrix tmp_var1 at index tmp_var2
                    
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`\n${type} ${node.leftNode.text};`, fun_params);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`for (int ${tmp_iter} = 1; ${tmp_iter} < ${numel(dim)}; ${tmp_iter}++ ) {`, fun_params);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`indexM(${tmp_var1}, &${node.leftNode.text}, 1, ${tmp_iter});`, fun_params);
                    loop_iterators.push(tmp_iter);
                } else if (node.rightNode.type == g.SyntaxType.Identifier) {
                    let tmp_var1 = transformNode(file, node.rightNode);
                    let [type, ndim, dim,,,, c,] = inferType(filename, node.rightNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`\n${type} ${node.leftNode.text};`, fun_params);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`for (int ${tmp_iter} = 1; ${tmp_iter} < ${numel(dim)}; ${tmp_iter}++ ) {`, fun_params);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`indexM(${tmp_var1}, &${node.leftNode.text}, 1, ${tmp_iter});`, fun_params);
                    loop_iterators.push(tmp_iter);
                    
                }
                
                
                block_level += 1;
                if (type != "int" && children != undefined) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`${type} ${node.leftNode.text} = ${children[0]} + ${tmp_iter}*${children[1]};`, fun_params);
                }
                for (let child of node.bodyNode.namedChildren) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(transformNode(file, child), fun_params);
                }
                block_level -= 1;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("\n}", fun_params)
                
                let idx = loop_iterators.indexOf(tmp_iter);
                /*if (node.rightNode.type == g.SyntaxType.Matrix || node.rightNode.type == g.SyntaxType.Identifier) {
                    idx = loop_iterators.indexOf(tmp_var2);
                }*/
                loop_iterators.splice(idx, 1);
                
                return null;
                break;
            }
    
            
            case g.SyntaxType.ExpressionStatement: {
                let expression = transformNode(file, node.firstChild);
                if (expression != null) {
                    if (![";","\n"].includes(expression.slice(-1))) {
                        return expression + ";";
                    } else {
                        return expression;
                    }
                } else {
                    return null;
                }
                break;
            }
            
            case g.SyntaxType.ParenthesizedExpression: {
                return "(" + transformNode(file, node.firstNamedChild) + ")";
                break;
            }
            
            // Assignment
            case g.SyntaxType.Assignment: {
                let [args1, outs, is_subscript] = parseNode(node, false);
                let arg_types = [];
                let args = [];
                var [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, node.rightNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                let [l_type, l_ndim, l_dim, l_ismatrix, l_ispointer, l_isstruct, , ] = inferType(filename, node.leftNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                
                if (type_to_matrix_type.findIndex(x=>x.type == l_type) != -1 && type_to_matrix_type.findIndex(x=>x.type == type) == -1) {
                    type = l_type;
                    ndim = l_ndim;
                    dim = l_dim;
                    ismatrix = l_ismatrix;
                    ispointer = l_ispointer;
                    isstruct = l_isstruct;
                }
                
                
                custom_functions = c;
                var init_flag = false;
                var lhs = null;
                if (node.rightNode.text == "[]" && node.leftNode.type == g.SyntaxType.CallOrSubscript) {
                    let [args2, , ] = parseNode(node.leftNode, false);
                    let num_to_remove = 0;
                    let expression = `removeValue(${transformNode(file, node.leftNode.valueNode)}, ${num_to_remove}`; 
                    for (let arg of args2) {
                        let [, child_ndim, child_dim, , , , ,] = inferType(filename, arg, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                        num_to_remove += Number(numel(child_dim));
                        expression.concat(`, ${transformNode(file, arg)}`);
                    }
                    expression.concat(")");
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(expression, fun_params);
                    return null;
                }
                if (node.rightNode.type == g.SyntaxType.Matrix || node.rightNode.type == g.SyntaxType.Cell) {
                    // https://www.mathworks.com/help/coder/ug/homogeneous-vs-heterogeneous-cell-arrays.html
                    
                    for (let arg of args1) {
                        lhs_flag = false;
                        args.push(transformNode(file, arg));
                        let [child_type, child_ndim, child_dim, child_ismatrix, child_ispointer, child_isstruct, c,] = inferType(filename, arg, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                        custom_functions = c;
                        arg_types.push({
                            type: child_type, 
                            ndim: child_ndim, 
                            dim: child_dim, 
                            ismatrix: child_ismatrix, 
                            isvector: numel(child_dim) > 1 && !child_ismatrix,
                            ispointer: child_ispointer,
                            isstruct: child_isstruct
                        });
                    }
                    
                    for (let i = 0; i < outs.length; i ++) {
                        lhs_flag = true;
                        outs[i] = transformNode(file, outs[i]);
                        lhs_flag = false;
                    }
                    
                    if (type == 'heterogeneous') {
                    numCellStruct += 1;
                    if (numCellStruct == 1) {
main_function.unshift(
`\t// Structure for cell arrays
\tstruct cell {
\t\tint type;
\t\tunion {
\t\t\tint ival;
\t\t\tdouble dval;
\t\t\tcomplex double cval;
\t\t\tchar chval[${MAXCHAR}];
\t\t} data;
\t};`);
                        }
                        let expression = [];
                        expression.push(`struct cell ${outs[0]}[${node.rightNode.namedChildCount}];`);
                        let types = [];
                        for (let i=0; i<node.rightNode.namedChildCount; i++) {
                            let child = node.rightNode.namedChildren[i];
                            let [child_type, child_ndim, child_dim, child_ismatrix, child_ispointer, child_isstruct, c,] = inferType(filename, child, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            custom_functions = c;
                            if (child.type == g.SyntaxType.Matrix) {
                                
                                //expression1.push(`Matrix f${i}[${numel}];`);
                                //expression2.push(initializeMatrix(node.rightNode, `${outs[0]}.f${i}`, child_ndim, child_dim, type));
          
                            } else if (child_type == 'char') {
                                expression.push(`${outs[0]}[${i}].type = 3;`);
                                //expression.push(`${outs[0]}[${i}].data.chval = ${child.text.replace(/'/g, '"')};`);
                                expression.push(`strcpy(${outs[0]}[${i}].data.chval, ${child.text.replace(/'/g, '"')});`);
                            } else {
                                let obj = type_to_cell_type.find(x => x.type === child_type);
                                if (obj == undefined) {
                                    console.error("ERROR IN HETEROGENEOUS CELL: CHILD TYPE UNKNOWN");
                                } else {
                                    expression.push(`${outs[0]}[${i}].type = ${obj.cell_type};`);
                                    expression.push(`${outs[0]}[${i}].data.${obj.cell_val} = ${child.text};`);
                                }
                            }
                            
                        }
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(expression.join("\n") + "\n", fun_params);
                    } else {
                        let obj = type_to_matrix_type.find(x => x.type === type);
                        if (obj != null) {
                            updateFunParams(0);
                            [main_function, function_definitions] = pushToMain(initializeMatrix(node.rightNode, outs[0]), fun_params);
                            
                        }
                        
                    }
                } else if (node.rightNode.type == g.SyntaxType.CallOrSubscript) {
                    for (let i = 0; i < outs.length; i ++) {
                        lhs_flag = true;
                        outs[i] = transformNode(file, outs[i]);
                        lhs_flag = false;
                    }
                    
                    let obj = classes.find(x => x.name === node.rightNode.valueNode.text);
                    // Is a class
                    if (obj != null) {
                        var rhs:string = obj.name;
                    } else {
                        let obj1 = custom_functions.find(x => x.name === node.rightNode.valueNode.text);
                       
                        //let obj2 = builtin_funs.find(x => x.fun_matlab === node.rightNode.valueNode.text);
                        let obj2 = findBuiltin(builtin_funs, node.rightNode.valueNode.text);
                        if (obj1 != null && obj1 != undefined) {
                            //lhs = obj1.outs_transform(args, arg_types, outs);
                            lhs = obj1.outs_transform(outs);
                        } else if (obj2 != null && obj2 != undefined) {
                            lhs = obj2.outs_transform(args, arg_types, outs);
                        }
                    }
                    //lhs_flag = false;
                    var rhs:string = transformNode(file, node.rightNode);
                    init_flag = true;
                } else {
                    for (let i = 0; i < outs.length; i ++) {
                        lhs_flag = true;
                        outs[i] = transformNode(file, outs[i]);
                        let [l_type, , , , , , , ] = inferTypeByName(outs[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                        if (l_type != null) {
                            if (l_type.includes("_type")) {
                                numUnknownType += 1;
                                if (numUnknownType == 1) {
                                    link.push(`#include "../unknownTypes.h"`);
                                }
                            }
                        }
                        lhs_flag = false;
                    }
                    
                    var rhs:string = transformNode(file, node.rightNode);
                    init_flag = true;
                    lhs = outs[0];
                }
                //if (node.leftNode.type != g.SyntaxType.CallOrSubscript && node.leftNode.type != g.SyntaxType.CellSubscript) {
                if (lhs == null && rhs != undefined && rhs != null) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`${rhs};`, fun_params);   
                } else if (init_flag && rhs != undefined && rhs != null) {
                    if (lhs[0].indexOf("[") > -1 || lhs.indexOf("[") > -1) {
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(`${lhs} = ${rhs};`, fun_params);
                    } else {
                        updateFunParams(0);
                        let [lhs2, var_type, flag1, flag2, fp] = isInitialized(lhs, node, type, fun_params);
                        lhs = lhs2;
                        fun_params = fp;
                        updateFunParams(1);
                        if (var_type != null && var_type != undefined) { 
                            if (flag1) {
                                updateFunParams(0);
                                if (lhs.includes("lhs_data")) {
                                    let alias_obj = alias_tbl.find(x => x.tmp_var == lhs);
                                    [main_function, function_definitions] = pushToMain(`${alias_obj.name} = ${rhs};`, fun_params);
                                } else {
                                    [main_function, function_definitions] = pushToMain(`${lhs} = ${rhs};`, fun_params);
                                }
                            } else if (flag2) {
                                let tmp = generateTmpVar(var_type.name, tmp_tbl);
                                tmp_var_types.push({
                                    name: tmp,
                                    type: type,
                                    ndim: ndim,
                                    dim: dim,
                                    ismatrix: ismatrix,
                                    isvector: numel(dim) > 1 && !ismatrix,
                                    ispointer: ispointer,
                                    isstruct: isstruct,
                                    initialized: true,
                                    scope: var_type.scope
                                });
                                updateFunParams(0);
                                //alias_tbl = pushAliasTbl(lhs, tmp, node, fun_params);
                                alias_tbl.push({
                                    name: lhs,
                                    tmp_var: tmp,
                                    scope: var_type.scope
                                });
                                
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(initVar(tmp, rhs, tmp_var_types[tmp_var_types.length - 1]), fun_params);
                                
                            } else {
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(initVar(lhs, rhs, var_type), fun_params);
                            }
                            
                            //tmp_var_types = tmp_var_types.filter(function(e) { return e.name !== var_type.name });
                            tmp_var_types = tmp_var_types.filter(function(e) { return JSON.stringify(e) !== JSON.stringify(var_type) });
                            var_type.initialized = true;
                            if (var_type.type == "unknown") {
                                var_type.type = type;
                            }
                            tmp_var_types.push(var_type);
                    
                                
                        } else {
                            let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                            tmp_var_types.push({
                                name: lhs,
                                type: type,
                                ndim: ndim,
                                dim: dim,
                                ismatrix: ismatrix,
                                isvector: numel(dim) > 1 && !ismatrix,
                                ispointer: ispointer,
                                isstruct: isstruct,
                                initialized: true,
                                scope: scope
                            });
                            updateFunParams(0);
                            [main_function, function_definitions] = pushToMain(initVar(lhs, rhs, tmp_var_types[tmp_var_types.length - 1]), fun_params);
                            
                            let obj = tmp_tbl.find(x => `${x.name}${x.count}` === rhs);
                            if (obj != null && obj != undefined) {
                                updateFunParams(0);
                                alias_tbl = pushAliasTbl(lhs, rhs, node, fun_params);
                            }
                        }
                    }
                }
                //}

                if (node.leftNode.type == g.SyntaxType.Matrix) {
                    for (let j = 0; j < node.leftNode.namedChildCount; j++) {
                        let child = node.leftNode.namedChildren[j];
                        // If element j of LHS matrix is a subscript
                        if (is_subscript[j]) {
                            
                            // Convert to linear idx
                            let obj4 = tmp_tbl.find(x => x.name == "d0_");
                            let idx = getSubscriptIdx(child, obj4.count);
                            let tmp_data = generateTmpVar("data", tmp_tbl);
                            let tmp_lhs = generateTmpVar("lhs_data", tmp_tbl);
                            let tmp_rhs = generateTmpVar("rhs_data", tmp_tbl);
                            let [ltype,,,,,,,] = inferType(filename, node.leftNode.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            updateFunParams(0);
                            [main_function, function_definitions] = pushToMain(`${type}* ${tmp_lhs} = ${type.charAt(0)}_to_${type.charAt(0)}(${transformNode(file, child.valueNode)});`, fun_params);
                            let [,,, ismatrix,,, c,] = inferType(filename, outs[j], tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            custom_functions = c;
                            
                            // If RHS is matrix
                            if (ismatrix) {
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(`${type}* ${tmp_rhs} = ${type.charAt(0)}_to_${type.charAt(0)}(${outs[j]});`, fun_params);
                                for (let i = 0; i < idx.length; i++) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[i]}] = ${tmp_rhs}[${i}];`, fun_params); 
                                }
                            
                            // If RHS not matrix
                            } else {
                                for (let i = 0; i < idx.length; i++) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${outs[j]}[${i}] = ${tmp_rhs}[${idx[i]}];`, fun_params); 
                                }
                            }
                            let tmp_size = generateTmpVar("size", tmp_tbl);
                            let tmp_iter = generateTmpVar("iter", tmp_tbl);
                            let tmp_mat = generateTmpVar("mat", tmp_tbl);
                            let obj1 = tmp_tbl.find(x => x.name === "ndim");
                            let tmp_ndim = `${obj1.name}${obj1.count}`;
                            let obj2 = tmp_tbl.find(x => x.name === "dim");
                            let tmp_dim = `${obj2.name}${obj2.count}`;
                            let obj3 = type_to_matrix_type.find(x => x.type === type);
                            updateFunParams(0);
[main_function, function_definitions] = pushToMain(`int ${tmp_size} = 1;
for (int ${tmp_iter} = 0 ; ${tmp_iter} < ${tmp_ndim}; ${tmp_iter}++)
{
\t${tmp_size} *= ${tmp_dim}[${tmp_iter}];
}
Matrix *${tmp_mat} = createM(${tmp_ndim}, ${tmp_dim}, ${obj3.matrix_type});
writeM(${tmp_mat}, ${tmp_size}, ${tmp_lhs});`, fun_params);
//printM(${tmp_mat});`); 
                            updateFunParams(0);
                            alias_tbl = pushAliasTbl(child.valueNode.text, tmp_mat, node, fun_params);
                            let obj = filterByScope(tmp_var_types, child.valueNode.text, child, 0);
                            
                            tmp_var_types.push({
                                name: tmp_mat,
                                type: type,
                                ndim: obj.ndim,
                                dim: obj.dim,
                                ismatrix: true,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: true,
                                scope: obj.scope
                            });
                        }
                    }
                } else {
                    // If LHS is a subscript
                    if (node.leftNode.type == g.SyntaxType.CellSubscript) {

                        /*let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                        if (loop_iterators.length > 0) {
                            scope = block_idxs.filter(function(e) { return e[2] == scope[2] - loop_iterators.length })
                            scope = scope[scope.length - 1];
                        }
                        updateFunParams(0);
                        alias_tbl = pushAliasTbl(node.leftNode.text, rhs, node, fun_params);
                        let obj = filterByScope(tmp_var_types, node.leftNode.valueNode.text, node, 0);
                        tmp_var_types.push({
                            name: rhs,
                            type: type,
                            ndim: obj.ndim,
                            dim: obj.dim,
                            ismatrix: true,
                            isvector: false,
                            ispointer: false,
                            isstruct: false,
                            initialized: true,
                            scope: obj.scope
                        });*/
                        
                    } else if (is_subscript[0]) {
                        // Convert to linear idx
                        let obj4 = tmp_var_types.filter(x => /ndim/.test(x.name) && x.propertyOf == node.leftNode.valueNode.text && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]);
                        let idx = ["UNKNOWN"];

                        if (obj4.length > 0) {
                            idx = getSubscriptIdx(node.leftNode, obj4[obj4.length - 1].name.match(/\d+/)[0]);
                        }
                        
                        let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                        if (loop_iterators.length > 0) {
                            scope = block_idxs.filter(function(e) { return e[2] == scope[2] - loop_iterators.length })
                            scope = scope[scope.length - 1];
                        }
                        let obj6 = tmp_tbl.find(x => x.name == "lhs_data");
                        let new_flag = true;
                        let tmp_lhs = "lhs_data";
                        if (obj6 != null && obj6 != undefined) {
                            //let obj5 = tmp_var_types.find(x => x.name == `lhs_data${obj6.count}`);
                            let obj5 = filterByScope(tmp_var_types, `lhs_data${obj6.count}`, node, 0);
                            if (obj5 != null && obj5 != undefined) {
                                if (obj5.loop == loop_iterators.join("")) {
                                    new_flag = false;
                                    tmp_lhs = obj5.name;
                                }
                            }
                        }
                        let obj7 = filterByScope(alias_tbl, node.leftNode.valueNode.text, node, 0);
                        if (obj7 != null && obj7 != undefined) {
                            if (obj7.tmp_var.includes("lhs_data")) {
                                new_flag = false;
                                tmp_lhs = obj7.tmp_var;
                            }
                        }
                        if (new_flag) {
                            let tmp_data = generateTmpVar("data", tmp_tbl);
                            tmp_lhs = generateTmpVar("lhs_data", tmp_tbl);
                            let tmp_rhs = generateTmpVar("rhs_data", tmp_tbl);
                            let [,,, ismatrix,,, c,] = inferType(filename, outs[0], tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            custom_functions = c;
                            let obj8 = filterByScope(tmp_var_types, `${node.leftNode.valueNode.text}_init`, node, 0);
                            let [ltype,,,,,,,] = inferType(filename, node.leftNode.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            if (ltype == undefined) {
                                ltype = "UNKNOWN";
                            }
                            if (obj8 != null && obj8 != undefined) {
                                [ltype,,,,,,,,] = inferTypeByName(`${node.leftNode.valueNode.text}_init`, node, tmp_var_types, custom_functions, alias_tbl, debug);
                            }

                            let tmp_block_level = block_level;
                            let transformed_lhs_valueNode = transformNode(file, node.leftNode.valueNode);
                            updateFunParams(0);
                            [main_function, function_definitions, tmp_block_level] = insertMain(`${type}* ${tmp_lhs} = ${ltype.charAt(0)}_to_${type.charAt(0)}(${transformed_lhs_valueNode});`, 
                                `${transformed_lhs_valueNode} =`, 1, fun_params);
                            // If RHS is matrix
                            if (ismatrix) {
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(`${type}* ${tmp_rhs} = ${type.charAt(0)}_to_${type.charAt(0)}(${outs[0]});`, fun_params);
                                for (let i = 0; i < idx.length; i++) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[i]}] = ${tmp_rhs}[${i}];`, fun_params);
                                }
                            
                            // If RHS not matrix
                            } else {
                                if (idx.length == 1) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[0]}] = ${lhs};`, fun_params);
                                } else {
                                    for (let i = 0; i < idx.length; i++) {
                                        updateFunParams(0);
                                        [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[i]}] = ${lhs}[${i}];`, fun_params);
                                    }
                                }
                            }

                            let tmp_size = generateTmpVar("size", tmp_tbl);
                            let tmp_iter = generateTmpVar("iter", tmp_tbl);
                            let tmp_mat = generateTmpVar("mat", tmp_tbl);
                            let obj1 = tmp_tbl.find(x => x.name === "ndim");
                            let tmp_ndim = `${obj1.name}${obj1.count}`;
                            let obj2 = tmp_tbl.find(x => x.name === "dim");
                            let tmp_dim = `${obj2.name}${obj2.count}`;
                            let obj3 = type_to_matrix_type.find(x => x.type === type);
                            if (obj3 == undefined) {
                                console.error("ERROR IN ASSIGNMENT: OBJ3 UNDEFINED");
                            }

                            let re = new RegExp(`${node.leftNode.valueNode.text}\\([\\s\\w+\\-\\*]*\\)(=| =)`);
                            updateFunParams(0);
                            let lastSubscript = findLastSubscript(node.leftNode.valueNode, fun_params);
                            let condition = `(block_level == ${tmp_block_level});`;
                            let lhs_scope = findVarScope(filename, node, block_idxs, current_code, debug);
                            let lhs_scope2 = JSON.parse(JSON.stringify(lhs_scope));

                            if (lastSubscript[0] != null) {
                                lhs_scope2[1] = lastSubscript[1];
                                alias_tbl.push({
                                    name: node.leftNode.valueNode.text,
                                    tmp_var: tmp_lhs,
                                    scope: lhs_scope2
                                });
                            condition = `
function myfun(block_level, node) {
    if ((block_level == ${tmp_block_level}) && node.previousNamedSibling !== null) {
        if (node.previousNamedSibling.text.includes("${lastSubscript[0]}")) {
            return true;
        }
    }
    return false;
}
myfun(block_level, node);`;
                            }
                            
                            if (obj3 != undefined) {
                            let mq: MainQueue = {
                                expression: `// Write matrix ${tmp_mat}
int ${tmp_size} = 1;
for (int ${tmp_iter} = 0 ; ${tmp_iter} < ${tmp_ndim}; ${tmp_iter}++)
{
\t${tmp_size} *= ${tmp_dim}[${tmp_iter}];
}
Matrix *${tmp_mat} = createM(${tmp_ndim}, ${tmp_dim}, ${obj3.matrix_type});
writeM(${tmp_mat}, ${tmp_size}, ${tmp_lhs});`,
                                condition: condition
                            };
                            main_queue.push(mq);
                            }
                            updateFunParams(0);
                            //alias_tbl = pushAliasTbl(node.leftNode.valueNode.text, tmp_mat, node, fun_params);
                            let obj = filterByScope(tmp_var_types, node.leftNode.valueNode.text, node, 0);
                            
                            if (obj == null) {
                                console.error("ERROR IN ASSIGNMENT: OBJ IS NULL");
                            } else {
                                if (node.leftNode.valueNode.type != g.SyntaxType.CellSubscript) {
                                    alias_tbl.push({
                                        name: node.leftNode.valueNode.text,
                                        tmp_var: tmp_mat,
                                        scope: [lhs_scope2[1] + 1, obj.scope[1], obj.scope[2]]//scope
                                    });
                                }
                                tmp_var_types.push({
                                    name: tmp_mat,
                                    type: type,
                                    ndim: obj.ndim,
                                    dim: obj.dim,
                                    ismatrix: true,
                                    isvector: false,
                                    ispointer: false,
                                    isstruct: false,
                                    initialized: true,
                                    scope: [lhs_scope2[1] + 1, obj.scope[1], obj.scope[2]]//scope
                                });
                                tmp_var_types.push({
                                    name: tmp_lhs,
                                    //type: loop_iterators.join(""),
                                    type: ltype,
                                    ndim: 1,
                                    dim: [numel(obj.dim)],
                                    ismatrix: false,
                                    isvector: true,
                                    ispointer: false,
                                    isstruct: false,
                                    initialized: true,
                                    scope: lhs_scope2,
                                    loop: loop_iterators.join("")
                                });
                                if (node.leftNode.valueNode.type == g.SyntaxType.CellSubscript) {
                                    main_queue.push({
                                        expression: `${transformed_lhs_valueNode} = ${tmp_mat};`,
                                        condition: condition
                                    });
                                }
                            }
                            
                            
                            
                        } else {
                            if (idx.length == 1) {
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[0]}] = ${lhs};`, fun_params);
                            } else {
                                for (let i = 0; i < idx.length; i++) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${tmp_lhs}[${idx[i]}] = ${lhs}[${i}];`, fun_params);
                                }
                            }
                        }
                    }
                }
                return null; 
                break;
            }
            
            // Transform matrix operations
            case g.SyntaxType.BinaryOperator:
            case g.SyntaxType.BooleanOperator:
            case g.SyntaxType.ComparisonOperator:
            case g.SyntaxType.TransposeOperator:
            case g.SyntaxType.UnaryOperator: {
                return printMatrixFunctions(node);
                break;
            }
            

            case g.SyntaxType.Block: {
                let expression = [];
                for (let child of node.namedChildren) {
                    expression.push(transformNode(file, child));
                }
                return expression.join("\n");
                break;
            }
            
            case g.SyntaxType.CellSubscript: {
                if (node.valueNode.text == "varargin") {
                    let idx = node.namedChildren[1].text.match(/\d+/)[0];
                    if (fun_obj.var_arg_types[Number(idx) - 1].ismatrix) {
                        return "va_arg(varargin, Matrix *)";
                    } else {
                        return `va_arg(varargin, ${fun_obj.var_arg_types[Number(idx) - 1].type})`;
                    }
                }
                
                let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, node.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                if (dim == null) {
                    console.error("ERROR IN CELL SUBSCRIPT: DIM IS NULL");
                    return null;
                }
                // only use indexM if subscript is on rhs
                let count = '';
                let obj = tmp_var_types.find(x => /ndim/.test(x.name) && x.propertyOf == node.valueNode.text && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]);
                if (obj == null || obj == undefined) {
                    //let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, node.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    let tmp_ndim = generateTmpVar("ndim", tmp_tbl);
                    let tmp_dim = generateTmpVar("dim", tmp_tbl);
                    count = tmp_ndim.match(/\d+/)[0];
                    let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                    let expression = [];
                    //expression.push(`int ${tmp_ndim} = getnDimM(*${transformNode(file, node.valueNode)});`);
                    expression.push(`int ${tmp_ndim} = ${ndim};`);
                    tmp_var_types.push({
                        name: tmp_ndim,
                        type: 'int',
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: scope,
                        propertyOf: node.valueNode.text
                    });
                    //expression.push(`int *${tmp_dim} = getDimsM(*${transformNode(file, node.valueNode)});`);
                    expression.push(`int ${tmp_dim}[${ndim}] = {${dim}};`);
                    tmp_var_types.push({
                        name: tmp_dim,
                        type: 'int',
                        ndim: dim.length,
                        dim: [dim.length],
                        ismatrix: false,
                        isvector: true,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: scope,
                        propertyOf: node.valueNode.text
                    });
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                } else {
                    count = obj.name.match(/\d+/)[0];
                }
                //let index = getSubscriptIdx(node, count);
                if (type == "heterogeneous") {
                    let index = getSubscriptIdx(node, count);
                    let tmp_var = generateTmpVar("tmp", tmp_tbl);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`struct cell ${tmp_var} = ${node.valueNode.text}[${index[0]}];`, fun_params);
                    tmp_var_types.push({
                        name: tmp_var,
                        type: 'dynamic',
                        ndim: 1,
                        dim: [1],
                        ismatrix: false,
                        isvector: false,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: findVarScope(filename, node, block_idxs, current_code, debug)
                    });
                    alias_tbl = pushAliasTbl(node.text, tmp_var, node, fun_params);
                    return tmp_var;
                }
                        
                //if (!lhs_flag) {
                let obj2 = filterByScope(alias_tbl, node.text, node, 0);
                if (obj2 == null || obj2 == undefined) {
                    let index = getSubscriptIdx(node, count);
                    alias_tbl = pushAliasTbl(node.text, `${transformNode(file, node.valueNode)}[${index[0]}]`, node, fun_params);
                    return `${transformNode(file, node.valueNode)}[${index[0]}]`;
                } else {
                    tmp_var = obj2.tmp_var;
                    return tmp_var;
                }
                //}
                
                //return `${node.valueNode.text}[${index[0]}]`;
                break;
            }
                
            case g.SyntaxType.CallOrSubscript: {
                let [args1, outs, is_subscript] = parseNode(node, false);
                let arg_types = [];
                let args = [];
                //for (let arg of args1) {
                for (let i = 0; i < args1.length; i ++) {
                    let arg = args1[i];
                    if (lhs_flag) {
                        args.push(arg.text);
                    } else if ((arg.text.includes("hamming") || arg.text.includes("hanning")) && node.valueNode.text == "stft") {
                        args.push(`${arg.namedChildren[0].text}(${transformNode(file, arg.namedChildren[1])})`);
                    } else {
                        args.push(transformNode(file, arg));
                    }
                    let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, arg, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    
                    if (/tmp.*\[0\]/.test(args[i])) {
                        [type, ndim, dim, ismatrix, , ispointer, isstruct, c,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                    }
                    
                    let isvector = numel(dim) > 1 && !ismatrix;
                    
                    // If a tmp var, infer type by name
                    if (args[i] != null) {
                        let match = args[i].match(/^([a-zA-Z_]+)(\d+)$/);
                        if (match != null) {
                            let tmpvar_obj = tmp_tbl.find(x => x.name == match[1] && Number(x.count) >= Number(match[2]));
                            if (tmpvar_obj != null && tmpvar_obj != undefined) {
                                let [type2, ndim2, dim2, ismatrix2, isvector2, ispointer2, isstruct2, c2,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                                if (type2 != null) {
                                    type = type2;
                                    ndim = ndim2;
                                    dim = dim2;
                                    ismatrix = ismatrix2;
                                    isvector = isvector2;
                                    ispointer = ispointer2;
                                    isstruct = isstruct2;
                                    c = c2;
                                }
                                //[type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                            }
                        }
                    }
                    
                    /*if (arg.type != g.SyntaxType.CellSubscript && ismatrix) { // if a matrix, could actually be a vector so check var name to see if initialized as vector
                        [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                    }*/
                    custom_functions = c;
                    arg_types.push({
                        type: type, 
                        ndim: ndim, 
                        dim: dim, 
                        ismatrix: ismatrix, 
                        isvector: isvector, //numel(dim) > 1 && !ismatrix,
                        ispointer: ispointer,
                        isstruct: isstruct
                    });
                }
                if (node.valueNode.text == "sprintf") {
                    if (node.parent.type == g.SyntaxType.CallOrSubscript) {
                        if (node.parent.valueNode.text == "disp") {
                            args.unshift('disp');
                        }
                    }
                }
                
                for (let i = 0; i < outs.length; i ++) {
                    //outs[i] = transformNode(file, outs[i]);
                    outs[i] = outs[i].text;
                }
                let class_obj = classes.find(x => x.name === node.valueNode.text);
                // Is a class (treat as structure)
                if (class_obj != null) {
                    if (!printed_classes.includes(class_obj.name)) {
                        link.push(`#include "./${class_obj.name}.h"`);
                        printClass(class_obj);
                    }
                    return null;
                }
                
                class_obj = classes.find(x => arg_types.some(y => y.type == x.name));
                let obj = null;
                
                let method_flag = false;
                if (class_obj != null && class_obj != undefined) {
                    obj = class_obj.methods.find(x => x.name === node.valueNode.text);
                    method_flag = true;
                } else {
                    // Is a custom function call
                    obj = custom_functions.find(x => x.name === node.valueNode.text);
                }
                
                
                if (obj != null && obj != undefined) {
                    // Is a custom function
                    if (obj.arg_types.find(x => x.name == "varargin") != null) {
                        let varargin_idx = obj.arg_types.findIndex(x => x.name == "varargin");
                        args.splice(varargin_idx, 0, `${args.length - 1}`);
                    }
                    
                    let ptr_args = obj.ptr_args(arg_types, outs);
                    if (ptr_args != null) {
                        let ptr_declaration = [];
                        for (let i = 0; i < ptr_args.length; i++) {
                            let tmp_ptr = generateTmpVar(ptr_args[i].name, tmp_tbl);
                            args.push(`&${tmp_ptr}`);
                            ptr_args[i].ispointer = false;
                            ptr_declaration.push(initVar(tmp_ptr, null, ptr_args[i]));
                            updateFunParams(0);
                            alias_tbl = pushAliasTbl(ptr_args[i].name, tmp_ptr, node, fun_params);
                            tmp_var_types.push({
                                name: tmp_ptr,
                                type: ptr_args[i].type,
                                ndim: ptr_args[i].ndim,
                                dim: ptr_args[i].dim,
                                ismatrix: ptr_args[i].ismatrix,
                                isvector: numel(ptr_args[i].dim) > 1 && !ptr_args[i].ismatrix,
                                ispointer: false, //ptr_args[i].ispointer,
                                isstruct: ptr_args[i].isstruct,
                                initialized: true,
                                scope: findVarScope(filename, node, block_idxs, current_code, debug)
                            });
                            
                        }
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(ptr_declaration.join("\n"), fun_params);
                        
                    }
                    
                    if (!method_flag && path.parse(obj.file).name !== path.parse(file).name && !link.includes(`#include "./${path.parse(obj.file).name}.h"`)) {
                        //link.push(`#include <${path.parse(obj.file).name}.h>`);
                        link.push(`#include "./${path.parse(obj.file).name}.h"`);
                    }
                    
                    if (obj.arg_type_dic.length == 1) {
                        return `${obj.name}(${args.join(", ")})`;
                    } else {
                        let typestr = '';
                        for  (let i = 0; i < arg_types.length; i++) { 
                            typestr = typestr.concat(arg_types[i].type);
                        }
                        return `${obj.name}_${typestr}(${args.join(", ")})`;
                    }
                    
                    
                    
                } else {
                    // Is a builtin function call
                    let obj = findBuiltin(builtin_funs, node.valueNode.text);
                    if (obj != null && obj != undefined) {
                        let req_arg_types = obj.req_arg_types(args, arg_types, outs);
                        let init_before = obj.init_before(args, arg_types, outs);
                        let push_before = obj.push_main_before(args, arg_types, outs);
                        let push_after = obj.push_main_after(args, arg_types, outs);
                        let return_type = obj.return_type(args, arg_types, outs);
                        let fun_c = obj.fun_c(args, arg_types, outs, node.valueNode.text);
                        let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                        let tmp_out_transform = obj.tmp_out_transform(args, arg_types, outs);
                        let push_alias_tbl = obj.push_alias_tbl(args, arg_types, outs);
                        args = obj.args_transform(args, arg_types, outs);
                        
                        if (req_arg_types != null) {
                            for (let i = 0; i < Math.min(req_arg_types.length, arg_types.length); i ++) {
                                if (!req_arg_types[i].ismatrix && arg_types[i].ismatrix) {
                                    if (arg_types[i].dim.every(x => x === 1)) {
                                        let expression = "";
                                        updateFunParams(0);
                                        [expression, fun_params] = extractSingularMat(args[i], arg_types[i], node, fun_params);
                                        updateFunParams(1);
                                        args[i] = expression;
                                    }
                                }
                            }
                        }
                        if (fun_c !== null) {
                            fun_c = fun_c.replace('fun_matlab', node.valueNode.text);
                        }
                        if (init_before != null && init_before != undefined) {
                            for (let i = 0; i < init_before.length; i++) {
                                if (init_before[i].name == "complex_one") {
                                    let obj2 = tmp_var_types.find(x => x.name === init_before[i].name);
                                    if (obj2 == null || obj2 == undefined) {
                                        updateFunParams(0);
                                        [main_function, function_definitions] = pushToMain(initVar(init_before[i].name, init_before[i].val, init_before[i]), fun_params);
                                        
                                        tmp_var_types.push({
                                            name: init_before[i].name,
                                            type: init_before[i].type,
                                            ndim: init_before[i].ndim,
                                            dim: init_before[i].dim,
                                            ismatrix: init_before[i].ismatrix,
                                            isvector: numel(init_before[i].dim) > 1 && !init_before[i].ismatrix,
                                            ispointer: init_before[i].ispointer,
                                            isstruct: init_before[i].isstruct,
                                            initialized: true,
                                            scope: scope
                                        });
                                    }
                                } else {
                                    let tmp_var = generateTmpVar(init_before[i].name, tmp_tbl);
                                    updateFunParams(0);
                                    alias_tbl = pushAliasTbl(init_before[i].name, tmp_var, node, fun_params);

                                    args[args.indexOf(init_before[i].name)] = tmp_var;
                                    args[args.indexOf("&" + init_before[i].name)] = "&" + tmp_var;
                                    
                                    let re = new RegExp(`\\b${init_before[i].name}\\b`, 'g');
                                    if (push_before != null) {
                                        push_before = push_before.replace(re, tmp_var);
                                    }
                                    if (push_after != null) {
                                        push_after = push_after.replace(re, tmp_var);
                                    }
                                    for (let j = i + 1; j < init_before.length; j++) {
                                        init_before[j].val = `${init_before[j].val}`.replace(re, tmp_var);
                                    }
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(initVar(tmp_var, init_before[i].val, init_before[i]), fun_params);
                                    
                                    if (outs.length > 0 && (init_before[i].name == "ndim" || init_before[i].name == "dim")) {
                                        tmp_var_types.push({
                                            name: tmp_var,
                                            type: init_before[i].type,
                                            ndim: init_before[i].ndim,
                                            dim: init_before[i].dim,
                                            ismatrix: init_before[i].ismatrix,
                                            isvector: numel(init_before[i].dim) > 1 && !init_before[i].ismatrix,
                                            ispointer: init_before[i].ispointer,
                                            isstruct: init_before[i].isstruct,
                                            initialized: true,
                                            scope: scope,
                                            propertyOf: outs[0]
                                        });
                                    } else {
                                        tmp_var_types.push({
                                            name: tmp_var,
                                            type: init_before[i].type,
                                            ndim: init_before[i].ndim,
                                            dim: init_before[i].dim,
                                            ismatrix: init_before[i].ismatrix,
                                            isvector: numel(init_before[i].dim) > 1 && !init_before[i].ismatrix,
                                            ispointer: init_before[i].ispointer,
                                            isstruct: init_before[i].isstruct,
                                            initialized: true,
                                            scope: scope
                                        });
                                    }
                                }
                            }
                        }

                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(push_before, fun_params);
                        let n_args = node.namedChildCount - 1;
                        if (n_args < obj.n_req_args && obj.opt_arg_defaults != null) {
                            args = args.concat(obj.opt_arg_defaults.slice(0, obj.n_req_args - n_args));
                        }
                        let ptr_args = obj.ptr_args(arg_types, outs);
                        if (ptr_args != null) {
                            let ptr_declaration = [];
                            let tmp_ptr = "tmp_ptr";
                            for (let i = 0; i < ptr_args.length; i++) {
                                tmp_ptr = generateTmpVar(ptr_args[i].name, tmp_tbl);
                                if (push_alias_tbl != null) {
                                    let idx = push_alias_tbl.map(function(e) { return e.tmp_var; }).indexOf(ptr_args[i].name);
                                    if (idx > -1) {
                                        push_alias_tbl[idx].tmp_var = tmp_ptr;
                                        let re = new RegExp(`${ptr_args[i].name}\\d+`);
                                        alias_tbl = alias_tbl.filter(x => !re.test(x.tmp_var));
                                    }
                                }
                                args.push(`&${tmp_ptr}`);
                                ptr_args[i].ispointer = false;
                                ptr_declaration.push(initVar(tmp_ptr, null, ptr_args[i]));
                                
                                let re = new RegExp(`\\b${ptr_args[i].name}\\b`, 'g');
                                if (push_after != null) {
                                    push_after = push_after.replace(re, tmp_ptr);
                                }
                                tmp_var_types.push({
                                    name: tmp_ptr,
                                    type: ptr_args[i].type,
                                    ndim: ptr_args[i].ndim,
                                    dim: ptr_args[i].dim,
                                    ismatrix: ptr_args[i].ismatrix,
                                    isvector: ptr_args[i].isvector,
                                    ispointer: false, //ptr_args[i].ispointer,
                                    isstruct: ptr_args[i].isstruct,
                                    initialized: true,
                                    scope: scope
                                });
                                updateFunParams(0);
                                alias_tbl = pushAliasTbl(ptr_args[i].name, tmp_ptr, node, fun_params);
                                
                            }
                            updateFunParams(0);
                            [main_function, function_definitions] = pushToMain(ptr_declaration.join("\n"), fun_params);
                            
                            if (push_alias_tbl != null) {
                                for (let i = 0; i < push_alias_tbl.length; i ++) {
                                    if (push_alias_tbl[i].scope == null) {
                                        push_alias_tbl[i].scope = findVarScope(filename, node, block_idxs, current_code, debug);
                                    }
                                    
                                    alias_tbl.push(push_alias_tbl[i]);
                                }
                            }
                            // ptr args were originally outputs in MATLAB code
                            // in the case of 1 ptr arg:
                            // disp(det(A)) -> determinantM(A, &d); fprint("%i", d) 
                            if ((ptr_args.length == 1) && (return_type == null)) {
                                alias_tbl.push({
                                    name: node.text,//`${fun_c}(${args.join(", ")})`,
                                    tmp_var: tmp_ptr,
                                    scope: findVarScope(filename, node, block_idxs, current_code, debug)
                                })
                                if (fun_c != null) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${fun_c}(${args.join(", ")});`, fun_params);
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(push_after, fun_params);
                                }
                                return tmp_ptr;
                            }
                        
                            
                        }
                        if (fun_c == null) {
                            return null;
                        } else {
                            if (return_type == null) {
                                if (args == null) {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${fun_c};`, fun_params);
                                } else if (args[0] == 'void') {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${fun_c}();`, fun_params);
                                } else {
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(`${fun_c}(${args.join(", ")});`, fun_params);
                                }
                                updateFunParams(0);
                                [main_function, function_definitions] = pushToMain(push_after, fun_params);
                                return null;
                                
                            } else {
                                let var_val = `${fun_c}`;
                                if (args != null) {
                                    var_val = `${fun_c}(${args.join(", ")})`;
                                }
                                
                                //if (push_after != null || node.parent.type == g.SyntaxType.CallOrSubscript || tmp_out_transform != null) {
                                    let tmp_var = generateTmpVar("tmp", tmp_tbl);
                                    if (init_before != null) {
                                        var tmp_out_obj = init_before.find(x => x.name == 'tmp_out');
                                    }
                                    if (tmp_out_obj != null && tmp_out_obj != undefined) {
                                        let tmp_out_obj2 = tmp_tbl.find(x => x.name == 'tmp_out');
                                        updateFunParams(0);
                                        [main_function, function_definitions] = replaceMain(tmp_var, `${tmp_out_obj2.name}${tmp_out_obj2.count}`, 1, fun_params);
                                        updateFunParams(0);
                                        [main_function, function_definitions] = pushToMain(`${tmp_var} = ${var_val};`, fun_params);
                                        tmp_var_types.push({
                                            name: tmp_var,
                                            type: tmp_out_obj.type,
                                            ndim: tmp_out_obj.ndim,
                                            dim: tmp_out_obj.dim,
                                            ismatrix: tmp_out_obj.ismatrix,
                                            isvector: tmp_out_obj.isvector,
                                            ispointer: tmp_out_obj.ispointer,
                                            isstruct: false,
                                            initialized: true,
                                            scope: findVarScope(filename, node, block_idxs, current_code, debug)
                                        });
                                    }
                                    if (fun_c == "getDimsM") {
                                        tmp_var = generateTmpVar("dim", tmp_tbl);
                                    }
                                    if (tmp_out_obj == null || tmp_out_obj == undefined) {
                                        updateFunParams(0);
                                        [main_function, function_definitions] = pushToMain(initVar(tmp_var, var_val, return_type), fun_params);
                                    }
                                    updateFunParams(0);
                                    [main_function, function_definitions] = pushToMain(push_after, fun_params);
                                    //alias_tbl = pushAliasTbl(node.text, tmp_var, node, fun_params);
                                    if (fun_c == "getDimsM") {
                                        tmp_var_types.push({
                                            name: tmp_var,
                                            type: return_type.type,
                                            ndim: return_type.ndim,
                                            dim: return_type.dim,
                                            ismatrix: return_type.ismatrix,
                                            isvector: return_type.isvector,
                                            ispointer: return_type.ispointer,
                                            isstruct: false,
                                            initialized: true,
                                            scope: findVarScope(filename, node, block_idxs, current_code, debug),
                                            propertyOf: args[0]
                                        });
                                    } else if (tmp_out_obj == null || tmp_out_obj == undefined) {
                                        tmp_var_types.push({
                                            name: tmp_var,
                                            type: return_type.type,
                                            ndim: return_type.ndim,
                                            dim: return_type.dim,
                                            ismatrix: return_type.ismatrix,
                                            isvector: return_type.isvector,
                                            ispointer: return_type.ispointer,
                                            isstruct: false,
                                            initialized: true,
                                            scope: findVarScope(filename, node, block_idxs, current_code, debug)
                                        });
                                    }
                                    
                                    if (tmp_out_transform != null) {
                                        tmp_out_transform = tmp_out_transform.replace('tmp_out', tmp_var);
                                        return tmp_out_transform;
                                    }
                                    
                                    let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                                    alias_tbl.push({
                                        name: node.text,
                                        tmp_var: tmp_var,
                                        scope: [node.endIndex+1, scope[1]]
                                    });
                                    
                                    return tmp_var;
                                /*} else {
                                    return var_val;
                                }*/
                            }
                        }
                        
                        
                        
                    // Is a subscript
                    
                    } else {
                        let [type, ndim, dim, ismatrix, isvector, ispointer, isstruct, c,] = inferTypeByName(node.valueNode.text, node, tmp_var_types, custom_functions, alias_tbl, debug);
                        if (isvector) {
                            return `${transformNode(file, node.valueNode)}[${transformNode(file, node.namedChildren[1])} - 1]`;
                        }
                        var tmp_var = generateTmpVar("tmp", tmp_tbl);
                        // only use indexM if subscript is on rhs
                        
                        let index = [];
                        let tmp_var1 = generateTmpVar("tmp_", tmp_tbl);
                        
                        let count = '';
                        let obj = tmp_var_types.find(x => /ndim/.test(x.name) && x.propertyOf == node.valueNode.text && node.startIndex >= x.scope[0] && node.endIndex <= x.scope[1]);

                        if (obj == null || obj == undefined) {
                            let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, node.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            let tmp_ndim = generateTmpVar("ndim", tmp_tbl);
                            let tmp_dim = generateTmpVar("dim", tmp_tbl);
                            count = tmp_ndim.match(/\d+/)[0];
                            let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                            let expression = [];
                            expression.push(`int ${tmp_ndim} = getnDimM(${transformNode(file, node.valueNode)});`);
                            tmp_var_types.push({
                                name: tmp_ndim,
                                type: 'int',
                                ndim: 1,
                                dim: [1],
                                ismatrix: false,
                                isvector: false,
                                ispointer: false,
                                isstruct: false,
                                initialized: true,
                                scope: scope,
                                propertyOf: node.valueNode.text
                            });
                            expression.push(`int *${tmp_dim} = getDimsM(${transformNode(file, node.valueNode)});`);
                            tmp_var_types.push({
                                name: tmp_dim,
                                type: 'int',
                                ndim: dim.length,
                                dim: [dim.length],
                                ismatrix: false,
                                isvector: true,
                                ispointer: false,
                                isstruct: false,
                                initialized: true,
                                scope: scope,
                                propertyOf: node.valueNode.text
                            });
                            updateFunParams(0);
                            [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                        
                        } else {
                            count = obj.name.match(/\d+/)[0];
                        }
                        let flat_idx = getSubscriptIdx(node, count);
                        
                        if (node.namedChildCount == 2) {
                            //let obj2 = tmp_var_types.find(x => x.name === node.valueNode.text);
                            //let dim = obj2.dim;
                            //index = rowMajorFlatIdx(obj3.count, dim, node.namedChildren[1].text, lhs_flag);
                            index = flat_idx;
                        } else {
                            for (let i=1; i<node.namedChildCount; i++) {
                                index.push(transformNode(file, node.namedChildren[i]));
                            }
                        }
                        if (!lhs_flag) { // subscript is on rhs
                            //let obj = alias_tbl.find(x => x.name === node.text);
                            let obj = filterByScope(alias_tbl, node.text, node, 0);
                            let [type, , , , , , ,] = inferType(filename, node.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                            if (obj == null || obj == undefined) {
                                let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                                updateFunParams(0);
                                alias_tbl = pushAliasTbl(node.text, tmp_var, node, fun_params);
                                tmp_var_types.push({
                                    name: tmp_var,
                                    type: type,
                                    ndim: flat_idx.length,
                                    dim: [flat_idx.length],
                                    ismatrix: flat_idx.length > 1,
                                    isvector: false,
                                    ispointer: false,
                                    isstruct: false,
                                    initialized: true,
                                    scope: scope
                                });
                                
                                
                                updateFunParams(0);
                                //[main_function, function_definitions] = pushToMain(`${type} ${tmp_var};`, fun_params);
                                [main_function, function_definitions] = pushToMain(initVar(tmp_var, null, tmp_var_types[tmp_var_types.length-1]), fun_params);
                                if (index.length == 1) {
                                    let isnum = /^\d+$/.test(index[0]);
                                    if (isnum) {
                                        index[0] = `${Number(index[0]) + 1}`;
                                    } else {
                                        //index[0] = index[0].replace(/-1/, '');
                                        index[0] = index[0].concat("+1");
                                    }
                                    //index = index[0].concat("+1");
                                }
                                updateFunParams(0);
                                let tmp = transformNode(file, node.valueNode);
                                if (tmp.includes("lhs_data")) {
                                    let obj4 = alias_tbl.find(x => x.tmp_var == tmp);
                                    [main_function, function_definitions] = pushToMain(`indexM(${obj4.name}, &${tmp_var}, ${index.length}, ${index.join(", ")});`, fun_params);
                                } else {
                                    if (type.includes("_type")) {
                                        [main_function, function_definitions] = pushToMain(`unknown_indexM(&${tmp_var}, ${type}, ${tmp}, 1, ${flat_idx[0]});`, fun_params);
                                    } else {
                                        [main_function, function_definitions] = pushToMain(`indexM(${tmp}, &${tmp_var}, ${index.length}, ${index.join(", ")});`, fun_params);
                                    }
                                }
                                
                                /*let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                                updateFunParams(0);
                                alias_tbl = pushAliasTbl(node.text, tmp_var, node, fun_params);
                                tmp_var_types.push({
                                    name: tmp_var,
                                    type: type,
                                    ndim: flat_idx.length,
                                    dim: [flat_idx.length],
                                    ismatrix: flat_idx.length > 1,
                                    isvector: false,
                                    ispointer: false,
                                    isstruct: false,
                                    initialized: true,
                                    scope: scope
                                });*/
                            } else {
                                tmp_var = obj.tmp_var;
                            }
                        }
                        return tmp_var;
                    }
                }    
                break;
            }
            
            case g.SyntaxType.ElseifClause: {
                let expression = [];
                updateFunParams(0);
                if_flag = true;
                [main_function, function_definitions] = pushToMain("} else if (" + transformNode(file, node.conditionNode) + ") {", fun_params);
                if_flag = false;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(transformNode(file, node.consequenceNode), fun_params);
                //pushToMain("\n}")
                return null;
                break;
            } 
            case g.SyntaxType.ElseClause: {
                let expression = [];
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("} else {", fun_params);
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(transformNode(file, node.bodyNode), fun_params);
                /*for (let i = 0; i < node.bodyNode.namedChildCount; i ++) {
                    pushToMain(transformNode(file, node.bodyNode.namedChildren[i]));
                }*/
                //pushToMain("\n}")
                return null;
                break;
            }
            
            // TO DO: FIX STUFF WITH SCOPE IN ADDITION
            case g.SyntaxType.Attribute:
            case g.SyntaxType.Identifier: {
                if (node.text == "nargin") {
                    return `(n_varargin + ${n_args})`;
                }
                // if identifier is on lhs return itself
                if (node.parent.type == g.SyntaxType.Matrix) {
                    if (node.parent.parent.type == g.SyntaxType.Assignment) {
                        let re = new RegExp(`\\b${node.text}\\b`);
                        if (re.test(node.parent.text)) {
                            return node.text;
                        }
                    }
                }   

                if (node.parent.type == g.SyntaxType.Assignment) {
                    if (node.parent.leftNode.text == node.text) {
                        return node.text;
                    }
                }

                
                let obj = filterByScope(alias_tbl, node.text, node, 0);
                
                if (obj != null && obj != undefined) {
                    return obj.tmp_var;
                } 
                
                return node.text;
                
                break;
            }
            
            // Basic types
            //case g.SyntaxType.Ellipsis:
            case g.SyntaxType.String:
            //case g.SyntaxType.Attribute:
            case g.SyntaxType.Integer:
            case g.SyntaxType.Float:
            case g.SyntaxType.True: 
            case g.SyntaxType.False: {
                return node.text;
                break;
            }
            
            case g.SyntaxType.Keyword: {
                let [,ndim,dim,,,, c,] = inferType(filename, node.parent.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                custom_functions = c;
                let firstNode = node.parent.namedChildren[1];
                let current_dim = 0;
                let dummyNode = node;
                if (node.parent.namedChildCount == 2) {
                    return `${numel(dim)}`;
                } else {
                    while (JSON.stringify(dummyNode) != JSON.stringify(firstNode)) {
                        dummyNode = dummyNode.previousNamedSibling;
                        current_dim += 1;
                    }
                    return `${dim[current_dim]}`;
                }
            }
            
            case g.SyntaxType.Complex: {
                
                return node.firstChild.text + "*I";
            }
            
            case g.SyntaxType.Slice: {
                let children_vals = []
                
                for (let i=0; i<node.namedChildCount; i++) {
                    let child = node.namedChildren[i];
                    let [child_type,,,,,, c,] = inferType(filename, child, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    
                    if (child_type == "keyword") {
                        
                        let [,ndim,dim,,,, c,] = inferType(filename, node.parent.valueNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                        custom_functions = c;
                        let firstNode = node.parent.namedChildren[1];
                        let current_dim = 0;
                        let dummyNode = node;
                        while (JSON.stringify(dummyNode) != JSON.stringify(firstNode)) {
                            dummyNode = dummyNode.previousNamedSibling;
                            current_dim += 1;
                        }
                        
                        children_vals.push(dim[current_dim]);
                        
                    } else {
                        extract_singular_mat = true;
                        children_vals.push(transformNode(file, child));
                        extract_singular_mat = false;
                        
                    }
                }
                
                let start = children_vals[0];
                let stop = children_vals[1];
                let step = 1;
                    
                if (children_vals.length == 3) {
                    stop = children_vals[2];
                    step = children_vals[1];
                }
        
                let [type, ndim, dim, , , ,,] = inferType(filename, node, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                let expression = [];
                let tmp_vec = generateTmpVar("vec", tmp_tbl);
                let tmp_iter = generateTmpVar("iter", tmp_tbl);
                if (dim.some(x=>isNaN(x))) {
                    dim = [1, `(int) floor((${stop}-${start})/${step}) + 1`];
                    //dim = [1, `(int) round((${stop}-${start})/${step}) + 1`];
                    //expression.push(`${type} *${tmp_vec};`);
                } else {
                    //expression.push(`${type} ${tmp_vec}[${numel(dim)}];`);
                }

                expression.push(`${type} ${tmp_vec}[${numel(dim)}];`);
                /*expression.push(`
for (int ${tmp_iter} = 0; ${start} + (${step})*${tmp_iter} < ${stop}; ${tmp_iter}++) {
\t${tmp_vec}[${tmp_iter}] = ${start} + (${step})*${tmp_iter};
}`);*/
                expression.push(`
for (int ${tmp_iter} = 0; ${tmp_iter} < ${dim[1]}; ${tmp_iter}++) {
\t${tmp_vec}[${tmp_iter}] = ${start} + (${step})*${tmp_iter};
}`);
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                let scope = findVarScope(filename, node, block_idxs, current_code, debug);
                tmp_var_types.push({
                    name: tmp_vec,
                    type: type,
                    ndim: ndim,
                    dim: dim,
                    ismatrix: false,
                    isvector: true,
                    ispointer: false,
                    isstruct: false,
                    initialized: true,
                    scope: scope
                });
                alias_tbl.push({
                    name: node.text,
                    tmp_var: tmp_vec,
                    //scope: [node.endIndex+1, scope[1], scope[2]]
                    scope: scope
                });
                return tmp_vec;
            }
            
            case g.SyntaxType.Matrix: {
                let [type, ndim, dim, , , ,,] = inferType(filename, node, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                
                /*if (ndim == 2 && dim.some(x => x === 1)) { // vector
                    let tmp_vec = generateTmpVar("vec", tmp_tbl);
                    let expression = [];
                    for (let child of node.namedChildren) {
                        expression.push(transformNode(file, child));
                    }
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(`${type} ${tmp_vec}[${numel(dim)}] = {${expression.join(", ")}};`, fun_params);
                    
                    tmp_var_types.push({
                        name: tmp_vec,
                        type: type,
                        ndim: 1,
                        dim: [numel(dim)],
                        ismatrix: false,
                        isvector: true,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: findVarScope(filename, node, block_idxs, current_code, debug)
                    });
                    
                    return tmp_vec;
                    
                } else { // matrix */
                    let tmp_mat = generateTmpVar("mat", tmp_tbl);
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(initializeMatrix(node, tmp_mat), fun_params);
                    
                    tmp_var_types.push({
                        name: tmp_mat,
                        type: type,
                        ndim: ndim,
                        dim: dim,
                        ismatrix: true,
                        isvector: false,
                        ispointer: false,
                        isstruct: false,
                        initialized: true,
                        scope: findVarScope(filename, node, block_idxs, current_code, debug)
                    });
            
                    return tmp_mat;
                //}
                
            }
        }
    }
    
    // Parse node
    // -----------------------------------------------------------------------------
    // Return args, outs from function
    function parseNode(node, ignore_parent) {
        if (debug == 1) {
            console.log("parseNode");
        }
        if (node.parent != undefined) {
            if (node.parent.type == g.SyntaxType.Assignment && ignore_parent == false) {
                return parseNode(node.parent, false);
            } 
        }
        switch (node.type) {
            case g.SyntaxType.Assignment: {
                var left_node = node.leftNode;
                var right_node = node.rightNode;
                break;
            }
            case g.SyntaxType.FunctionDefinition: {
                node = parseFunctionDefNode(node);
                if (node.return_variableNode != undefined) {
                    left_node = node.return_variableNode.firstNamedChild;
                } else {
                    left_node = null;
                }
                var right_node = node.parametersNode;
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
        if (right_node != null) {
            switch (right_node.type) {
                case g.SyntaxType.FunctionDefinition:
                case g.SyntaxType.CellSubscript:
                case g.SyntaxType.CallOrSubscript: {
    
                    for (let i = 1; i < right_node.namedChildCount; i++) {
                        args.push(right_node.namedChildren[i])
                    }
                    break;
                }
                case g.SyntaxType.ComparisonOperator:
                case g.SyntaxType.BooleanOperator:
                case g.SyntaxType.BinaryOperator: {
                    let [l_type, l_ndim, l_dim, l_ismatrix, l_ispointer, l_isstruct, c1,] = inferType(filename, right_node.leftNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c1;
                    let [r_type, r_ndim, r_dim, r_ismatrix, r_ispointer, r_isstruct, c2,] = inferType(filename, right_node.rightNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c2;
    
                    args.push(right_node.leftNode);
                    args.push(right_node.rightNode);
                    
                    break;
                }
                case g.SyntaxType.UnaryOperator:
                case g.SyntaxType.TransposeOperator: {
                    let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, right_node.argumentNode, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                    custom_functions = c;
                    
                    args.push(right_node.argumentNode);
    
                    break;
                }
            }
        }
        let outs = [];
        let is_subscript = [];
        if (left_node == null) {
        } else if (left_node.type == g.SyntaxType.Matrix) {
            for (let child of left_node.namedChildren) {
                
                if (child.type == g.SyntaxType.CallOrSubscript || child.type == g.SyntaxType.CellSubscript) {
                    is_subscript.push(true);
                }
                outs.push(child);
            }
        } else {
            
            if (left_node.type == g.SyntaxType.CallOrSubscript || left_node.type == g.SyntaxType.CellSubscript) {
                is_subscript.push(true);
            } 
            outs.push(left_node);
        }
        return [args, outs, is_subscript];
            

    }
    
    // Initialize matrices
    // -----------------------------------------------------------------------------
    function initializeMatrix(node, name) {
        let [type, ndim, dim, , , ,,] = inferType(filename, node, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
        let obj = type_to_matrix_type.find(x => x.type === type);
        
        if (numel(dim) == 0) {
            return `${type} ${name}[] = {};`;
        }
        
        if (obj == undefined) {
            console.error("ERROR IN INITIALIZE MATRIX: TYPE UNKNOWN");
            obj = type_to_matrix_type.find(x => x.type === 'double');
        }
        
        let expression = [];
        let tmp_ndim = generateTmpVar("ndim", tmp_tbl);
        let tmp_dim = generateTmpVar("dim", tmp_tbl);
        let scope = findVarScope(filename, node, block_idxs, current_code, debug);
        
        //papaya
        let tmp_input = generateTmpVar("input", tmp_tbl);
        
    	let j = 0;
    	for (let i=0; i<node.childCount; i++) {
            if (node.children[i].isNamed) {
                //let transform_child = node.children[i].text;
                let transform_child = transformNode(file, node.children[i]);
                let [child_type, child_ndim, child_dim, child_ismatrix, child_ispointer, child_isstruct, c,] = inferType(filename, node.children[i], tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
                if (obj.matrix_type == 3) {
                    expression.push(`${tmp_input}[${j}][] = ${transform_child.replace(/'/g, '"')};`);
                    j++;
                } else if (numel(child_dim) != 1 && !child_ismatrix) {
                    let tmp_iter = generateTmpVar("iter", tmp_tbl);
                    expression.push(`for (int ${tmp_iter} = 0; ${tmp_iter} < ${numel(child_dim)}; ${tmp_iter}++) {`);
                    expression.push(`   ${tmp_input}[${j} + ${tmp_iter}] = ${transform_child}[${tmp_iter}];`)
                    expression.push(`}`)
                    j+=numel(child_dim);
                } else {
                    expression.push(`${tmp_input}[${j}] = ${transform_child};`);
                    j++;
                }
                
            }
    	}
    	[type, ndim, dim, , , ,,] = inferType(filename, node, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);

    	expression.unshift(`${tmp_input} = malloc( ${numel(dim)}*sizeof(*${tmp_input}));`);
    	expression.unshift(`${type} *${tmp_input} = NULL;`);
    	
    	tmp_var_types.push({
            name: tmp_ndim,
            type: 'int',
            ndim: 1,
            dim: [1],
            ismatrix: false,
            isvector: false,
            ispointer: false,
            isstruct: false,
            initialized: true,
            scope: scope,
            propertyOf: name
        });
        tmp_var_types.push({
            name: tmp_dim,
            type: 'int',
            ndim: dim.length,
            dim: [dim.length],
            ismatrix: false,
            isvector: true,
            ispointer: false,
            isstruct: false,
            initialized: true,
            scope: scope,
            propertyOf: name
        });
        
        updateFunParams(0);
        let [name2, , flag1, flag2, fp] = isInitialized(name, node, type, fun_params);
        fun_params = fp;
        updateFunParams(1);
        let obj2 = filterByScope(tmp_var_types, name, node, 0);
        if (obj2 != null && obj2 != undefined) {
            //if (obj2.initialized || name.indexOf("[")>-1) {
            if (flag1 || name.indexOf("[")>-1) {
                expression.unshift(`${name} = createM(${tmp_ndim}, ${tmp_dim}, ${obj.matrix_type});`)    
            } else {
                expression.unshift(`Matrix * ${name} = createM(${tmp_ndim}, ${tmp_dim}, ${obj.matrix_type});`)
                //tmp_var_types = tmp_var_types.filter(function(e) { return e.name !== name });
            }
            //tmp_var_types = tmp_var_types.filter(function(e) { return JSON.stringify(e) !== JSON.stringify(obj2) });
            //obj2.initialized = true;
            //tmp_var_types.push(obj2);
            tmp_var_types = tmp_var_types.filter(x=> JSON.stringify(x) != JSON.stringify(obj2));
            tmp_var_types.push({
                name: name,
                type: type,
                ndim: ndim,
                dim: dim,
                ismatrix: true,
                isvector: false,
                ispointer: false,
                isstruct: false,
                initialized: true,
                scope: obj2.scope
            });
        } else {
            if (name.indexOf("[")>-1) {
                expression.unshift(`${name} = createM(${tmp_ndim}, ${tmp_dim}, ${obj.matrix_type});`)
            } else {
                expression.unshift(`Matrix * ${name} = createM(${tmp_ndim}, ${tmp_dim}, ${obj.matrix_type});`)
            }
        }
        
        expression.unshift(`int ${tmp_dim}[${ndim}] = {${dim}};`);
    	expression.unshift(`int ${tmp_ndim} = ${ndim};`);
        
    	expression.push(`writeM( ${name}, ${numel(dim)}, ${tmp_input});`);
    	expression.push(`free(${tmp_input});`)
    	return "\n" + expression.join("\n") + "\n";
    }
    
    // Print matrix functions
    // -----------------------------------------------------------------------------
    function printMatrixFunctions(node) {
        if (debug == 1) {
            console.log("printMatrixFunctions");
        }

        let [args1, outs, is_subscript] = parseNode(node, false);
        let arg_types = [];
        let args = [];
        for (let i = 0; i < args1.length; i ++) {
            let arg = args1[i];
            if (lhs_flag) {
                args.push(arg.text);
            } else if ((arg.text.includes("hamming") || arg.text.includes("hanning")) && node.valueNode.text == "stft") {
                args.push(`${arg.namedChildren[0].text}(${transformNode(file, arg.namedChildren[1])})`);
            } else {
                args.push(transformNode(file, arg));
            }
            let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, arg, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
            
            if (/tmp.*\[0\]/.test(args[i])) {
                [type, ndim, dim, ismatrix, , ispointer, isstruct, c,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
            }
            
            let isvector = numel(dim) > 1 && !ismatrix;
            
            // If a tmp var, infer type by name
            if (args[i] != null) {
                let match = args[i].match(/^([a-zA-Z_]+)(\d+)$/);
                if (match != null) {
                    let tmpvar_obj = tmp_tbl.find(x => x.name == match[1] && Number(x.count) >= Number(match[2]));
                    if (tmpvar_obj != null && tmpvar_obj != undefined) {
                        let [type2, ndim2, dim2, ismatrix2, isvector2, ispointer2, isstruct2, c2,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                        if (type2 != null) {
                            type = type2;
                            ndim = ndim2;
                            dim = dim2;
                            ismatrix = ismatrix2;
                            isvector = isvector2;
                            ispointer = ispointer2;
                            isstruct = isstruct2;
                            c = c2;
                        }
                        //[type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferTypeByName(args[i], node, tmp_var_types, custom_functions, alias_tbl, debug);
                    }
                }
            }
            
            custom_functions = c;
            arg_types.push({
                type: type, 
                ndim: ndim, 
                dim: dim, 
                ismatrix: ismatrix, 
                isvector: isvector, //numel(dim) > 1 && !ismatrix,
                ispointer: ispointer,
                isstruct: isstruct
            });
        }
        /*for (let arg of args1) {
            args.push(transformNode(file, arg));
            let [type, ndim, dim, ismatrix, ispointer, isstruct, c,] = inferType(filename, arg, tmp_var_types, custom_functions, classes, file, alias_tbl, debug);
            custom_functions = c;
            arg_types.push({
                type: type, 
                ndim: ndim, 
                dim: dim, 
                ismatrix: ismatrix, 
                isvector: numel(dim) > 1 && !ismatrix,
                ispointer: ispointer,
                isstruct: isstruct
            });
        }*/

        for (let i = 0; i < outs.length; i ++) {
            outs[i] = outs[i].text;
        }
        let obj = findBuiltin(operatorMapping, node.operatorNode.type);
        let req_arg_types = obj.req_arg_types(args, arg_types, outs);
        let return_type = obj.return_type(args, arg_types, outs);
        let init_before = obj.init_before(args, arg_types, outs);
        let fun_c = obj.fun_c(args, arg_types, outs, node.operatorNode.type);
        if (obj.args_transform(args, arg_types, outs) != null) {
            args = obj.args_transform(args, arg_types, outs);
        }
        if (req_arg_types != null) {
            for (let i = 0; i < Math.min(req_arg_types.length, arg_types.length); i ++) {
                if (!req_arg_types[i].ismatrix && arg_types[i].ismatrix) {
                    if (arg_types[i].dim.every(x => x === 1)) {
                        let expression = "";
                        updateFunParams(0);
                        [expression, fun_params] = extractSingularMat(args[i], arg_types[i], node, fun_params);
                        updateFunParams(1);
                        args[i] = expression;
                    }
                }
            }
        }
        if (init_before != null && init_before != undefined) {
                            
            for (let i = 0; i < init_before.length; i++) {
                let tmp_var = generateTmpVar(init_before[i].name, tmp_tbl);
                args[args.indexOf(init_before[i].name)] = tmp_var;
                args[args.indexOf("&" + init_before[i].name)] = "&" + tmp_var;
                for (let j = 0; j < init_before.length; j++) {
                    let re = new RegExp(`\\b${init_before[i].name}\\b`);
                    init_before[j].val = init_before[j].val.replace(re, tmp_var);
                }
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(initVar(tmp_var, init_before[i].val, init_before[i]), fun_params);
                tmp_var_types.push({
                    name: tmp_var,
                    type: init_before[i].type,
                    ndim: init_before[i].ndim,
                    dim: init_before[i].dim,
                    ismatrix: init_before[i].ismatrix,
                    isvector: numel(init_before[i].dim) > 1 && !init_before[i].ismatrix,
                    ispointer: init_before[i].ispointer,
                    isstruct: init_before[i].isstruct,
                    initialized: true,
                    scope: findVarScope(filename, node, block_idxs, current_code, debug)
                });
            }
        }
        if (fun_c == null) {
            switch (node.type) {
                case g.SyntaxType.UnaryOperator: {
                    //return `${node.operatorNode.type}${transformNode(file, node.argumentNode)}`;
                    return `${node.operatorNode.type}${args[0]}`;
                    break;
                }
                case g.SyntaxType.TransposeOperator: {
                    //return `${transformNode(file, node.argumentNode)}${node.operatorNode.type}`;
                    return `${args[0]}${node.operatorNode.type}`;
                    break;
                }
                case g.SyntaxType.ComparisonOperator:
                case g.SyntaxType.BooleanOperator:
                case g.SyntaxType.BinaryOperator: {
                    //return `${transformNode(file, node.leftNode)} ${node.operatorNode.type} ${transformNode(file, node.rightNode)}`;
                    return `${args[0]} ${node.operatorNode.type} ${args[1]}`;
                    break;
                }
            }
        } else if (if_flag) {
            let var_val = `${fun_c}()`;
            if (args[0] == "void") {
                var_val = `${fun_c}`;
            } else if (args != null) {
                var_val = `${fun_c}(${args.join(", ")})`;
            }
            return var_val;
        } else {
            let tmp_var = generateTmpVar("tmp", tmp_tbl);
            tmp_var_types.push({
                name: tmp_var,
                type: return_type.type,
                ndim: return_type.ndim,
                dim: return_type.dim,
                ismatrix: return_type.ismatrix,
                isvector: numel(return_type.dim) > 1 && !return_type.ismatrix,
                ispointer: return_type.ispointer,
                isstruct: false,
                initialized: true,
                scope: findVarScope(filename, node, block_idxs, current_code, debug)
            });
            let var_val = `${fun_c}()`;
            if (args[0] == "void") {
                var_val = `${fun_c}`;
            } else if (args != null) {
                var_val = `${fun_c}(${args.join(", ")})`;
            }
            updateFunParams(0);
            [main_function, function_definitions] = pushToMain(initVar(tmp_var, var_val, tmp_var_types[tmp_var_types.length - 1]), fun_params);
            return tmp_var;
            
        }
    }
    
    // Print function declarations and definitions
    // -----------------------------------------------------------------------------
    function printFunctionDefDeclare(node) {
        if (debug == 1) {
            console.log("printFunctionDefDeclare");
        }
        let obj = custom_functions.find(x => x.name === node.nameNode.text);
        
        if (obj != null) {
            for (let i = 0; i < obj.arg_type_dic.length; i ++) {
                let fun_name = `${node.nameNode.text}_${obj.arg_type_dic[i].arg_type_id}`;
                if (obj.arg_type_dic.length == 1) {
                    fun_name = node.nameNode.text;
                }
                
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(`${fun_name}_placeholder`, fun_params);
                

                for (let j = 0; j < obj.arg_type_dic[i].arg_types.length; j++) {
                    let arg_type = obj.arg_type_dic[i].arg_types[j];
                    if (isNaN(arg_type.dim[0]) && arg_type.ismatrix) {
                        let expression = [];
                        expression.push(`int *${arg_type.name}_dim = getDimsM(${arg_type.name});`)
                        for (let k = 0; k < arg_type.ndim; k++) {
                            expression.push(`int ${arg_type.dim[k]} = ${arg_type.name}_dim[${k}];`)
                        }
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                    }
                    if (isNaN(arg_type.dim[0]) && arg_type.ismatrix) {
                        let expression = [];
                        expression.push(`int ${arg_type.name}_type = gettypeM(${arg_type.name});`)
                        updateFunParams(0);
                        [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                    }
                }
                
                tmp_var_types = obj.arg_type_dic[i].var_types;
                if (tmp_var_types == null) {
                    tmp_var_types = [];
                }
                
                
                if (obj.var_arg_types != null) {
                    let expression = [];
                    expression.push("va_list varargin;");
                    expression.push("va_start(varargin, n_varargin);");
                    for (let k = 0; k < obj.var_arg_types.length; k++) {
                        expression.push(initVar(obj.var_arg_types[k].name, null, obj.var_arg_types[k]));
                    }
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(expression.join("\n"), fun_params);
                      
                }

                for (let child of node.bodyNode.namedChildren) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(transformNode(file, child), fun_params);
                }
                
                if (obj.var_arg_types != null) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain("va_end(varargin);", fun_params);
                      
                }

                let [, outs, ] = parseNode(node, false);

                for (let j = 0; j < outs.length; j ++) {
                    outs[j] = transformNode(file, outs[j]);
                }

                let ptr_args = obj.ptr_args(obj.arg_type_dic[i].arg_types, outs);
                var param_list = [];

                /*if (node.parametersNode != null) {
                    for (let j = 0; j < node.parametersNode.namedChildCount; j++) {
                        let param = node.parametersNode.namedChildren[j];
                        if (obj.arg_type_dic[i].arg_types[j].ismatrix) {
                            param_list.push(`Matrix * ${param.text}`);
                        } else {
                            param_list.push(`${obj.arg_type_dic[i].arg_types[j].type} ${param.text}`);
                        }
                    }
                }*/
                
                
                for (let j = 0; j < obj.arg_type_dic[i].arg_types.length; j++) {
                    if (obj.arg_type_dic[i].arg_types[j].ismatrix) {
                        param_list.push(`Matrix * ${obj.arg_type_dic[i].arg_types[j].name}`);
                    } else if (obj.arg_type_dic[i].arg_types[j].name == "varargin") {
                        param_list.push("int n_varargin");
                        param_list.push("...");
                    } else {
                        param_list.push(`${obj.arg_type_dic[i].arg_types[j].type} ${obj.arg_type_dic[i].arg_types[j].name}`);
                    }
                }
                
                
                let return_node = node.children[1].firstChild;
                //if (obj.return_type.ismatrix) {
                if (ptr_args != null) {
                    var ptr_declaration = [];
                    for (let i = 0; i < return_node.namedChildCount; i++) {
                        let return_var = return_node.namedChildren[i];
                        let tmp = transformNodeByName(return_var.text, return_var, alias_tbl);
                        ptr_declaration.push(`*p_${return_var.text} = ${tmp};`);
                        
                        if (tmp !== return_var.text) {
                            let [type, ndim, dim, ismatrix, , , ,] = inferTypeByName(tmp, node, tmp_var_types, custom_functions, alias_tbl, debug);
                            ptr_args[i].type = type;
                            ptr_args[i].ndim = ndim;
                            ptr_args[i].dim = dim;
                            ptr_args[i].ismatrix = ismatrix;
                        }
                        
                        // come back here
                        if (ptr_args[i].ismatrix) {
                            param_list.push(`Matrix ** p_${return_var.text}`);
                        } else {
                            param_list.push(`${ptr_args[i].type}* p_${return_var.text}`);
                        }
                    }
                    
                    var ptr_declaration_joined = ptr_declaration.join("\n");
                    //grapes
                    if (param_list.length == 0) {
                        var param_list_joined = "void";
                    } else {
                        var param_list_joined = param_list.join(", ");
                    }
    
                    function_declarations.push(`void ${fun_name}(${param_list_joined});`);
                    //pushToMain(`\nvoid ${node.nameNode.text}(${param_list_joined}) {`);
                    block_level -= 1;
                    updateFunParams(0);
                    [main_function, function_definitions] = replaceMain(`\nvoid ${fun_name}(${param_list_joined}) {`, `${fun_name}_placeholder`, 0, fun_params);
                    block_level += 1;
                   
                } else {
                    if (param_list.length == 0) {
                        var param_list_joined = "void";
                    } else {
                        var param_list_joined = param_list.join(", ");
                    }
                    
                    var return_statement = null
                    if (obj.arg_type_dic[i].return_type == null) {
                        var return_type = "void";
                    } else {
                        if (obj.arg_type_dic[i].return_type.ismatrix) {
                            var return_type = "Matrix *";
                        } else if (obj.arg_type_dic[i].return_type.ispointer) {
                            var return_type = `${obj.arg_type_dic[i].return_type.type} *`;
                        } else {
                            var return_type = `${obj.arg_type_dic[i].return_type.type}`;
                        }
                        return_statement = `return ${outs[0]};`;
                    }
    
                    function_declarations.push(`${return_type} ${fun_name}(${param_list_joined});`);
                    //pushToMain(`\n${return_type} ${node.nameNode.text}(${param_list_joined}) {`);
                    block_level -= 1;
                    updateFunParams(0);
                    [main_function, function_definitions] = replaceMain(`\n${return_type} ${fun_name}(${param_list_joined}) {`, `${fun_name}_placeholder`, 0, fun_params);
                    block_level += 1;
                }
                
                if (ptr_declaration != undefined) {
                    updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(ptr_declaration.join("\n"), fun_params);
                }
    
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain(return_statement, fun_params);
                block_level -= 1;
                updateFunParams(0);
                [main_function, function_definitions] = pushToMain("}", fun_params);
                block_level += 1;
            }
                
        }
    
    }
    
    // Print function declarations and definitions
    // -----------------------------------------------------------------------------
    function printClass(class_obj) {
        if (debug == 1) {
            console.log("printClass");
        }
        printed_classes.push(class_obj.name);
                    
        let expression = [];
        expression.push(`struct ${class_obj.name} {`);
        for (let i = 0; i < class_obj.properties.length; i ++) {
            expression.push(`\t${initVar(class_obj.properties[i].name, null, class_obj.properties[i])}`)
            //expression.push(`\t${class_obj.properties[i].type} ${class_obj.properties[i].name};`)
        }
        expression.push('}');
        
        let main_function2 = [];
        let function_definitions2 = [];
        let function_declarations2 = [];
        let fun_params2 = fun_params;
        fun_params2.main_function = [];
        fun_params2.function_definitions = [];
        fun_params2.function_declarations = [];
        fun_params2.block_level = 0;
        [main_function2, function_definitions2] = pushToMain(expression.join("\n"), fun_params2);
        
        //console.log("CLASS:");
        //console.log(class_obj.name);
        //console.log("--------------------------");

        for (let k = 0; k < class_obj.methods.length; k++) {
            let method_obj = class_obj.methods[k];
            let node = method_obj.def_node;
            
            for (let i = 0; i < method_obj.arg_type_dic.length; i++) {
                let fun_name = `${class_obj.name}_${method_obj.name}_${method_obj.arg_type_dic[i].arg_type_id}`;
                if (method_obj.arg_type_dic.length == 1) {
                    fun_name = `${class_obj.name}_${method_obj.name}`;
                }
                //updateFunParams(0);
                fun_params2.main_function = main_function2; 
                fun_params2.function_definitions = function_definitions2;
                [main_function2, function_definitions2] = pushToMain(`${fun_name}_placeholder`, fun_params2);

                tmp_var_types = method_obj.arg_type_dic[i].var_types;
                if (tmp_var_types == null) {
                    tmp_var_types = [];
                }

                fun_params2.main_function = main_function2; 
                fun_params2.function_definitions = function_definitions2;
                [main_function2, function_definitions2] = pushToMain(transformNode(method_obj.file, node.bodyNode), fun_params2);
                //console.log("NAMEDCHILDREN");
                /*for (let child of node.bodyNode.namedChildren) {
                    //updateFunParams(0);
                    //console.log(child.text);
                    //console.log(child.type);
                    fun_params2.main_function = main_function2; 
                    fun_params2.function_definitions = function_definitions2;
                    [main_function2, function_definitions2] = pushToMain(transformNode(method_obj.file, child), fun_params2);
                }*/
                let [, outs, ] = parseNode(node, false);
                for (let j = 0; j < outs.length; j ++) {
                    outs[j] = transformNode(file, outs[j]);
                }
                let ptr_args = method_obj.ptr_args(method_obj.arg_type_dic[i].arg_types, outs);
                var param_list = [];

                if (node.parametersNode != null) {
                    for (let j = 0; j < node.parametersNode.namedChildCount; j++) {
                        let param = node.parametersNode.namedChildren[j];
                        if (method_obj.arg_type_dic[i].arg_types[j].ismatrix) {
                            param_list.push(`Matrix * ${param.text}`);
                        } else {
                            param_list.push(`${method_obj.arg_type_dic[i].arg_types[j].type} ${param.text}`);
                        }
                    }
                }

                let return_node = node.return_variableNode; //node.children[1].firstChild;
                //if (obj.return_type.ismatrix) {
                if (ptr_args != null) {
                    var ptr_declaration = [];
                    for (let j = 0; j < return_node.namedChildCount; j++) {
                        let return_var = return_node.namedChildren[j];
                        let tmp = transformNodeByName(return_var.text, return_var, alias_tbl);
                        ptr_declaration.push(`*p_${return_var.text} = ${tmp};`);
                        
                        if (tmp !== return_var.text) {
                            let [type, ndim, dim, ismatrix, , , ,] = inferTypeByName(tmp, node, tmp_var_types, custom_functions, alias_tbl, debug);
                            ptr_args[j].type = type;
                            ptr_args[j].ndim = ndim;
                            ptr_args[j].dim = dim;
                            ptr_args[j].ismatrix = ismatrix;
                        }
                        
                        // come back here
                        if (ptr_args[j].ismatrix) {
                            param_list.push(`Matrix ** p_${return_var.text}`);
                        } else {
                            param_list.push(`${ptr_args[j].type}* p_${return_var.text}`);
                        }
                    }
                    
                    var ptr_declaration_joined = ptr_declaration.join("\n");
                    //grapes
                    if (param_list.length == 0) {
                        var param_list_joined = "void";
                    } else {
                        var param_list_joined = param_list.join(", ");
                    }
    
                    function_declarations.push(`void ${fun_name}(${param_list_joined});`);
                    //pushToMain(`\nvoid ${node.nameNode.text}(${param_list_joined}) {`);
                    block_level -= 1;
                    //updateFunParams(0);
                    fun_params2.main_function = main_function2; 
                    fun_params2.function_definitions = function_definitions2;
                    [main_function2, function_definitions2] = replaceMain(`\nvoid ${fun_name}(${param_list_joined}) {`, `${fun_name}_placeholder`, 0, fun_params2);
                    block_level += 1;
                   
                } else {
                    if (param_list.length == 0) {
                        var param_list_joined = "void";
                    } else {
                        var param_list_joined = param_list.join(", ");
                    }
                    
                    var return_statement = null
                    if (method_obj.arg_type_dic[i].return_type == null) {
                        var return_type = "void";
                    } else {
                        if (method_obj.arg_type_dic[i].return_type.ismatrix) {
                            var return_type = "Matrix *";
                        } else if (method_obj.arg_type_dic[i].return_type.ispointer) {
                            var return_type = `${method_obj.arg_type_dic[i].return_type.type} *`;
                        } else {
                            var return_type = `${method_obj.arg_type_dic[i].return_type.type}`;
                        }
                        return_statement = `return ${outs[0]};`;
                    }
    
                    function_declarations2.push(`${return_type} ${fun_name}(${param_list_joined});`);
                    
                    block_level -= 1;
                    //updateFunParams(0);
                    fun_params2.main_function = main_function2; 
                    fun_params2.function_definitions = function_definitions2;
                    [main_function2, function_definitions2] = replaceMain(`\n${return_type} ${fun_name}(${param_list_joined}) {`, `${fun_name}_placeholder`, 0, fun_params2);
                    block_level += 1;
                }
                
                if (ptr_declaration != undefined) {
                    //updateFunParams(0);
                    [main_function, function_definitions] = pushToMain(ptr_declaration.join("\n"), fun_params);
                }
    
                //updateFunParams(0);
                fun_params2.main_function = main_function2; 
                fun_params2.function_definitions = function_definitions2;
                [main_function2, function_definitions2] = pushToMain(return_statement, fun_params2);
                block_level -= 1;
                //updateFunParams(0);
                fun_params2.main_function = main_function2; 
                fun_params2.function_definitions = function_definitions2;
                [main_function2, function_definitions2] = pushToMain("}", fun_params2);
                block_level += 1;
            }
                
        }
    
        writeToFile(out_folder, class_obj.name + ".c", main_function2.concat(function_definitions2).join("\n"));
        //console.log(main_function2);
        //console.log(function_definitions2);
    }
    
    // Generate header files
    // -----------------------------------------------------------------------------
    function generateHeader() {
        if (debug == 1) {
            console.log("generateHeader");
        }
        
        let macro_fun = filename.toUpperCase() + "_H";
        header.push(`#ifndef ${macro_fun}`);
        header.push(`#define ${macro_fun}`);
        if (function_definitions.length == 0) {
            header.push("\n// Function declarations")
            header.push(function_declarations.join("\n"));
        } else {
            header.push("int " + filename + "(void);");
        }
        header.push("#endif");
        writeToFile(out_folder, filename + ".h", header.join("\n"));
        
    }
    
    // Get subscript index
    // -----------------------------------------------------------------------------
    function getSubscriptIdx(node, count) {
        if (debug == 1) {
            console.log("getSubscriptIdx");
        }

        /*let lhs_flag = false;
        if (node.parent.type == g.SyntaxType.Assignment) {
            if (node.parent.leftNode.text == node.text) {
                lhs_flag = true;
            }
        }*/
        let obj = filterByScope(tmp_var_types, node.valueNode.text, node, 0);
        let dim = [1, 1];
        if (obj == null || obj == undefined) {
            let obj1 = filterByScope(alias_tbl, node.valueNode.text, node, 0);
            if (obj1 != null && obj1 != undefined) {
                obj = filterByScope(tmp_var_types, obj1.tmp_var, node, 0);
                dim = obj.dim;
            } else {
                console.error("ERROR IN GETSUBSCRIPTIDX: OBJ1 UNDEFINED");
            }
            
        } else {
            dim = obj.dim;
        }
        
        if (dim[3] == 1) {
            dim.pop();
        }
        if (dim[2] == 1) {
            dim.pop();
        }

        let idx = [node.namedChildren[1].text];
        updateFunParams(0);
        // already a linear idx
        if (node.namedChildCount == 2) {
            if (node.namedChildren[1].type == g.SyntaxType.Slice) {
                idx = slice2list(node.namedChildren[1], fun_params)
            } else if (node.namedChildren[1].type == g.SyntaxType.Matrix) {
                idx = matrix2list(node.namedChildren[1], fun_params)
            } else {
                //[fun_params, idx] = rowMajorFlatIdx(count, dim, transformNode(file, node.namedChildren[1]), fun_params);
                //updateFunParams(1);
                let good_flag = !lhs_flag || node.type == g.SyntaxType.CellSubscript;
                if (node.type == g.SyntaxType.CellSubscript) {
                    if (node.nextSibling != null) {
                        if (node.nextSibling.type == "(") {
                            good_flag = false;
                        }
                    }
                }
                if (good_flag) {
                    updateFunParams(0);
                    let tmp_idx = generateTmpVar("idx", tmp_tbl);
                    [main_function, function_definitions] = pushToMain(`int ${tmp_idx} = convertSubscript(ndim${count}, dim${count}, ${transformNode(file, node.namedChildren[1])});`, fun_params);
                    idx = [tmp_idx];
                }
            }
            
        }
        else {
            if (node.namedChildCount == 3) {
                idx = sub2idx(
                    node.namedChildren[1], transformNode(file, node.namedChildren[1]),
                    node.namedChildren[2], transformNode(file, node.namedChildren[2]),
                    null, null,
                    null, null,
                    dim[0], dim[1], 1,
                    fun_params
                );
            } else if (node.namedChildCount == 4) {
                idx = sub2idx(
                    node.namedChildren[1], transformNode(file, node.namedChildren[1]),
                    node.namedChildren[2], transformNode(file, node.namedChildren[2]),
                    node.namedChildren[3], transformNode(file, node.namedChildren[3]),
                    null, null,
                    dim[0], dim[1], 1,
                    fun_params
                );
            } else if (node.namedChildCount == 5) {
                idx = sub2idx(
                    node.namedChildren[1], transformNode(file, node.namedChildren[1]),
                    node.namedChildren[2], transformNode(file, node.namedChildren[2]),
                    node.namedChildren[3], transformNode(file, node.namedChildren[3]),
                    node.namedChildren[4], transformNode(file, node.namedChildren[4]),
                    dim[0], dim[1], dim[2],
                    fun_params
                );
            }
        }
        return idx;
    }
    
    // Put together generated code
    // -----------------------------------------------------------------------------
    main();
    
    generated_code.push(link.join("\n"));
    if (function_definitions.length != 0) {
        generated_code.push("\n// Function declarations")
        generated_code.push(function_declarations.join("\n"));
    }
    if (!fileIsFunction(tree, debug)){
    generated_code.push(
`\n// Entry-point function
int ${filename}(void) {`);
    }
    
    generated_code.push("\n" + main_function.join("\n"));
    if (!fileIsFunction(tree, fileIsFunction)){
        generated_code.push("\treturn 0;");
        generated_code.push("}\n");
    }

    if (function_definitions.length != 0) {
        generated_code.push("\n// Subprograms");
        generated_code.push(function_definitions.join("\n"));
    }
    
    generateHeader();
    
    writeToFile(out_folder, filename + ".c", generated_code.join("\n"));
    return [generated_code.join("\n"), header.join("\n"), var_types, custom_functions];
}