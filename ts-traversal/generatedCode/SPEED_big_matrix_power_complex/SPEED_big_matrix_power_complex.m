%more off
%format short

%source octaveIncludes.m;

iterations = 1;
exponent = 20.48 + I;

size = 1000*1000;

a = ones(1000);

for n = 1:size
	a(n) = n^2+0.5;
end

for i = 1:iterations
	c = a^exponent;
	%disp(c);
end