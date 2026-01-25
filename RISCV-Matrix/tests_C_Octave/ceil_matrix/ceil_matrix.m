more off
format short

source octaveIncludes.m;

a = [0, 10, 10*i; 
10.102, 10.102+0.5*i, -12*i; 
-0.0002-0.1*I, -100.01*I, 81];

b = ceil(a);

complexDisp(a);
complexDisp(b);