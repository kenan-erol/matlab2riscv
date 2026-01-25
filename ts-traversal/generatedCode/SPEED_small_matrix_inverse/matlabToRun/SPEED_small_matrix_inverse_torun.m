addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_small_matrix_inverse/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_small_matrix_inverse/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

iterations = 1000000;

a = [1.5,4.5;9.5,16.5];

dispArr(fileID, a);

for i = 1:iterations
	b = inv(a);
end