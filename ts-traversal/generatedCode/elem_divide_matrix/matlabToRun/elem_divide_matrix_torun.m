addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/elem_divide_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/elem_divide_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = ones(3);
a(1) = -0.75 + 1i;
a(5) = -0.75 + 1i;
a(9) = -0.75 + 1i;
a=a.';
dispArr(fileID, a);

b = ones(3);
b(1) = 0.5 + 1i;
b(5) = 0.5 + 1i;
b(9) = 0.5 + 1i;
b=b.';

dispArr(fileID, b);

c = a./b;
dispArr(fileID, c);

d = eye(3)./a;
dispArr(fileID, d);

e = b./eye(3);
dispArr(fileID, e);

c = a.\b;
dispArr(fileID, c);

d = eye(3).\a;
dispArr(fileID, d);

e = b.\eye(3);
dispArr(fileID, e);