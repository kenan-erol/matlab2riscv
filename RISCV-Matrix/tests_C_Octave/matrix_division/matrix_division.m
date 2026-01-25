more off
format short

source octaveIncludes.m;

%ldivide_test1
a = [1,4;9,16];
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a\b;

intDisp(a);
complexDisp(b);
complexDisp(c);

%ldivide_test2
a = eye(2);
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a\b;

intDisp(a);
complexDisp(b);
complexDisp(c);

%ldivide_test3
a = [2.1+0.5*i,0;0,2.1+0.5*i];
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a\b;

complexDisp(a);
complexDisp(b);
complexDisp(c);

%rdivide_test1
a = [1,4;9,16];
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a/b;

intDisp(a);
complexDisp(b);
complexDisp(c);

%rdivide_test2
a = eye(2);
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a/b;

intDisp(a);
complexDisp(b);
complexDisp(c);

%rdivide_test3
a = [2.1+0.5*i,0;0,2.1+0.5*i];
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = a/b;

complexDisp(a);
complexDisp(b);
complexDisp(c);

%divide_by_zero
a = zeros(2);
b = [2.1+0.5*i,0;0,2.1+0.5*i];
c = b/a;

intDisp(a);
complexDisp(b);