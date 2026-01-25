addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/comparison_ops_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/comparison_ops_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

matrices = cell(12,1);

matrices{1} = zeros(3);
matrices{2} = ones(3);
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


for i=1:12
	for j=1:12
	    dispArr(fileID, sprintf("i = %d, j = %d\n", i, j));
		dispArr(fileID, matrices{i} < matrices{j});
		dispArr(fileID, matrices{i} <= matrices{j});
		dispArr(fileID, matrices{i} > matrices{j});
		dispArr(fileID, matrices{i} >= matrices{j});
		dispArr(fileID, matrices{i} ~= matrices{j});
		dispArr(fileID, matrices{i} == matrices{j});
        dispArr(fileID, max(matrices{i} , matrices{j}));
		dispArr(fileID, min(matrices{i} , matrices{j}));
		if (i == 4 || i == 7 || i == 10 || j == 4 || j == 7 || j == 10)
			dispArr(fileID, max(matrices{i} , matrices{j}));
			%dispArr(fileID, min(matrices{i} , matrices{j}));
		elseif (i == 6 || i == 9 || i == 12 || j == 6 || j == 9 || j == 12)
			%dispArr(fileID, max(matrices{i} , matrices{j}));
			dispArr(fileID, min(matrices{i} , matrices{j}));
		else
			dispArr(fileID, max(matrices{i} , matrices{j}));
			dispArr(fileID, min(matrices{i} , matrices{j}));
		end
		dispArr(fileID, sprintf("\n-------------------------\n"));
	end
end