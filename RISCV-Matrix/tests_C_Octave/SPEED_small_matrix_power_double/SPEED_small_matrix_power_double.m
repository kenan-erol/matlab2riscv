more off
format short

source octaveIncludes.m;

exponent = 10.5;
iterations = 100000;

b = [2.1+0.5*i,1;1,2.1+0.5*i];
complexDisp(b);

c = b^exponent;
complexDisp(c);

for i = 1:iterations
	c = b^exponent;
	%complexDisp(c);
endfor
