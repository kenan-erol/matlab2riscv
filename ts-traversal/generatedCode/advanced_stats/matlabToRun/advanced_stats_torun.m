addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/advanced_stats/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/advanced_stats/output.txt','w');
%more off
%format short

%source octaveIncludes.m;
%pkg load statistics;

%row_vectors_i
dispArr(fileID, "row_vectors_i")
a = [3,-5,0,1];
dispArr(fileID, a);
two_t_test(fileID, a, ones(size(a)));
int_vec_stats(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%row_vectors_d
dispArr(fileID, "row_vectors_d")
a = [0.5,0.25,0,0.6];
dispArr(fileID, a);
two_t_test(fileID, a, ones(size(a)));
double_vec_stats(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%column_vectors_i
dispArr(fileID, "column_vectors_i")
a = [3;-5;0;1];
dispArr(fileID, a);
int_vec_stats(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%column_vectors_d
dispArr(fileID, "column_vectors_d")
a = [0.25;0.5;0;0.6];
dispArr(fileID, a);
double_vec_stats(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%matrices_23_i
dispArr(fileID, "matrices_23_i")
a=[3,-2,0;1,5,10];
dispArr(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%matrices_23_d
dispArr(fileID, "matrices_23_d")
a=[3.25,-2,0;1,5,10];
dispArr(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%matrices_32_i
dispArr(fileID, "matrices_32_i")
a=[3,-2;0,1;5,10];
dispArr(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%matrices_32_d
dispArr(fileID, "matrices_32_d")
a=[3.25,-2;0,1;5,10];
dispArr(fileID, a);
double_stats(fileID, a);
dispArr(fileID, "--------------------")

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
dispArr(fileID, a);
double_stats(fileID, a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
dispArr(fileID, a);
double_stats(fileID, a);

% %big_matrix
% a=ones(32,32);
% dispArr(fileID, a);
% double_stats(fileID, a);
% 
% %big_vector
% a=ones(1010,1);
% dispArr(fileID, a);
% int_vec_stats(fileID, a);
% double_stats(fileID, a);







