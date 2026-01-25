%more off
%format short

%source octaveIncludes.m;

a = [1.5,4.5;9.5,16.5];
b = [2.1+0.5i,0;0,2.1+0.5i];
c = a*b;

disp(a);
disp(b);
disp(c);