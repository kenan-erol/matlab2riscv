addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/reindex/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/reindex/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%row_vectors_i
a = [3,-5,0,1];
dispArr(fileID, a);
int_reindexing_tests(fileID, a);

%row_vectors_d
a = [3.25,-2,0,10.1];
dispArr(fileID, a);
double_reindexing_tests(fileID, a);

%row_vectors_c
a = [3.25,-2,0,1-1i];
dispArr(fileID, a);
complex_reindexing_tests(fileID, a);

%column_vectors_i
a = [3;-5;0;1];
dispArr(fileID, a);
int_reindexing_tests(fileID, a);

%column_vectors_d
a = [3.25;-2;0;10.1];
dispArr(fileID, a);
double_reindexing_tests(fileID, a);

%column_vectors_c
a = [3.25;-2;0;1-1i];
dispArr(fileID, a);
complex_reindexing_tests(fileID, a);

%matrices_23_i
a=[3,-2,0;1,5,10];
dispArr(fileID, a);
int_reindexing_tests(fileID, a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
dispArr(fileID, a);
double_reindexing_tests(fileID, a);

%matrices_23_c
a=[3.25,-2,0;1,5-1i,10];
dispArr(fileID, a);
complex_reindexing_tests(fileID, a);

%matrices_32_i
a=[3,-2;0,1;5,10];
dispArr(fileID, a);
int_reindexing_tests(fileID, a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
dispArr(fileID, a);
double_reindexing_tests(fileID, a);

%matrices_32_c
a=[3.25,-2;0,1;5-1i,10];
dispArr(fileID, a);
complex_reindexing_tests(fileID, a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
dispArr(fileID, a);
int_reindexing_tests(fileID, a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
dispArr(fileID, a);
double_reindexing_tests(fileID, a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17i;
end
a=a.';
dispArr(fileID, a);
complex_reindexing_tests(fileID, a);





