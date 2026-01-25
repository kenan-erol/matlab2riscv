more off
format short

source octaveIncludes.m;

%int_test
a = [1,2;3,4];
intDisp(a);
doubleDisp(inv(a));
doubleDisp(inv(inv(a)));

%double_test
a = [1.5,2.5;3.5,4.5];
doubleDisp(a);
doubleDisp(inv(a));
doubleDisp(inv(inv(a)));

%complex_test
a = [1.5+I,2.5+2*I;3.5+3*I,4.5+4*I];
complexDisp(a);
complexDisp(inv(a));
complexDisp(inv(inv(a)));

%singular_test
intDisp(ones(2));

%non_square_test
intDisp(ones(2,3));
