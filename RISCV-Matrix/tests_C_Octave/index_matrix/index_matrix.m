more off
format short

source octaveIncludes.m;

a = [0, 10, 10*i; 
10.102, 10.102+0.5*i, -12*i; 
-0.0002-0.1*I, -100.01*I, 81];

complexDisp(a);

for i=1:9
	% Octave is natively column-major matrix storage, but C is row-major
	% So when iterating over a matrix flatly (i.e., not calling dimensions), you must transpose
	if imag(a.'(i)) < 0
		fprintf('%.5f  %.5fi  \n',real(a.'(i)),imag(a.'(i)));
	else
		fprintf('%.5f + %.5fi  \n',real(a.'(i)),imag(a.'(i)));
	end
end

for i=1:3
	for j=1:3

		if imag(a(i,j)) < 0
			fprintf('%.5f  %.5fi  \n',real(a(i,j)),imag(a(i,j)));
		else
			fprintf('%.5f + %.5fi  \n',real(a(i,j)),imag(a(i,j)));
		end

	end
end
