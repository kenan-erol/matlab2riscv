addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/index_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/index_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

a = [0, 10, 10i; 
10.102, 10.102+0.5i, -12i; 
-0.0002-0.1i, -100.01i, 81];
a_trans = a.';

dispArr(fileID, a);

for i=1:9
	% Octave is natively column-major matrix storage, but C is row-major
	% So when iterating over a matrix flatly (i.e., not calling dimensions), you must transpose
	if imag(a_trans(i)) < 0
		dispArr(fileID, sprintf('%.5f  %.5fi  \n',real(a_trans(i)),imag(a_trans(i))));
	else
		dispArr(fileID, sprintf('%.5f + %.5fi  \n',real(a_trans(i)),imag(a_trans(i))));
	end
end

for i=1:3
	for j=1:3

		if imag(a(i,j)) < 0
			dispArr(fileID, sprintf('%.5f  %.5fi  \n',real(a(i,j)),imag(a(i,j))));
		else
			dispArr(fileID, sprintf('%.5f + %.5fi  \n',real(a(i,j)),imag(a(i,j))));
		end

	end
end
