addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/scale_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/scale_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

dispArr(fileID, ones(3));

a = zeros(3);

for i=1:9
	a(i) = i;
end
a = a.';
dispArr(fileID, a);

b = zeros(3);

for i=1:9
	b(i) = i+i*1i;
end
b = b.';
dispArr(fileID, b);

dispArr(fileID, 2*ones(3));
dispArr(fileID, 2.1*ones(3));
dispArr(fileID, (2.1+1i)*ones(3));

dispArr(fileID, 2*a);
dispArr(fileID, 2.1*a);
dispArr(fileID, (2.1+1i)*a);

dispArr(fileID, 2*b);
dispArr(fileID, 2.1*b);
dispArr(fileID, (2.1+1i)*b);

%dispArr(fileID, 2*INT_MAX*ones(3));
%dispArr(fileID, 2*INT_MIN*ones(3));