addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_small_matrix_power_double/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_small_matrix_power_double/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

exponent = 10.5;
iterations = 100000;

b = [2.1+0.5i,1;1,2.1+0.5i];
dispArr(fileID, b);

c = b^exponent;
dispArr(fileID, c);

for i = 1:iterations
	c = b^exponent;
	%dispArr(fileID, c);
end
