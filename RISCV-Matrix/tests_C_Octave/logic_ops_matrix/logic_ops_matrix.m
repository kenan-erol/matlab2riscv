more off
format short

source octaveIncludes.m;

a = [0, 10, 10*i; 
10.102, 10.102+0.5*i, -12*i; 
-0.0002-0.1*I, -100.01*I, 81];

complexDisp(a);

intDisp(~a);

intDisp(~zeros(3));
intDisp(~ones(3));
intDisp(~eye(3));

intDisp(a & zeros(3));
intDisp(a & ~eye(3));

intDisp(zeros(3) | ~ones(3));
intDisp(~ones(3) | eye(3));
