more off
format short

source octaveIncludes.m;

%int_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i-1);
end
a=a.';
intDisp(a);

doubleDisp(sin(a));
doubleDisp(sind(a));
doubleDisp(cos(a));
doubleDisp(cosd(a));
doubleDisp(tan(a));
doubleDisp(tand(a));


%double_test
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
doubleDisp(a);

doubleDisp(sin(a));
doubleDisp(sind(a));
doubleDisp(cos(a));
doubleDisp(cosd(a));
doubleDisp(tan(a));
doubleDisp(tand(a));


%complex_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = i + 0.5*I;
end
a=a.';
complexDisp(a);

complexDisp(sin(a));
complexDisp(sind(a));
complexDisp(cos(a));
complexDisp(cosd(a));
complexDisp(tan(a));
complexDisp(tand(a));