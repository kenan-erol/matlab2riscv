%more off
%format short

%source octaveIncludes.m;

iterations = 1;

size = 1000;

a = ones(1000);

for n = 1:size
	for m = 1:size
		a(n,m) = ((n-1)*size + m)^2.1 + 0.5 + mod(((n-1)*size + m), 7);
	end
end

for i = 1:iterations
	b = fft(a);
end
