addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/octaveTimer/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/octaveTimer/output.txt','w');
%more off
%format short

tic;
for j=1:5
	dispArr(fileID, j);
end

a = toc;

dispArr(fileID, a);
