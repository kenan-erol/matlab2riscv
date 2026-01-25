addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/subtract_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/subtract_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = 0.5*ones(3);
a(1) = a(1) + 1i;
a(5) = a(5) + 1i;
a(9) = a(9) + 1i;
a = a.';
dispArr(fileID, a);


b = 0.5*ones(3);
b(1) = -0.5 + 1i;
b(5) = -0.5 + 1i;
b(9) = -0.5 + 1i;
b = b.';

dispArr(fileID, b);

c = a-b;
dispArr(fileID, c);

d = eye(3)-a;
dispArr(fileID, d);

%e = INT_MIN*eye(3)-eye(3);
%dispArr(fileID, e);