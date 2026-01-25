%more off
%format short

%source octaveIncludes.m;


iterations = 100;

for i=1:iterations
    %disp("hamming");
	disp(hamming(i));
	%disp("periodic hamming");
	disp(hamming(i, "periodic"));
	%disp("hanning");
	disp(hanning(i));
	%disp("periodic hanning");
	disp(hanning(i, "periodic"));
end