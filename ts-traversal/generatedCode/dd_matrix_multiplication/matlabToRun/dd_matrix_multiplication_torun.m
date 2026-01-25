addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/dd_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/dd_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [1,4;9,16];
b = [2,0;0,2];
c = a*b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);