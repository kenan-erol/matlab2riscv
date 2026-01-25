%more off
%format short

%source octaveIncludes.m;

iterations = 1000000;

a = [1.5,4.5;9.5,16.5];
b = [2.1+0.5i,0;0,2.1+0.5i];

disp(a);
disp(b);

for i = 1:iterations
	c = b*a;
	%disp(c);
end