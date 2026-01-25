var fs = require('fs');
var path = require("path");
let OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
import { writeToFile, waitForFileExists } from "./helperFunctions";

let code = fs.readFileSync(`${OCTAVEC}/matrix.c`, "utf8");

code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, '#include <cblas.h>');
code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, '#include <lapacke.h>');
code = code.replace(/#include "fftw-3.3.9\/api\/fftw3.h"/g, '#include <fftw3.h>');

writeToFile(OCTAVEC, "matrix.c", code);

code = fs.readFileSync(`${OCTAVEC}/matrix.h`, "utf8");

code = code.replace(/#include "CBLAS\/include\/cblas.h"/g, '#include <cblas.h>');
code = code.replace(/#include "lapack-master\/LAPACKE\/include\/lapacke.h"/g, '#include <lapacke.h>');
code = code.replace(/#include "fftw-3.3.9\/api\/fftw3.h"/g, '#include <fftw3.h>');

writeToFile(OCTAVEC, "matrix.h", code);