addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/eigen/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/eigen/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

matrices = cell(15,1);

matrices{1} = zeros(3);
matrices{2} = 2*eye(3);
matrices{3} = eye(3);
matrices{4} = (4.2 - 0.03i)*eye(3);

matrices{5} = zeros(3);
for i=1:9
	matrices{5}(i) = i*i;
end
matrices{5} = matrices{5}.';


matrices{6} = zeros(3);
for i=1:9
	matrices{6}(i) = i*i+0.5;
end
matrices{6} = matrices{6}.';


matrices{7} = zeros(3);
for i=1:9
	matrices{7}(i) = i*i+0.5i;
end
matrices{7} = matrices{7}.';


matrices{8} = zeros(3);
for i=1:9
	matrices{8}(i) = (i-5)*i;
end
matrices{8} = matrices{8}.';


matrices{9} = zeros(3);
for i=1:9
	matrices{9}(i) = (i-8.2)*i+0.5;
end
matrices{9} = matrices{9}.';

matrices{10} = zeros(3);
for i=1:9
	matrices{10}(i) = (i-5.89)*(i)+((0.5)*(4-i))*1i;
end
matrices{10} = matrices{10}.';

matrices{11} = [3,-2,0;   4,-1,0;   0,0,1];

matrices{12} = [11.25, -7.525, -1.45;    11, -6.9, -2.2;    5.5, -5.45, 2.9];

matrices{13} = zeros(3);
for i=1:9
	matrices{13}(i) = (-1)^i*i*i;
end
matrices{13} = matrices{13}.';

% Non-diagonalizeable matrices
matrices{14} = [1,1,0;   0,1,0;   0,0,0];
matrices{15} = [3,4,3;   -1,0,-1;   1,2,3]; % Returns slightly different eigenvectors compared to the C output

for index=1:14
	dispArr(fileID, sprintf('Original\n'));
	dispArr(fileID, matrices{index});

	[V, lambda] = eig(matrices{index});

	dispArr(fileID, sprintf('Eigenvalues:\n'));
	dispArr(fileID, lambda);

	dispArr(fileID, sprintf('Eigenvectors:\n'));
	dispArr(fileID, V);
end