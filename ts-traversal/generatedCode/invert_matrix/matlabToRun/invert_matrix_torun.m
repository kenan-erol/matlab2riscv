addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/invert_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/invert_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%int_test
a = [1,2;3,4];
dispArr(fileID, a);
dispArr(fileID, inv(a));
dispArr(fileID, inv(inv(a)));

%double_test
a = [1.5,2.5;3.5,4.5];
dispArr(fileID, a);
dispArr(fileID, inv(a));
dispArr(fileID, inv(inv(a)));

%complex_test
a = [1.5+1i,2.5+2i;3.5+3i,4.5+4i];
dispArr(fileID, a);
dispArr(fileID, inv(a));
dispArr(fileID, inv(inv(a)));

%singular_test
dispArr(fileID, ones(2));

%non_square_test
dispArr(fileID, ones(2,3));
