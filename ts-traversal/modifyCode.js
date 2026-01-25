"use strict";
exports.__esModule = true;
exports.replaceMain = exports.insertMain = exports.pushToMain = void 0;
function pushToMain(expression, fun_params) {
    if (fun_params.debug == 1) {
        console.log("pushToMain");
    }
    if (expression != null) {
        // Indent expression
        var indent = '\t'.repeat(fun_params.block_level);
        if (expression.slice(0, 2) != '\n') {
            expression = indent.concat(expression);
        }
        expression = expression.replace(/\n|\r/gm, "\n".concat(indent));
        // Push expression
        if (fun_params.current_code == "main") {
            fun_params.main_function.push(expression);
        }
        else if (fun_params.entry_fun_node != null) {
            if (fun_params.entry_fun_node.nameNode.text == fun_params.current_code) {
                fun_params.main_function.push(expression);
            }
        }
        else {
            fun_params.function_definitions.push(expression);
        }
    }
    return [fun_params.main_function, fun_params.function_definitions];
}
exports.pushToMain = pushToMain;
//export function insertMain(expression, search_exp, num_back, before_after, fun_params) {
function insertMain(expression, search_exp, before_after, fun_params) {
    if (fun_params.debug == 1) {
        console.log("insertMain");
    }
    // Push expression
    // num_back: if more than one instance of search_exp is found, which instance to choose as formatted as
    // matches[matches.length - num_back]
    var idx = fun_params.main_function.reduce(function (a, e, i) {
        if (e.includes(search_exp))
            a.push(i);
        return a;
    }, []);
    if (fun_params.current_code != "main") {
        idx = fun_params.function_definitions.reduce(function (a, e, i) {
            if (e.includes(search_exp))
                a.push(i);
            return a;
        }, []);
    }
    //idx = idx[idx.length - num_back];
    idx = idx[idx.length - 1];
    if (idx == undefined) {
        console.error("ERROR IN INSERT MAIN: IDX UNDEFINED");
        return [fun_params.main_function, fun_params.function_definitions, fun_params.block_level];
    }
    // Indent expression
    var match = fun_params.main_function[idx].match(/\t+/);
    if (fun_params.current_code != "main") {
        match = fun_params.function_definitions[idx].match(/\t+/);
    }
    var indent = match[0];
    var block_level = indent.length;
    if (expression.slice(0, 2) != '\n') {
        expression = indent.concat(expression);
    }
    expression = expression.replace(/\n|\r/gm, "\n".concat(indent));
    if (expression != null) {
        if (fun_params.current_code == "main") {
            if (before_after == 1) {
                //main_function.splice(idx, 0, expression);
                fun_params.main_function.splice(idx + 1, 0, expression);
            }
            else {
                //main_function.splice(idx-1, 0, expression);
                fun_params.main_function.splice(idx, 0, expression);
            }
        }
        else if (fun_params.entry_fun_node != null) {
            if (fun_params.entry_fun_node.nameNode.text == fun_params.current_code) {
                fun_params.main_function.splice(idx, 0, expression);
                if (before_after == 1) {
                    fun_params.main_function.splice(idx + 1, 0, expression);
                }
                else {
                    fun_params.main_function.splice(idx, 0, expression);
                }
            }
        }
        else {
            var idx_1 = fun_params.function_definitions.reduce(function (a, e, i) {
                if (e.includes(search_exp))
                    a.push(i);
                return a;
            }, []);
            //idx = idx[idx.length - num_back];
            idx_1 = idx_1[idx_1.length - 1];
            if (before_after == 1) {
                fun_params.function_definitions.splice(idx_1 + 1, 0, expression);
            }
            else {
                fun_params.function_definitions.splice(idx_1, 0, expression);
            }
        }
    }
    return [fun_params.main_function, fun_params.function_definitions, block_level];
}
exports.insertMain = insertMain;
//export function replaceMain(expression, search_exp, num_back, fun_params) {
function replaceMain(expression, search_exp, replace_all, fun_params) {
    if (fun_params.debug == 1) {
        console.log("replaceMain");
    }
    // Indent expression
    /*let indent = '\t'.repeat(fun_params.block_level);
    if (expression.slice(0, 2) != '\n') {
        expression = indent.concat(expression);
    }
    expression = expression.replace(/\n|\r/gm, `\n${indent}`);*/
    // Push expression
    // num_back: if more than one instance of search_exp is found, which instance to choose as formatted as
    // matches[matches.length - num_back]
    var idx = fun_params.main_function.reduce(function (a, e, i) {
        if (e.includes(search_exp))
            a.push(i);
        return a;
    }, []);
    /*if (idx.length > 1) {
        idx = idx[idx.length - num_back];
    }*/
    var re = new RegExp("\\b".concat(search_exp, "\\b"), 'g');
    if (!replace_all) {
        idx = [idx[idx.length - 1]];
    }
    if (expression != null) {
        if (fun_params.current_code == "main") {
            //fun_params.main_function[idx] = expression;
            for (var i = 0; i < idx.length; i++) {
                fun_params.main_function[idx[i]] = fun_params.main_function[idx[i]].replace(re, expression);
            }
        }
        else if (fun_params.entry_fun_node != null) {
            if (fun_params.entry_fun_node.nameNode.text == fun_params.current_code) {
                //main_function.splice(idx, 0, expression);
                //fun_params.main_function[idx] = expression;
                for (var i = 0; i < idx.length; i++) {
                    fun_params.main_function[idx[i]] = fun_params.main_function[idx[i]].replace(re, expression);
                }
            }
        }
        else {
            var idx_2 = fun_params.function_definitions.reduce(function (a, e, i) {
                if (e.includes(search_exp))
                    a.push(i);
                return a;
            }, []);
            /*if (idx.length > 1) {
                idx = idx[idx.length - num_back];
            }
            fun_params.function_definitions[idx] = expression;*/
            if (!replace_all) {
                idx_2 = [idx_2[idx_2.length - 1]];
            }
            for (var i = 0; i < idx_2.length; i++) {
                fun_params.function_definitions[idx_2[i]] = fun_params.function_definitions[idx_2[i]].replace(re, expression);
            }
            //console.log(fun_params.function_definitions);
        }
    }
    return [fun_params.main_function, fun_params.function_definitions];
}
exports.replaceMain = replaceMain;
//# sourceMappingURL=modifyCode.js.map