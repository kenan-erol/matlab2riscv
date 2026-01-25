more off
format short

source octaveIncludes.m;

% normalTest
a = 0.5*ones(3);
a(1) += I;
a(5) += I;
a(9) += I;
a = a.';
complexDisp(a);


b = -0.5*ones(3);
b(1) = 0.5 - I;
b(5) = 0.5 - I;
b(9) = 0.5 - I;
b = b.';
complexDisp(b);

c = a+b;
complexDisp(c);

d = c+c;
complexDisp(d);

% overflowTest
d = INT_MAX*eye(3)+eye(3);
doubleDisp(d);

