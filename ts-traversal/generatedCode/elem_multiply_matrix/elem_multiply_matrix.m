%more off
%format short

%source octaveIncludes.m;

a = (0.5 + 1i)*eye(3);
disp(a);

b = (0.4 - 0.8i)*eye(3);
b = b.';

disp(b);

c = a.*b;
disp(c);

d = eye(3).*a;
disp(d);

e = 2*eye(3);
disp(e);