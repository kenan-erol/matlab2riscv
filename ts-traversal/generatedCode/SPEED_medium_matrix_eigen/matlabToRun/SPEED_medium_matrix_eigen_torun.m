addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_eigen/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/SPEED_medium_matrix_eigen/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

iterations = 100;

size = 100;

a = ones(100);

for n = 1:size
	for m = 1:size
		a(n,m) = ((n-1)*size + m)^2.1 + 0.5 + mod(((n-1)*size + m), 7);
	end
end

for i = 1:iterations
	[V, lambda] = eig(a);
end
