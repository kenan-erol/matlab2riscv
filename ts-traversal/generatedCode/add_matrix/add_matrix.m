%more off
%format short

%source octaveIncludes.m;

% normalTest
a = 0.5*ones(3);
a(1) = a(1) + 1i;
a(5) = a(5) + 1i;
a(9) = a(9) + 1i;
a = a.';
disp(a);


b = -0.5*ones(3);
b(1) = 0.5 - 1i;
b(5) = 0.5 - 1i;
b(9) = 0.5 - 1i;
b = b.';
disp(b);

c = a+b;
disp(c);

d = c+c;
disp(d);

% overflowTest
%d = intmax*eye(3)+eye(3);
d = eye(3)+eye(3);
disp(d);

