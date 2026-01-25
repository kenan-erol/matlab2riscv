addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/create_1D_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/create_1D_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [1; 4; 9.5];

dispArr(fileID, a);