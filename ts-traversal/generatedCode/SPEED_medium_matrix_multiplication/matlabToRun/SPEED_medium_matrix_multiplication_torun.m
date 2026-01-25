addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

iterations = 100;

size = 100*100;

a = ones(100);
b = ones(100);

for n = 1:size
	a(n) = n^2+0.5;
	b(n) = (n^2+0.5)*1i;
end

for i = 1:iterations
	c = b*a;
	%dispArr(fileID, c);
end