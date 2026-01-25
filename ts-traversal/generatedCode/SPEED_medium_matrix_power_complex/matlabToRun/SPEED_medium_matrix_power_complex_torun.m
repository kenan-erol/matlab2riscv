addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_power_complex/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_power_complex/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

iterations = 100;
exponent = 20.48 + I;

size = 100*100;

a = ones(100);

for n = 1:size
	a(n) = n^2+0.5;
end

for i = 1:iterations
	c = a^exponent;
	%dispArr(fileID, c);
end