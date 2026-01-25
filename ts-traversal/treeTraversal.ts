import * as g from "./generated";
import { parseFunctionDefNode } from "./helperFunctions";

// Tree traversal function
// -----------------------------------------------------------------------------

export function gotoPreorderSucc(cursor: g.TreeCursor, debug): boolean {
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

export function gotoPreorderSucc_OnlyMajorTypes(cursor: g.TreeCursor, debug): boolean {
    /*if (debug == 1) {
        console.log("generateCode.ts");
        console.log(cursor.currentNode);
        console.log(cursor.currentNode.text);
    }*/
    switch (cursor.currentNode.type) {
        // Don't iterate through children nodes
        case g.SyntaxType.BinaryOperator:
        case g.SyntaxType.BooleanOperator:
        case g.SyntaxType.ComparisonOperator:
        case g.SyntaxType.TransposeOperator:
        case g.SyntaxType.UnaryOperator:
        case g.SyntaxType.Assignment:
        case g.SyntaxType.CallOrSubscript:
        case g.SyntaxType.Comment:
        case g.SyntaxType.ExpressionStatement:
        case g.SyntaxType.FunctionDefinition:
        case g.SyntaxType.IfStatement:
        case g.SyntaxType.WhileStatement:
        case g.SyntaxType.ForStatement: {
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

export function gotoPreorderSucc_SkipFunctionDef(cursor: g.TreeCursor, debug): boolean {
    /*if (debug == 1) {
        console.log("typeInference.ts");
        console.log(cursor.currentNode);
        console.log(cursor.currentNode.text);
    }*/
    switch (cursor.currentNode.type) {
        // Don't iterate through children nodes
        //case g.SyntaxType.Assignment:
        case g.SyntaxType.CallOrSubscript:
        case g.SyntaxType.ERROR:
        case g.SyntaxType.FunctionDefinition: {
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

export function fileIsFunction(tree, debug): boolean {
    var encountered_code_before = false;
    var encountered_function = false;
    var encountered_code_after = false;
    let cursor = tree.walk();
    do {
        const c = cursor as g.TypedTreeCursor;
        let node = c.currentNode;
        if (debug == 1) {
            console.log("fileIsFunction");
            console.log(cursor.currentNode);
            //console.log(cursor.currentNode.text);
        }
        switch (node.type) {
            case g.SyntaxType.ERROR:
                node = parseFunctionDefNode(c.currentNode);
                if (node != null) {
                    if (encountered_function) {
                        return false;
                    }
                    encountered_function = true;
                    if (encountered_code_before) {
                        return false;
                    }
                } else {
                    if (encountered_function) {
                        encountered_code_after = true;
                    } else {
                        encountered_code_before = true;
                    }
                }
                break;
            case g.SyntaxType.FunctionDefinition: {
                if (encountered_function) {
                    return false;
                }
                encountered_function = true;
                if (encountered_code_before) {
                    return false;
                }
                break;
            }
            case g.SyntaxType.Module:
            case g.SyntaxType.Comment: {
                break;
            }
            default: {
                if (encountered_function) {
                    encountered_code_after = true;
                } else {
                    encountered_code_before = true;
                }
                break;
            }
        }    
    } while(gotoPreorderSucc_SkipFunctionDef(cursor, debug));
    
    if (!encountered_function || encountered_code_after) {
        return false;
    } else
        return true;
}

export function findEntryFunction(tree, debug) {
    if (fileIsFunction(tree, debug)) {
        let cursor = tree.walk(); 
        do {
            const c = cursor as g.TypedTreeCursor;
            let node = parseFunctionDefNode(c.currentNode);
            if (debug == 1) {
                console.log("findEntryFunction");
                console.log(cursor.currentNode);
                //console.log(cursor.currentNode.text);
            }
            if (node != null) {
                return node;
            }
        } while(gotoPreorderSucc(cursor, debug));
    }
    else {
        return null;
    }
}