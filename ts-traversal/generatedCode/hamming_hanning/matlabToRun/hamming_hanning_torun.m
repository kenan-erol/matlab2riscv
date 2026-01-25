addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/hamming_hanning/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/hamming_hanning/output.txt','w');
%more off
%format short

%source octaveIncludes.m;


iterations = 100;

for i=1:iterations
    %dispArr(fileID, "hamming");
	dispArr(fileID, hamming(i));
	%dispArr(fileID, "periodic hamming");
	dispArr(fileID, hamming(i, "periodic"));
	%dispArr(fileID, "hanning");
	dispArr(fileID, hanning(i));
	%dispArr(fileID, "periodic hanning");
	dispArr(fileID, hanning(i, "periodic"));
end