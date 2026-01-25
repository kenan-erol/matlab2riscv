%more off
%format short

%source octaveIncludes.m;

%ldivide_test1
a = [1,4;9,16];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

disp(a);
disp(b);
disp(c);

%ldivide_test2
a = eye(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

disp(a);
disp(b);
disp(c);

%ldivide_test3
a = [2.1+0.5*1i,0;0,2.1+0.5*1i];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a\b;

disp(a);
disp(b);
disp(c);

%rdivide_test1
a = [1,4;9,16];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

disp(a);
disp(b);
disp(c);

%rdivide_test2
a = eye(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

disp(a);
disp(b);
disp(c);

%rdivide_test3
a = [2.1+0.5*1i,0;0,2.1+0.5*1i];
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = a/b;

disp(a);
disp(b);
disp(c);

%divide_by_zero
a = zeros(2);
b = [2.1+0.5*1i,0;0,2.1+0.5*1i];
c = b/a;

disp(a);
disp(b);