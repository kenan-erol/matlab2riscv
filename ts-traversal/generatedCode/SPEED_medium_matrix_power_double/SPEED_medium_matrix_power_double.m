%more off
%format short

%source octaveIncludes.m;

iterations = 100;
exponent = 20.48;

size = 100*100;

a = ones(100);

for n = 1:size
	a(n) = n^2+0.5;
end

for i = 1:iterations
	c = a^exponent;
	%disp(c);
end