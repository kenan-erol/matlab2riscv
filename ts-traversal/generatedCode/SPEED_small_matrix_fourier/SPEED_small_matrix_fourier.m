%more off
%format short

%source octaveIncludes.m;

iterations = 1000000;

a = [1.5,4.5;9.5,16.5];

disp(a);

for i = 1:iterations
	b = fft(a);
end