%more off
%format short

%source octaveIncludes.m;

iterations = 100;

size = 100;

a = ones(100);

for n = 1:size
	for m = 1:size
		a(n,m) = ((n-1)*size + m)^2.1 + 0.5 + mod(((n-1)*size + m), 7);
	end
end

for i = 1:iterations
	b = inv(a);
end
