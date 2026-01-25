addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/elem_trig_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/elem_trig_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%int_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i-1);
end
a=a.';
dispArr(fileID, a);

dispArr(fileID, sin(a));
dispArr(fileID, sind(a));
dispArr(fileID, cos(a));
dispArr(fileID, cosd(a));
dispArr(fileID, tan(a));
dispArr(fileID, tand(a));


%double_test
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
dispArr(fileID, a);

dispArr(fileID, sin(a));
dispArr(fileID, sind(a));
dispArr(fileID, cos(a));
dispArr(fileID, cosd(a));
dispArr(fileID, tan(a));
dispArr(fileID, tand(a));


%complex_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = i + 0.5i;
end
a=a.';
dispArr(fileID, a);

dispArr(fileID, sin(a));
dispArr(fileID, sind(a));
dispArr(fileID, cos(a));
dispArr(fileID, cosd(a));
dispArr(fileID, tan(a));
dispArr(fileID, tand(a));