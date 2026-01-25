"use strict";
exports.__esModule = true;
var fs = require('fs');
var path = require("path");
var OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
var helperFunctions_1 = require("./helperFunctions");
/*const args = process.argv.slice(2);

if (args.length != 2) {
    process.exit(1);
}*/
if (fs.existsSync("".concat(OCTAVEC, "/matrix.c"))) {
    setTimeout(function () {
        var code = fs.readFileSync("".concat(OCTAVEC, "/matrix.c"), "utf8");
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, "#include <cblas.h>");
        code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, "#include <lapacke.h>");
        (0, helperFunctions_1.writeToFile)(OCTAVEC, "matrix.c", code);
    }, 8000);
}
if (fs.existsSync("".concat(OCTAVEC, "/matrix.h"))) {
    setTimeout(function () {
        var code = fs.readFileSync("".concat(OCTAVEC, "/matrix.h"), "utf8");
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, "#include <cblas.h>");
        code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, "#include <lapacke.h>");
        (0, helperFunctions_1.writeToFile)(OCTAVEC, "matrix.h", code);
    }, 8000);
}
//# sourceMappingURL=modifyHalo.js.map