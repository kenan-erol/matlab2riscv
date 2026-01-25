more off
format short

source octaveIncludes.m;

a = [1+i,0.4*i;9-0.5*i,16;10+17*i,35-9*i];
b = [2,0;0,i;10+17*i,35-9*i].';
c = a*b;

complexDisp(a);
complexDisp(b);
complexDisp(c);