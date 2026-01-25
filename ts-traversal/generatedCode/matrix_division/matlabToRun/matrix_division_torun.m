addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/matrix_division/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/matrix_division/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%ldivide_test1
a = [1,4;9,16];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%ldivide_test2
a = eye(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%ldivide_test3
a = [2.1+0.5*1i,0;0,2.1+0.5*1i];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%rdivide_test1
a = [1,4;9,16];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%rdivide_test2
a = eye(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%rdivide_test3
a = [2.1+0.5*1i,0;0,2.1+0.5*1i];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

dispArr(fileID, a);
dispArr(fileID, b);
dispArr(fileID, c);

%divide_by_zero
a = zeros(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = b/a;

dispArr(fileID, a);
dispArr(fileID, b);