addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/elem_multiply_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/elem_multiply_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = (0.5 + 1i)*eye(3);
dispArr(fileID, a);

b = (0.4 - 0.8i)*eye(3);
b = b.';

dispArr(fileID, b);

c = a.*b;
dispArr(fileID, c);

d = eye(3).*a;
dispArr(fileID, d);

e = 2*eye(3);
dispArr(fileID, e);