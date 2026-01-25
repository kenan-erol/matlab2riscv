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
//let out_folder = args[1] + "/generatedCode/" + name;
var out_folder = args[1] + "/generatedCode/" + name;
if (!fs.existsSync(out_folder)) {
    fs.mkdirSync(out_folder);
}
if (!fs.existsSync("".concat(out_folder, "/").concat(name, ".c")) && fs.existsSync("".concat(OCTAVEC, "/tests_C_Octave/").concat(name, "/").concat(name, ".c"))) {
    //fs.copyFile(`${OCTAVEC}/tests/${name}.c`, `${out_folder}/octavec_main.c`, (err) => {
    fs.copyFile("".concat(OCTAVEC, "/tests_C_Octave/").concat(name, "/").concat(name, ".c"), "".concat(out_folder, "/octavec_main.c"), function (err) {
        if (err)
            throw err;
    });
    setTimeout(function () {
        var code = fs.readFileSync("".concat(out_folder, "/octavec_main.c"), "utf8");
        code = code.replace('#include "../matrix.h"', '#include <matrix.h>');
        code = code.replace('#include "../../matrix.h"', '#include <matrix.h>');
        (0, helperFunctions_1.writeToFile)(out_folder, "octavec_main.c", code);
    }, 8000);
}
if (!fs.existsSync("".concat(out_folder, "/").concat(mfile))) {
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile("".concat(OCTAVEC, "/tests_C_Octave/").concat(name, "/").concat(mfile), "".concat(out_folder, "/").concat(mfile), function (err) {
        if (err)
            throw err;
    });
    setTimeout(function () {
        var code = fs.readFileSync("".concat(out_folder, "/").concat(mfile), "utf8");
        // Comment out directives
        var comment_lines = ["more", "format", "source"];
        for (var _i = 0, comment_lines_1 = comment_lines; _i < comment_lines_1.length; _i++) {
            var comment_line = comment_lines_1[_i];
            //let re = new RegExp(`^(${comment_line}|\n${comment_line})\\s[\\w;\.]*\\n`, "g");
            var re = new RegExp("".concat(comment_line, "\\s[\\w;.]*\\n"), "g");
            var match = code.match(re);
            if (match != null) {
                code = code.replace(re, "%".concat(match));
            }
        }
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/\)\*I/g, ')*1i');
        code = code.replace(/\*I/g, 'i');
        //code = code.replace(/\*i/g, 'i');
        // Replace augmented assignment
        var idx = code.indexOf("++");
        while (idx != -1) {
            var tmp = code.slice(0, idx).split(/[\s,\n]/);
            var variable = tmp.splice(-1);
            code = code.replace("".concat(variable, "++"), "".concat(variable, " = ").concat(variable, " + 1"));
            idx = code.indexOf("++");
        }
        // Replace complexDisp and doubleDisp
        code = code.replace(/complexDisp/g, 'disp');
        code = code.replace(/doubleDisp/g, 'disp');
        code = code.replace(/intDisp/g, 'disp');
        code = code.replace(/\bprintf\b/g, 'sprintf');
        code = code.replace(/\bfprintf\b/g, 'sprintf');
        code = code.replace(/fdisp/g, 'sprintf');
        //code = code.replace(/printf/g, 'disp');
        // Replace endfor, endif and endfunction
        code = code.replace(/endfor/g, 'end');
        code = code.replace(/endif/g, 'end');
        code = code.replace(/endfunction/g, 'end');
        // Replace tic with tic() and toc with toc() 
        // so parser recognizes as calls
        code = code.replace(/tic/g, 'tic()');
        code = code.replace(/toc/g, 'toc()');
        (0, helperFunctions_1.writeToFile)(out_folder, mfile, code);
    }, 8000);
}
if (!fs.existsSync("".concat(out_folder, "/").concat(name, "_torun.m"))) {
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile("".concat(out_folder, "/").concat(mfile), "".concat(out_folder, "/").concat(name, "_torun.m"), function (err) {
        if (err)
            throw err;
    });
    setTimeout(function () {
        var code = fs.readFileSync("".concat(out_folder, "/").concat(name, "_torun.m"), "utf8");
        var expression = [];
        expression.push("addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');");
        expression.push("fileID = fopen('".concat(out_folder, "/output.txt','w');\n"));
        code = expression.join("\n") + code;
        // Replace disp with dispArr
        code = code.replace(/disp\(/g, 'dispArr(fileID, ');
        (0, helperFunctions_1.writeToFile)(out_folder, "".concat(name, "_torun.m"), code);
    }, 8000);
}
//# sourceMappingURL=cleanUp.js.map