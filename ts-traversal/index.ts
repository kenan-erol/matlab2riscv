require('source-map-support').install()
//const fs = require("graceful-fs");
var fs = require('fs');
var path = require("path");
var gracefulFs = require('graceful-fs');
gracefulFs.gracefulify(fs);
import { generateCode } from "./generateCode";
import { getFilesInPath, getNonClassFilesInPath, getClasses, writeToFile } from "./helperFunctions";
import * as g from "./generated";
import { CustomFunction, VarType, Type } from "./customTypes";
import { identifyCustomFunctions } from "./identifyCustomFunctions";
import { typeInference, inferType } from "./typeInference";
import Parser = require("tree-sitter");
import Matlab = require("tree-sitter-matlab");
//import Matlab = require("/gpfs/gibbs/project/manohar/dlg59/ts-traversal/node_modules/tree-sitter-matlab");

let parser = new Parser() as g.Parser;
parser.setLanguage(Matlab);

const args = process.argv.slice(2);

if (args.length != 5) {
    process.exit(1);
}

// Display generated code on console
let show_output = parseInt(args[3]);

// Debug mode
let debug = parseInt(args[4]);

// Load the file passed as an argument
const sourceCode = fs.readFileSync(args[0], "utf8");
let tree = parser.parse(sourceCode);

// Read filenames in given directory
const search_folder = args[1];
//let classes = getClasses(search_folder, debug);
//console.log(classes);

// Output code to given directory
//let out_folder = args[2] + "/generatedCode";
let out_folder = args[2] + "/generatedCode/" + path.parse(args[0]).name;
if (!fs.existsSync(out_folder)){
fs.mkdirSync(out_folder);
}

// Makefile
if (!fs.existsSync(`${args[2]}/generatedCode/Makefile`)){
    fs.copyFile('Makefile_template', `${args[2]}/generatedCode/Makefile`, (err) => {
        if (err) throw err;
    });
}

if (show_output==1) {
    console.log("Source code:\n" + sourceCode);
    console.log("---------------------\n");
}


if (!fs.existsSync(`${out_folder}/matlabToRun`)){
    fs.mkdirSync(`${out_folder}/matlabToRun`);
} else {
    fs.rmSync(`${out_folder}/matlabToRun`, { recursive: true, force: true });
    fs.mkdirSync(`${out_folder}/matlabToRun`);
}


var files = getNonClassFilesInPath(search_folder);
files = files.filter(function(e) { return path.parse(e).name !== path.parse(args[0]).name });


var var_types: VarType[] = [];
var [custom_functions, file_traversal_order] = identifyCustomFunctions(tree, [], files, args[0], [args[0]], debug);
if (show_output==1) {
    console.log("File traversal order");
    console.log(file_traversal_order);
    console.log("---------------------\n");
}
//console.log("CUSTOMFUN");
//console.log(files);
//console.log(custom_functions);
let classes = getClasses(search_folder, files, debug);

for (let i = 0; i < custom_functions.length; i ++) {
    writeToFile(`${out_folder}/matlabToRun`, `${custom_functions[i].name}.m`, custom_functions[i].def_node.text);
}

for (let file of file_traversal_order.reverse()) {
    const sourceCode = fs.readFileSync(file, "utf8");
    let tree = parser.parse(sourceCode);
    let block_idxs = [];
    if (file == args[0]) {
        var filename = "main";
    } else {
        var filename:string = path.parse(file).name;
    }

    [var_types, custom_functions, block_idxs] = typeInference(filename, file, custom_functions, classes, debug);

    let [generated_code, header, vt, cf] = generateCode(filename, tree, out_folder, custom_functions, classes, var_types, block_idxs, file, debug);
    var_types = vt;
    custom_functions = cf;
    
    if (show_output==1) {
        console.log(`---------------------\nCustom functions for ${filename}.c:\n`);
        console.log(custom_functions);
        console.log(`---------------------\nInferred types for ${filename}.c:\n`);
        console.log(var_types);
        console.log(`---------------------\nGenerated code for ${filename}.c:\n`);
        console.log(generated_code);
        console.log(`---------------------\nGenerated code for ${filename}.h:\n`);
        console.log(header);
    }
}

