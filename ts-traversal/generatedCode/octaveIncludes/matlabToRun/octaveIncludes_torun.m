addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/octaveIncludes/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/octaveIncludes/output.txt','w');
% Functions that display matrices in the right format to be diff'ed with printM outputs
% Also defines INT_MAX and INT_MIN

INT_MAX = 2147483647;
INT_MIN = -2147483648;

% This function is equivalent to dispArr(fileID, ), but formats complex numbers correctly for diffing outputs
function intDisp (mat)
	for i=1:size(mat)(1)
		for j=1:size(mat)(2)
			fprintf('%d\t',mat(i,j));
		end
		fprintf('\n');
	end
endfunction

function doubleDisp (mat)
	for i=1:size(mat)(1)
		for j=1:size(mat)(2)
			fprintf('%.4f\t',mat(i,j));
		end
		fprintf('\n');
	end
endfunction

function complexDisp (mat)
	for i=1:size(mat)(1)
		for j=1:size(mat)(2)

			if imag(mat(i,j)) < 0
				fprintf('%.4f  %.4fi  \t',real(mat(i,j)),imag(mat(i,j)));
			else
				fprintf('%.4f + %.4fi  \t',real(mat(i,j)),imag(mat(i,j)));
			end

		end
		fprintf('\n');
	end
endfunction