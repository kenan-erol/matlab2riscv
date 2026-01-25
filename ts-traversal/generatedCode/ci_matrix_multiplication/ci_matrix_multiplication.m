%more off
%format short

%source octaveIncludes.m;

a = [1,4;9,16;25,36];
b = [2.1+0.5i,0;0,2.1+0.5i;0,0].';
c = b*a;

disp(a);
disp(b);
disp(c);