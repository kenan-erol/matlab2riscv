addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/randomness/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/randomness/output.txt','w');
%more off
%format short

%source octaveIncludes.m;


% rand_test
rand("seed", 1);
a = rand(3, 4);
s1 = rand("seed");

rand("seed", 1);
b = rand(3, 4);
s2 = rand("seed");


%randi_test
rand("seed", "reset");
a = randi(10, 3, 4);
dispArr(fileID, a <= 10);


%randn_test
rand("seed", "reset");
a = randn(1000);
mu = mean(a);
sigma = var(a);