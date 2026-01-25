"use strict";
exports.__esModule = true;
exports.findEntryFunction = exports.fileIsFunction = exports.gotoPreorderSucc_SkipFunctionDef = exports.gotoPreorderSucc_OnlyMajorTypes = exports.gotoPreorderSucc = void 0;
var helperFunctions_1 = require("./helperFunctions");
// Tree traversal function
// -----------------------------------------------------------------------------
function gotoPreorderSucc(cursor, debug) {
    /*if (debug == 1) {
        console.log("identifyCustomFunctions.ts");
        console.log(cursor.currentNode);
    }*/
    if (cursor.gotoFirstChild())
        return true;
    while (!cursor.gotoNextSibling()) {
        if (!cursor.gotoParent()) {
            return false;
        }
        if (debug == 1) {
            console.log("identifyCustomFunctions.ts");
            console.log(cursor.currentNode);
        }
    }
    return true;
}
exports.gotoPreorderSucc = gotoPreorderSucc;
function gotoPreorderSucc_OnlyMajorTypes(cursor, debug) {
    /*if (debug == 1) {
        console.log("generateCode.ts");
        console.log(cursor.currentNode);
        console.log(cursor.currentNode.text);
    }*/
    switch (cursor.currentNode.type) {
        // Don't iterate through children nodes
        case "binary_operator" /* g.SyntaxType.BinaryOperator */:
        case "boolean_operator" /* g.SyntaxType.BooleanOperator */:
        case "comparison_operator" /* g.SyntaxType.ComparisonOperator */:
        case "transpose_operator" /* g.SyntaxType.TransposeOperator */:
        case "unary_operator" /* g.SyntaxType.UnaryOperator */:
        case "assignment" /* g.SyntaxType.Assignment */:
        case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */:
        case "comment" /* g.SyntaxType.Comment */:
        case "expression_statement" /* g.SyntaxType.ExpressionStatement */:
        case "function_definition" /* g.SyntaxType.FunctionDefinition */:
        case "if_statement" /* g.SyntaxType.IfStatement */:
        case "while_statement" /* g.SyntaxType.WhileStatement */:
        case "for_statement" /* g.SyntaxType.ForStatement */: {
            while (!cursor.gotoNextSibling()) {
                if (!cursor.gotoParent()) {
                    return false;
                }
            }
            if (debug == 1) {
                console.log("generateCode.ts");
                console.log(cursor.currentNode);
                console.log(cursor.currentNode.text);
            }
            break;
        }
        default: {
            if (cursor.gotoFirstChild())
                return true;
            while (!cursor.gotoNextSibling()) {
                if (!cursor.gotoParent()) {
                    return false;
                }
            }
            break;
        }
    }
    return true;
}
exports.gotoPreorderSucc_OnlyMajorTypes = gotoPreorderSucc_OnlyMajorTypes;
function gotoPreorderSucc_SkipFunctionDef(cursor, debug) {
    /*if (debug == 1) {
        console.log("typeInference.ts");
        console.log(cursor.currentNode);
        console.log(cursor.currentNode.text);
    }*/
    switch (cursor.currentNode.type) {
        // Don't iterate through children nodes
        //case g.SyntaxType.Assignment:
        case "call_or_subscript" /* g.SyntaxType.CallOrSubscript */:
        case "ERROR" /* g.SyntaxType.ERROR */:
        case "function_definition" /* g.SyntaxType.FunctionDefinition */: {
            while (!cursor.gotoNextSibling()) {
                if (!cursor.gotoParent()) {
                    return false;
                }
                if (debug == 1) {
                    console.log("typeInference.ts");
                    console.log(cursor.currentNode);
                    console.log(cursor.currentNode.text);
                }
            }
            break;
        }
        default: {
            if (cursor.gotoFirstChild())
                return true;
            if (debug == 1) {
                console.log("typeInference.ts");
                console.log(cursor.currentNode);
                console.log(cursor.currentNode.text);
            }
            while (!cursor.gotoNextSibling()) {
                if (!cursor.gotoParent()) {
                    return false;
                }
                if (debug == 1) {
                    console.log("typeInference.ts");
                    console.log(cursor.currentNode);
                    console.log(cursor.currentNode.text);
                }
            }
            break;
        }
    }
    return true;
}
exports.gotoPreorderSucc_SkipFunctionDef = gotoPreorderSucc_SkipFunctionDef;
function fileIsFunction(tree, debug) {
    var encountered_code_before = false;
    var encountered_function = false;
    var encountered_code_after = false;
    var cursor = tree.walk();
    do {
        var c = cursor;
        var node = c.currentNode;
        if (debug == 1) {
            console.log("fileIsFunction");
            console.log(cursor.currentNode);
            //console.log(cursor.currentNode.text);
        }
        switch (node.type) {
            case "ERROR" /* g.SyntaxType.ERROR */:
                node = (0, helperFunctions_1.parseFunctionDefNode)(c.currentNode);
                if (node != null) {
                    if (encountered_function) {
                        return false;
                    }
                    encountered_function = true;
                    if (encountered_code_before) {
                        return false;
                    }
                }
                else {
                    if (encountered_function) {
                        encountered_code_after = true;
                    }
                    else {
                        encountered_code_before = true;
                    }
                }
                break;
            case "function_definition" /* g.SyntaxType.FunctionDefinition */: {
                if (encountered_function) {
                    return false;
                }
                encountered_function = true;
                if (encountered_code_before) {
                    return false;
                }
                break;
            }
            case "module" /* g.SyntaxType.Module */:
            case "comment" /* g.SyntaxType.Comment */: {
                break;
            }
            default: {
                if (encountered_function) {
                    encountered_code_after = true;
                }
                else {
                    encountered_code_before = true;
                }
                break;
            }
        }
    } while (gotoPreorderSucc_SkipFunctionDef(cursor, debug));
    if (!encountered_function || encountered_code_after) {
        return false;
    }
    else
        return true;
}
exports.fileIsFunction = fileIsFunction;
function findEntryFunction(tree, debug) {
    if (fileIsFunction(tree, debug)) {
        var cursor = tree.walk();
        do {
            var c = cursor;
            var node = (0, helperFunctions_1.parseFunctionDefNode)(c.currentNode);
            if (debug == 1) {
                console.log("findEntryFunction");
                console.log(cursor.currentNode);
                //console.log(cursor.currentNode.text);
            }
            if (node != null) {
                return node;
            }
        } while (gotoPreorderSucc(cursor, debug));
    }
    else {
        return null;
    }
}
exports.findEntryFunction = findEntryFunction;
//# sourceMappingURL=treeTraversal.js.map