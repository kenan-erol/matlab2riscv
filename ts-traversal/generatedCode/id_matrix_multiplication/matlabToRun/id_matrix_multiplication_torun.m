addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/id_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/id_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [1,4;9,16;25,36];
b = [2.1,0;0,2.1;0,0].';
c = a*b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);