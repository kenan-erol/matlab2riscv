"use strict";
var _a;
exports.__esModule = true;
require('source-map-support').install();
//const fs = require("graceful-fs");
var fs = require('fs');
var path = require("path");
var gracefulFs = require('graceful-fs');
gracefulFs.gracefulify(fs);
var generateCode_1 = require("./generateCode");
var helperFunctions_1 = require("./helperFunctions");
var identifyCustomFunctions_1 = require("./identifyCustomFunctions");
var typeInference_1 = require("./typeInference");
var Parser = require("tree-sitter");
var Matlab = require("tree-sitter-matlab");
//import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");
var parser = new Parser();
parser.setLanguage(Matlab);
var args = process.argv.slice(2);
if (args.length != 5) {
    process.exit(1);
}
// Display generated code on console
var show_output = parseInt(args[3]);
// Debug mode
var debug = parseInt(args[4]);
// Load the file passed as an argument
var sourceCode = fs.readFileSync(args[0], "utf8");
var tree = parser.parse(sourceCode);
// Read filenames in given directory
var search_folder = args[1];
//let classes = getClasses(search_folder, debug);
//console.log(classes);
// Output code to given directory
//let out_folder = args[2] + "/generatedCode";
var out_folder = args[2] + "/generatedCode/" + path.parse(args[0]).name;
if (!fs.existsSync(out_folder)) {
    fs.mkdirSync(out_folder);
}
// Makefile
if (!fs.existsSync("".concat(args[2], "/generatedCode/Makefile"))) {
    fs.copyFile('Makefile_template', "".concat(args[2], "/generatedCode/Makefile"), function (err) {
        if (err)
            throw err;
    });
}
if (show_output == 1) {
    console.log("Source code:\n" + sourceCode);
    console.log("---------------------\n");
}
if (!fs.existsSync("".concat(out_folder, "/matlabToRun"))) {
    fs.mkdirSync("".concat(out_folder, "/matlabToRun"));
}
else {
    fs.rmSync("".concat(out_folder, "/matlabToRun"), { recursive: true, force: true });
    fs.mkdirSync("".concat(out_folder, "/matlabToRun"));
}
var files = (0, helperFunctions_1.getNonClassFilesInPath)(search_folder);
files = files.filter(function (e) { return path.parse(e).name !== path.parse(args[0]).name; });
var var_types = [];
var _b = (0, identifyCustomFunctions_1.identifyCustomFunctions)(tree, [], files, args[0], [args[0]], debug), custom_functions = _b[0], file_traversal_order = _b[1];
if (show_output == 1) {
    console.log("File traversal order");
    console.log(file_traversal_order);
    console.log("---------------------\n");
}
//console.log("CUSTOMFUN");
//console.log(files);
//console.log(custom_functions);
var classes = (0, helperFunctions_1.getClasses)(search_folder, files, debug);
for (var i = 0; i < custom_functions.length; i++) {
    (0, helperFunctions_1.writeToFile)("".concat(out_folder, "/matlabToRun"), "".concat(custom_functions[i].name, ".m"), custom_functions[i].def_node.text);
}
for (var _i = 0, _c = file_traversal_order.reverse(); _i < _c.length; _i++) {
    var file = _c[_i];
    var sourceCode_1 = fs.readFileSync(file, "utf8");
    var tree_1 = parser.parse(sourceCode_1);
    var block_idxs = [];
    if (file == args[0]) {
        var filename = "main";
    }
    else {
        var filename = path.parse(file).name;
    }
    _a = (0, typeInference_1.typeInference)(filename, file, custom_functions, classes, debug), var_types = _a[0], custom_functions = _a[1], block_idxs = _a[2];
    var _d = (0, generateCode_1.generateCode)(filename, tree_1, out_folder, custom_functions, classes, var_types, block_idxs, file, debug), generated_code = _d[0], header = _d[1], vt = _d[2], cf = _d[3];
    var_types = vt;
    custom_functions = cf;
    if (show_output == 1) {
        console.log("---------------------\nCustom functions for ".concat(filename, ".c:\n"));
        console.log(custom_functions);
        console.log("---------------------\nInferred types for ".concat(filename, ".c:\n"));
        console.log(var_types);
        console.log("---------------------\nGenerated code for ".concat(filename, ".c:\n"));
        console.log(generated_code);
        console.log("---------------------\nGenerated code for ".concat(filename, ".h:\n"));
        console.log(header);
    }
}
//# sourceMappingURL=index.js.map