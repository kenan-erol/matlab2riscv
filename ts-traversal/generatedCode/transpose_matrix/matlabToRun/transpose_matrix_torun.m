addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/transpose_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/transpose_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

% int_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter;
		counter = counter + 1;
	end
end

dispArr(fileID, a);

dispArr(fileID, a.');

b=a.';
dispArr(fileID, b.');

% double_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5;
		counter = counter + 1;
	end
end

dispArr(fileID, a);

dispArr(fileID, a.');

b=a.';
dispArr(fileID, b.');

% complex_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5i;
		counter = counter + 1;
	end
end

dispArr(fileID, a);

dispArr(fileID, a.');

b=a.';
dispArr(fileID, b.');

% complex_conjugate_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5i;
		counter = counter + 1;
	end
end

dispArr(fileID, a);

dispArr(fileID, a');

b=a';
dispArr(fileID, b');