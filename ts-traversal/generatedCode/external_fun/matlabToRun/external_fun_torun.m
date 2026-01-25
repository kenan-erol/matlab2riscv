addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/external_fun/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/external_fun/output.txt','w');
[F,G] = myfun1(fileID, 1,2);
b = myfun3(fileID, 4);
dispArr(fileID, b);



