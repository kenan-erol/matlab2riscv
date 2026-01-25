addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/ceil_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/ceil_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [0, 10, 10i; 
10.102, 10.102+0.5i, -12i; 
-0.0002-0.1i, -100.01i, 81];

b = ceil(a);

dispArr(fileID, a);
dispArr(fileID, b);