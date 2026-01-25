addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/areEqual_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/areEqual_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

% trueTest
a = zeros(3);

for i=1:9
	a(i) = i*i;
end
a = a.';

dispArr(fileID, a);

b=a;
dispArr(fileID, b);

c=a;
dispArr(fileID, c);

d=a;
dispArr(fileID, d);

dispArr(fileID, (a==b)&(a==c)&(a==d));

% falseTest
dispArr(fileID, a);
dispArr(fileID, b);

c = zeros(3);
for i=1:9
	c(i) = i*i;
end
c(2) = 10;
c(3) = 11;
c(6) = 12;
c = c.';
dispArr(fileID, c);

d = zeros(3);
for i=1:9
	d(i) = i*i;
end
d(4) = 13;
d(7) = 14;
d(8) = 15;
d = d.';
dispArr(fileID, d);

dispArr(fileID, (a==b)&(a==c)&(a==d));