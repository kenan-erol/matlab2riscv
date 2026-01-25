addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/4d_index_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/4d_index_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;



a = zeros(2,3,4,5);
counter = 0;

% Method 1 to create 4D matrix
% Creates the matrix row-major to match C's implementation
% Note that in Octave, the i (row) loop is outside the j (column) loop. this is
% because Octave is natively column-major, so we must assign carefully.
for l=1:5
	for k=1:4
		for i=1:2
			for j=1:3
				a(i,j,k,l) = counter + 0.5;
				counter = counter + 1;
			end
		end
	end
end
dispArr(fileID, a);

% Flat indexing in C must be must be matched in Octave by flipping the row & column iteration
for l=1:5
	for k=1:4
		for i=1:2
			for j=1:3
				dispArr(fileID, a(i,j,k,l));
			end
		end
	end
end
dispArr(fileID, sprintf("\n"));

% Normal indexing in C and normal indexing in Octave are the same
for l=1:5
	for k=1:4
		for j=1:3
			for i=1:2	
				dispArr(fileID, a(i,j,k,l));
			end
		end
	end
end
dispArr(fileID, sprintf("\n"));

% Flat indexing in Octave must be matched by normal indexing in C
for i=1:120
	dispArr(fileID, a(i));
end
dispArr(fileID, sprintf("\n"));



% Method 2 to create 4D matrix
% Creates the matrix column-major to match Octave's implementation
a = zeros(2,3,4,5);
counter = 0;
for i=1:120
	a(i) = counter+0.5;
	counter = counter + 1;
end
dispArr(fileID, a);


% Flat indexing in C must be must be matched in Octave by flipping the row & column iteration
for l=1:5
	for k=1:4
		for i=1:2
			for j=1:3
				dispArr(fileID, a(i,j,k,l));
			end
		end
	end
end
dispArr(fileID, sprintf("\n"));

% Normal indexing in C and normal indexing in Octave are the same
for l=1:5
	for k=1:4
		for j=1:3
			for i=1:2
				dispArr(fileID, a(i,j,k,l));
			end
		end
	end
end
dispArr(fileID, sprintf("\n"));

% Flat indexing in Octave must be matched by normal indexing in C
for i=1:120
	dispArr(fileID, a(i));
end
dispArr(fileID, sprintf("\n"));

