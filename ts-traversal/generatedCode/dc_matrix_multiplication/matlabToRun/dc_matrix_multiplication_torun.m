addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/dc_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/dc_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [1.5,4.5;9.5,16.5];
b = [2.1+0.5i,0;0,2.1+0.5i];
c = a*b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);