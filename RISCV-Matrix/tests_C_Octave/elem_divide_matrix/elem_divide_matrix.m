more off
format short

source octaveIncludes.m;

a = ones(3);
a(1) = -0.75 + I;
a(5) = -0.75 + I;
a(9) = -0.75 + I;
a=a.';
complexDisp(a);

b = ones(3);
b(1) = 0.5 + I;
b(5) = 0.5 + I;
b(9) = 0.5 + I;
b=b.';

complexDisp(b);

c = a./b;
complexDisp(c);

d = eye(3)./a;
complexDisp(d);

e = b./eye(3);
complexDisp(e);

c = a.\b;
complexDisp(c);

d = eye(3).\a;
complexDisp(d);

e = b.\eye(3);
complexDisp(e);