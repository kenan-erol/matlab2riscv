addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/create_one_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/create_one_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = ones(3,3);
b = ones(1,6);
c = ones(4,1);

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);