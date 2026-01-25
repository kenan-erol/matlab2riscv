addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/cell_test/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/cell_test/output.txt','w');
A = 1;
B = {2.0, 3 + 5i};
C = {'hello world', 1, 2.0};

for i = 1:3
    dispArr(fileID, C{i});
end
%dispArr(fileID, C{1});