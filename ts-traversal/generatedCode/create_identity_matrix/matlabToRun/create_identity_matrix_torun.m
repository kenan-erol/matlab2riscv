addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/create_identity_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/create_identity_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = eye(8);
b = eye(4);
c = eye(1);

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);