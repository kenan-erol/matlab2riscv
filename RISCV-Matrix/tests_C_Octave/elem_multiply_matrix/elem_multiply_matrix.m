more off
format short

source octaveIncludes.m;

a = (0.5 + I)*eye(3);
complexDisp(a);

b = (0.4 - 0.8*I)*eye(3);
b = b.';

complexDisp(b);

c = a.*b;
complexDisp(c);

d = eye(3).*a;
complexDisp(d);

e = 2*INT_MAX*eye(3);
doubleDisp(e);