var fs = require('fs');
var path = require("path");
let OCTAVEC = "/home/dlg59/project/Halo-Algorithm/OctaveC";
import { writeToFile, getFilesInPath } from "./helperFunctions";

const args = process.argv.slice(2);

if (args.length != 2) {
    process.exit(1);
}

let mfile = args[0];
let name = path.parse(mfile).name;
let out_folder = `${args[1]}/generatedCode/${name}`;
/*if (!fs.existsSync(`${out_folder}/matlabToRun`)){
    fs.mkdirSync(`${out_folder}/matlabToRun`);
}*/

if (!fs.existsSync(`${out_folder}/matlabToRun/${name}_torun.m`)){
    let custom_fun = getFilesInPath(`${out_folder}/matlabToRun`);
    
    //fs.copyFile(`${OCTAVEC}/tests/${mfile}`, `${out_folder}/${mfile}`, (err) => {
    fs.copyFile(`${out_folder}/${mfile}`, `${out_folder}/matlabToRun/${name}_torun.m`, (err) => {
        if (err) throw err;
    });
    
    setTimeout(function () {
        
        let code = fs.readFileSync(`${out_folder}/matlabToRun/${name}_torun.m`, "utf8");
        let expression = [];
        expression.push("addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');")
        expression.push(`addpath('${out_folder}/matlabToRun');`);
        expression.push(`fileID = fopen('${out_folder}/output.txt','w');\n`);
        code = expression.join("\n") + code;
        
        let fun_defs = [];
        for (let i = 0; i < custom_fun.length; i++) {
            let fun_code = fs.readFileSync(custom_fun[i], "utf8");
            code = code.replace(fun_code, '');
            // Replace disp with dispArr
            fun_code = fun_code.replace(/disp\(/g, 'dispArr(fileID, ');
            // Replace nargin with (nargin - 1) because of fileID param
            fun_code = fun_code.replace(/nargin/g, '(nargin - 1)');
            fun_defs.push(fun_code);
        }
        
        // Replace disp with dispArr
        code = code.replace(/disp\(/g, 'dispArr(fileID, ');
        
        for (let i = 0; i < custom_fun.length; i++) {
            let fun_name = path.parse(custom_fun[i]).name;
            let re = new RegExp(`${fun_name}\\(`, 'g');
            code = code.replace(re, `${fun_name}(fileID, `);
            for (let j = 0; j < custom_fun.length; j++) {
                fun_defs[j] = fun_defs[j].replace(re, `${fun_name}(fileID, `);
            }
        }
        
        for (let i = 0; i < fun_defs.length; i++) {
            let fun_name = path.parse(custom_fun[i]).name;
            writeToFile(`${out_folder}/matlabToRun`, `${fun_name}.m`, fun_defs[i]);
        }
        
        writeToFile(`${out_folder}/matlabToRun`, `${name}_torun.m`, code);
        
    //}, 8000);
    }, 20000);
    
}

