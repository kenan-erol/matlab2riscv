addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_big_matrix_multiplication/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_big_matrix_multiplication/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

iterations = 1;

size = 1000*1000;

a = ones(1000);
b = ones(1000);

for n = 1:size
	a(n) = n^2+0.5;
	b(n) = (n^2+0.5)*1i;
end

for i = 1:iterations
	c = b*a;
	%dispArr(fileID, c);
end