"use strict";
exports.__esModule = true;
exports.identifyCustomFunctions = void 0;
var fs = require("fs");
var helperFunctions_1 = require("./helperFunctions");
var treeTraversal_1 = require("./treeTraversal");
var Parser = require("tree-sitter");
var Matlab = require("tree-sitter-matlab");
//import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");
//import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/tree-sitter-matlab");
var parser = new Parser();
parser.setLanguage(Matlab);
// Identify function definitions
// -----------------------------------------------------------------------------
function identifyCustomFunctions(tree, custom_functions, files, filename, file_traversal_order, debug) {
    // Internal functions
    var cursor = tree.walk();
    do {
        var c = cursor;
        var node = (0, helperFunctions_1.parseFunctionDefNode)(c.currentNode);
        if (node != null) {
            var arg_types = [];
            if (node.parametersNode != null && node.parametersNode != undefined) {
                for (var _i = 0, _a = node.parametersNode.namedChildren; _i < _a.length; _i++) {
                    var arg = _a[_i];
                    // Placeholder
                    /*arg_types.push({
                        name: arg.text,
                        type: null,
                        ndim: null,
                        dim: null,
                        ismatrix: null,
                        isstruct: null,
                        ispointer: null,
                        original_out: false
                    });*/
                    //if (arg.text != "varargin") {
                    arg_types.push({
                        name: arg.text,
                        type: "".concat(arg.text, "_type"),
                        ndim: 2,
                        dim: ["".concat(arg.text, "_dim0"), "".concat(arg.text, "_dim1")],
                        ismatrix: null,
                        isstruct: null,
                        ispointer: null,
                        original_out: false
                    });
                    //}
                }
            }
            var v1 = {
                name: node.nameNode.text,
                arg_types: arg_types,
                var_arg_types: null,
                return_type: null,
                outs_transform: function (outs) { return outs; },
                ptr_args: function (arg_types, outs) { return null; },
                ptr_arg_types: null,
                external: filename !== file_traversal_order.slice(-1),
                file: filename,
                def_node: node,
                var_types: null,
                arg_type_dic: []
            };
            custom_functions.push(v1);
        }
    } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, debug));
    // External functions
    cursor = tree.walk();
    var _loop_1 = function () {
        var _b;
        var c = cursor;
        switch (c.nodeType) {
            case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */: {
                var node_1 = c.currentNode;
                var obj_1 = custom_functions.find(function (x) { return x.name === node_1.valueNode.text; });
                if (obj_1 == null) {
                    var match = files.find(function (element) {
                        if (element.includes("/" + node_1.valueNode.text + ".m")) {
                            return true;
                        }
                    });
                    if (match !== undefined) {
                        // To perform type inference on files from most to least deeply nested
                        file_traversal_order.unshift(match);
                        var functionCode = fs.readFileSync(match, "utf8");
                        var tree2 = parser.parse(functionCode);
                        _b = identifyCustomFunctions(tree2, custom_functions, files, match, file_traversal_order, debug), custom_functions = _b[0], file_traversal_order = _b[1];
                    }
                }
                else {
                    if (obj_1.external) {
                        file_traversal_order = file_traversal_order.filter(function (e) { return e !== obj_1.file; });
                        file_traversal_order.unshift(obj_1.file);
                    }
                }
                break;
            }
        }
    };
    do {
        _loop_1();
    } while ((0, treeTraversal_1.gotoPreorderSucc)(cursor, debug));
    return [custom_functions, file_traversal_order];
}
exports.identifyCustomFunctions = identifyCustomFunctions;
//# sourceMappingURL=identifyCustomFunctions.js.map