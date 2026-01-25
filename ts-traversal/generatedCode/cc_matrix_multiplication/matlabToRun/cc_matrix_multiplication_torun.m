addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/cc_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/cc_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [1+1i,0.4i;9-0.5i,16;10+17i,35-9i];
b = [2,0;0,1i;10+17i,35-9i].';
c = a*b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);