addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/determinant_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/determinant_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%test1
a=[1];
dispArr(fileID, a);
dispArr(fileID, det(a));

%test2
a = [26 + 1i, 3 - 8i; 20i, 1 + 25i];
dispArr(fileID, a);
dispArr(fileID, det(a));

%test3
a = zeros(3);
counter=1;
for i=1:3
	for j=1:3
		a(i,j) = counter*counter;
		counter = counter + 1;
	end
end
dispArr(fileID, a);
dispArr(fileID, det(a));

%testn
a = zeros(5);
counter = 0;
for i=0:4
	for j=0:4
		counter = counter + 1;
		if (mod((counter-1),2) == 0)
			a(counter) = mod((counter + i),7);
		else
			a(counter) = -1*mod((counter + j),7);
		end
	end
end
a=a.';
dispArr(fileID, a);
dispArr(fileID, det(a));

%non_square
a = zeros(3,2);
a(1) = 26 + 1i;
a(2) = 3 - 8i;
a(3) = 20i;
a(4) = 1 + 25i;
a(5) = 0;
a(6) = 1;
a=a.';
dispArr(fileID, a);