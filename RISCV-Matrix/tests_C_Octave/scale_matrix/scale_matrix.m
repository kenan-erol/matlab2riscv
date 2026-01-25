more off
format short

source octaveIncludes.m;

intDisp(ones(3));

a = zeros(3);

for i=1:9
	a(i) = i;
end
a = a.';
doubleDisp(a);

b = zeros(3);

for i=1:9
	b(i) = i+i*I;
end
b = b.';
complexDisp(b);

intDisp(2*ones(3));
doubleDisp(2.1*ones(3));
complexDisp((2.1+I)*ones(3));

doubleDisp(2*a);
doubleDisp(2.1*a);
complexDisp((2.1+I)*a);

complexDisp(2*b);
complexDisp(2.1*b);
complexDisp((2.1+I)*b);

doubleDisp(2*INT_MAX*ones(3));
doubleDisp(2*INT_MIN*ones(3));