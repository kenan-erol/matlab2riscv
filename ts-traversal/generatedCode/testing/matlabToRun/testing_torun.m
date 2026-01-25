addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/testing/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/testing/output.txt','w');
a = [0, 10, 10i; 
10.102, 10.102+0.5i, -12i; 
-0.0002-0.1i, -100.01i, 81];

b = abs(a);

dispArr(fileID, a);
dispArr(fileID, b)

c = 1.0;
dispArr(fileID, c);
