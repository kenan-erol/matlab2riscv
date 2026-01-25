"use strict";
exports.__esModule = true;
var fs = require('fs');
var path = require("path");
var OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
var helperFunctions_1 = require("./helperFunctions");
var args = process.argv.slice(2);
if (args.length != 2) {
    process.exit(1);
}
var mfile = args[0];
var name = path.parse(mfile).name;
var out_folder = "".concat(args[1], "/generatedCode/").concat(name);
/*if (!fs.existsSync(`${out_folder}/matlabToRun`)){
    fs.mkdirSync(`${out_folder}/matlabToRun`);
}*/
if (!fs.existsSync("".concat(out_folder, "/matlabToRun/").concat(name, "_torun.m"))) {
    var custom_fun_1 = (0, helperFunctions_1.getFilesInPath)("".concat(out_folder, "/matlabToRun"));
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile("".concat(out_folder, "/").concat(mfile), "".concat(out_folder, "/matlabToRun/").concat(name, "_torun.m"), function (err) {
        if (err)
            throw err;
    });
    setTimeout(function () {
        var code = fs.readFileSync("".concat(out_folder, "/matlabToRun/").concat(name, "_torun.m"), "utf8");
        var expression = [];
        expression.push("addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');");
        expression.push("addpath('".concat(out_folder, "/matlabToRun');"));
        expression.push("fileID = fopen('".concat(out_folder, "/output.txt','w');\n"));
        code = expression.join("\n") + code;
        var fun_defs = [];
        for (var i = 0; i < custom_fun_1.length; i++) {
            var fun_code = fs.readFileSync(custom_fun_1[i], "utf8");
            code = code.replace(fun_code, '');
            // Replace disp with dispArr
            fun_code = fun_code.replace(/disp\(/g, 'dispArr(fileID, ');
            // Replace nargin with (nargin - 1) because of fileID param
            fun_code = fun_code.replace(/nargin/g, '(nargin - 1)');
            fun_defs.push(fun_code);
        }
        // Replace disp with dispArr
        code = code.replace(/disp\(/g, 'dispArr(fileID, ');
        for (var i = 0; i < custom_fun_1.length; i++) {
            var fun_name = path.parse(custom_fun_1[i]).name;
            var re = new RegExp("".concat(fun_name, "\\("), 'g');
            code = code.replace(re, "".concat(fun_name, "(fileID, "));
            for (var j = 0; j < custom_fun_1.length; j++) {
                fun_defs[j] = fun_defs[j].replace(re, "".concat(fun_name, "(fileID, "));
            }
        }
        for (var i = 0; i < fun_defs.length; i++) {
            var fun_name = path.parse(custom_fun_1[i]).name;
            (0, helperFunctions_1.writeToFile)("".concat(out_folder, "/matlabToRun"), "".concat(fun_name, ".m"), fun_defs[i]);
        }
        (0, helperFunctions_1.writeToFile)("".concat(out_folder, "/matlabToRun"), "".concat(name, "_torun.m"), code);
        //}, 8000);
    }, 20000);
}
//# sourceMappingURL=matlabToRun.js.map