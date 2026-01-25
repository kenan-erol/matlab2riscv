%more off
%format short

%source octaveIncludes.m;

%int_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i-1);
end
a=a.';
disp(a);

disp(sin(a));
disp(sind(a));
disp(cos(a));
disp(cosd(a));
disp(tan(a));
disp(tand(a));


%double_test
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
disp(a);

disp(sin(a));
disp(sind(a));
disp(cos(a));
disp(cosd(a));
disp(tan(a));
disp(tand(a));


%complex_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = i + 0.5i;
end
a=a.';
disp(a);

disp(sin(a));
disp(sind(a));
disp(cos(a));
disp(cosd(a));
disp(tan(a));
disp(tand(a));