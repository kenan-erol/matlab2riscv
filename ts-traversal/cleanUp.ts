var fs = require('fs');
var path = require("path");
let OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
import { writeToFile } from "./helperFunctions";

const args = process.argv.slice(2);

if (args.length != 2) {
    process.exit(1);
}

let mfile = args[0];
let name = path.parse(mfile).name;
//let out_folder = args[1] + "/generatedCode/" + name;
let out_folder = args[1] + "/generatedCode/" + name;
if (!fs.existsSync(out_folder)){
fs.mkdirSync(out_folder);
}

if (!fs.existsSync(`${out_folder}/${name}.c`) && fs.existsSync(`${OCTAVEC}/tests_C_Octave/${name}/${name}.c`)){
    //fs.copyFile(`${OCTAVEC}/tests/${name}.c`, `${out_folder}/octavec_main.c`, (err) => {
    fs.copyFile(`${OCTAVEC}/tests_C_Octave/${name}/${name}.c`, `${out_folder}/octavec_main.c`, (err) => {
        if (err) throw err;
    });
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${out_folder}/octavec_main.c`, "utf8");
        
        code = code.replace('#include "../matrix.h"', '#include <matrix.h>');
        code = code.replace('#include "../../matrix.h"', '#include <matrix.h>');
        
        
        writeToFile(out_folder, "octavec_main.c", code);
        
    }, 8000);
}

if (!fs.existsSync(`${out_folder}/${mfile}`)){
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile(`${OCTAVEC}/tests_C_Octave/${name}/${mfile}`, `${out_folder}/${mfile}`, (err) => {
        if (err) throw err;
    });
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${out_folder}/${mfile}`, "utf8");
    
        // Comment out directives
        let comment_lines = ["more", "format", "source"];
        for (let comment_line of comment_lines) {
            //let re = new RegExp(`^(${comment_line}|\n${comment_line})\\s[\\w;\.]*\\n`, "g");
            let re = new RegExp(`${comment_line}\\s[\\w;\.]*\\n`, "g");
            let match = code.match(re);
            if (match != null) {
                code = code.replace(re, `%${match}`);
            }
        }
        
        // Replace binary operators in complex numbers
        //code = code.replace(/(\*i)|(\*I)|/g, 'i');
        code = code.replace(/\)\*I/g, ')*1i');
        code = code.replace(/\*I/g, 'i');
        //code = code.replace(/\*i/g, 'i');
        
        // Replace augmented assignment
        let idx = code.indexOf("++");
        while (idx != -1) {
            let tmp = code.slice(0,idx).split(/[\s,\n]/);
            let variable = tmp.splice(-1);
            code = code.replace(`${variable}++`, `${variable} = ${variable} + 1`);
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
        
        writeToFile(out_folder, mfile, code);
        
    }, 8000);
    
}

/*if (!fs.existsSync(`${out_folder}/${name}_torun.m`)){
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile(`${out_folder}/${mfile}`, `${out_folder}/${name}_torun.m`, (err) => {
        if (err) throw err;
    });
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${out_folder}/${name}_torun.m`, "utf8");
        let expression = [];
        expression.push("addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');")
        expression.push(`fileID = fopen('${out_folder}/output.txt','w');\n`);
        code = expression.join("\n") + code;
        
        // Replace disp with dispArr
        code = code.replace(/disp\(/g, 'dispArr(fileID, ');
        
        writeToFile(out_folder, `${name}_torun.m`, code);
        
    }, 8000);
    
}*/

