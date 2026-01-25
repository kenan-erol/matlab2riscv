var fs = require('fs');
var path = require("path");
let OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
import { writeToFile } from "./helperFunctions";

/*const args = process.argv.slice(2);

if (args.length != 2) {
    process.exit(1);
}*/


if (fs.existsSync(`${OCTAVEC}/matrix.c`)){
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${OCTAVEC}/matrix.c`, "utf8");
        
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, "#include <cblas.h>");
        code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, "#include <lapacke.h>");
        
        writeToFile(OCTAVEC, "matrix.c", code);
        
    }, 8000);
}
if (fs.existsSync(`${OCTAVEC}/matrix.h`)){
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${OCTAVEC}/matrix.h`, "utf8");
        
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, "#include <cblas.h>");
        code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, "#include <lapacke.h>");
        
        writeToFile(OCTAVEC, "matrix.h", code);
        
    }, 8000);
}

