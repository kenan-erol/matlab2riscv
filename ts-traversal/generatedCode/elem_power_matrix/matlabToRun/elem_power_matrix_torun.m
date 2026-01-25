addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/elem_power_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/elem_power_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%ii_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
dispArr(fileID, a);

b = a;
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%id_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%neg_id_test
a = zeros(3);
for i=1:9
	a(i) = -i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%ic_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%di_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%dd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%neg_dd_test
a = zeros(3);
for i=1:9
	a(i) = -(i+0.4);
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = -(i+0.4);
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%dc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%ci_test
a = zeros(3);
for i=1:9
	a(i) = i+1i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%cd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.5i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%cc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4i;
end
a=a.';
dispArr(fileID, a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
dispArr(fileID, b);

c = a.^b;
dispArr(fileID, c);

%overflow_test
%a = zeros(3);
%for i=1:9
%	a(i) = intmax;
%end
%a=a.';
%dispArr(fileID, a);

%b = 2*ones(3);
%dispArr(fileID, a);

%c = a.^b;
%dispArr(fileID, c);