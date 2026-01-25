more off
format short

source octaveIncludes.m;

function int_reindexing_tests(a)
	a=a.';
	intDisp([a(1)]);
	intDisp([a(4)]);
	intDisp([a(1) ; a(2) ; a(3) ; a(4)]);
	intDisp([a(4) ; a(3) ; a(2) ; a(1)]);
	intDisp([a(2) ; a(2) ; a(2)]);
	intDisp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
endfunction

function double_reindexing_tests(a)
	a=a.';
	doubleDisp([a(1)]);
	doubleDisp([a(4)]);
	doubleDisp([a(1) ; a(2) ; a(3) ; a(4)]);
	doubleDisp([a(4) ; a(3) ; a(2) ; a(1)]);
	doubleDisp([a(2) ; a(2) ; a(2)]);
	doubleDisp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
endfunction

function complex_reindexing_tests(a)
	a=a.';
	complexDisp([a(1)]);
	complexDisp([a(4)]);
	complexDisp([a(1) ; a(2) ; a(3) ; a(4)]);
	complexDisp([a(4) ; a(3) ; a(2) ; a(1)]);
	complexDisp([a(2) ; a(2) ; a(2)]);
	complexDisp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
endfunction

%row_vectors_i
a = [3,-5,0,1];
intDisp(a);
int_reindexing_tests(a);

%row_vectors_d
a = [3.25,-2,0,10.1];
doubleDisp(a);
double_reindexing_tests(a);

%row_vectors_c
a = [3.25,-2,0,1-I];
complexDisp(a);
complex_reindexing_tests(a);

%column_vectors_i
a = [3;-5;0;1];
intDisp(a);
int_reindexing_tests(a);

%column_vectors_d
a = [3.25;-2;0;10.1];
doubleDisp(a);
double_reindexing_tests(a);

%column_vectors_c
a = [3.25;-2;0;1-I];
complexDisp(a);
complex_reindexing_tests(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
intDisp(a);
int_reindexing_tests(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
doubleDisp(a);
double_reindexing_tests(a);

%matrices_23_c
a=[3.25,-2,0;1,5-I,10];
complexDisp(a);
complex_reindexing_tests(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
intDisp(a);
int_reindexing_tests(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
doubleDisp(a);
double_reindexing_tests(a);

%matrices_32_c
a=[3.25,-2;0,1;5-I,10];
complexDisp(a);
complex_reindexing_tests(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
intDisp(a);
int_reindexing_tests(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
doubleDisp(a);
double_reindexing_tests(a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17*I;
end
a=a.';
complexDisp(a);
complex_reindexing_tests(a);