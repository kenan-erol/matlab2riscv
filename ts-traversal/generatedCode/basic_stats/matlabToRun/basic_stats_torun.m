addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/basic_stats/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/basic_stats/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%row_vectors_i
a = [3,-5,0,1];
dispArr(fileID, a);
int_vec_stats(fileID, a);
int_stats(fileID, a);

%row_vectors_d
a = [3.25,-2,0,10.1];
dispArr(fileID, a);
double_vec_stats(fileID, a);
double_stats(fileID, a);

%row_vectors_c
a = [3.25,-2,0,1-1i];
dispArr(fileID, a);
complex_vec_stats(fileID, a);
complex_stats(fileID, a);

%column_vectors_i
a = [3;-5;0;1];
dispArr(fileID, a);
int_vec_stats(fileID, a);
int_stats(fileID, a);

%column_vectors_d
a = [3.25;-2;0;10.1];
dispArr(fileID, a);
double_vec_stats(fileID, a);
double_stats(fileID, a);

%column_vectors_c
a = [3.25;-2;0;1-1i];
dispArr(fileID, a);
complex_vec_stats(fileID, a);
complex_stats(fileID, a);

%matrices_23_i
a=[3,-2,0;1,5,10];
dispArr(fileID, a);
int_stats(fileID, a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
dispArr(fileID, a);
double_stats(fileID, a);

%matrices_23_c
a=[3.25,-2,0;1,5-1i,10];
dispArr(fileID, a);
complex_stats(fileID, a);

%matrices_32_i
a=[3,-2;0,1;5,10];
dispArr(fileID, a);
int_stats(fileID, a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
dispArr(fileID, a);
double_stats(fileID, a);

%matrices_32_c
a=[3.25,-2;0,1;5-1i,10];
dispArr(fileID, a);
complex_stats(fileID, a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
dispArr(fileID, a);
int_stats(fileID, a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
dispArr(fileID, a);
double_stats(fileID, a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17i;
end
a=a.';
dispArr(fileID, a);
complex_stats(fileID, a);

%basic_quantile_test
a = [1:100];
dispArr(fileID, quantile(a, 0:0.01:1).');

b = zeros(1,1004);
for i=1:1004
	b(i) = i*i/17;
end
b=b.';
dispArr(fileID, quantile(b, 0:0.01:1).');

c = zeros(1,57);
for i=1:57
	c(i) = i-i/17i;
end
c=c.';
%dispArr(fileID, quantile(c, 0:0.01:1).');















