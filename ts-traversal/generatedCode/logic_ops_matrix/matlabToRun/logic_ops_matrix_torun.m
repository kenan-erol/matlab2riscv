addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/logic_ops_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/logic_ops_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [0, 1, 0;
    1, 0, 1;
    0, 0, 0];

dispArr(fileID, a);

dispArr(fileID, ~a);

dispArr(fileID, ~zeros(3));
dispArr(fileID, ~ones(3));
dispArr(fileID, ~eye(3));

dispArr(fileID, a & zeros(3));
dispArr(fileID, a & ~eye(3));

dispArr(fileID, zeros(3) | ~ones(3));
dispArr(fileID, ~ones(3) | eye(3));
