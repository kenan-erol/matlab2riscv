addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/xcorrM/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/xcorrM/output.txt','w');
%more off
%format short

%source octaveIncludes.m;
%pkg load signal;

matrices = cell(12,1);

matrices{1} = zeros(1,10);
matrices{2} = ones(20,1);
matrices{3} = ones(1,10);
matrices{4} = (4.5 - 0.5i)*ones(20,1);

matrices{5} = zeros(1,10);
for i=1:10
	matrices{5}(i) = i*i;
end
matrices{5} = matrices{5};


matrices{6} = zeros(1,10);
for i=1:10
	matrices{6}(i) = i*i+0.5;
end
matrices{6} = matrices{6};


matrices{7} = ones(20,1);
for i=1:20
	matrices{7}(i) = i*i+0.5i;
end
matrices{7} = matrices{7};


matrices{8} = ones(20,1);
for i=1:20
	matrices{8}(i) = (i-5)*i;
end
matrices{8} = matrices{8};


matrices{9} = ones(20,1);
for i=1:20
	matrices{9}(i) = (i-8.5)*i+0.5;
end
matrices{9} = matrices{9};

matrices{10} = zeros(1,10);
for i=1:10
	matrices{10}(i) = (i-5.5)*(i)+((0.5)*(4-i))*1i;
end
matrices{10} = matrices{10};

matrices{11} = [3,-2,0,   4,-1,0,   0,0,1, 2.5];

matrices{12} = [3,-2,0,   4,-1,0,   0,0,1, 2.5];


for index=6:6
	dispArr(fileID, sprintf('i = %d\n', index));
	dispArr(fileID, matrices{index});

	for j=11:11
		dispArr(fileID, sprintf('j = %d\n', j));
		dispArr(fileID, matrices{j});
		dispArr(fileID, sprintf('\n\n'));
		dispArr(fileID, xcorr(matrices{index}, matrices{j}, 'none'));
		if (index > 1 && j > 1 && size(matrices{index},1)*size(matrices{index},2) == size(matrices{j},1)*size(matrices{j},2))
			dispArr(fileID, xcorr(matrices{index}, matrices{j}, 'unbiased'));
			dispArr(fileID, xcorr(matrices{index}, matrices{j}, 'biased'));
			dispArr(fileID, xcorr(matrices{index}, matrices{j}, 'coeff'));
		end

		%for k=9:21
		%	dispArr(fileID, xcorr(matrices{index}, matrices{j}, k, 'none'));
		%	if (index > 1 && j > 1 && size(matrices{index},1)*size(matrices{index},2) == size(matrices{j},1)*size(matrices{j},2))
		%		dispArr(fileID, xcorr(matrices{index}, matrices{j}, k, 'unbiased'));
		%		dispArr(fileID, xcorr(matrices{index}, matrices{j}, k, 'biased'));
		%		dispArr(fileID, xcorr(matrices{index}, matrices{j}, k, 'coeff'));
		%	end
		%end

	end
end