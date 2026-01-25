addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/varargin_test/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/varargin_test/output.txt','w');
A = [1, 2, 3];
B = myfun(fileID, A, 3, A, 4.1);

B = myfun(fileID, A, 2);

