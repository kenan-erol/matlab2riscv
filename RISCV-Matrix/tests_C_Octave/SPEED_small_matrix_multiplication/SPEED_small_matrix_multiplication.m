more off
format short

source octaveIncludes.m;

iterations = 1000000;

a = [1.5,4.5;9.5,16.5];
b = [2.1+0.5*i,0;0,2.1+0.5*i];

doubleDisp(a);
complexDisp(b);

for i = 1:iterations
	c = b*a;
	%complexDisp(c);
endfor