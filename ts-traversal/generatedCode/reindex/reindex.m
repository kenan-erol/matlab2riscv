%more off
%format short

%source octaveIncludes.m;

%row_vectors_i
a = [3,-5,0,1];
disp(a);
int_reindexing_tests(a);

%row_vectors_d
a = [3.25,-2,0,10.1];
disp(a);
double_reindexing_tests(a);

%row_vectors_c
a = [3.25,-2,0,1-1i];
disp(a);
complex_reindexing_tests(a);

%column_vectors_i
a = [3;-5;0;1];
disp(a);
int_reindexing_tests(a);

%column_vectors_d
a = [3.25;-2;0;10.1];
disp(a);
double_reindexing_tests(a);

%column_vectors_c
a = [3.25;-2;0;1-1i];
disp(a);
complex_reindexing_tests(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
disp(a);
int_reindexing_tests(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
disp(a);
double_reindexing_tests(a);

%matrices_23_c
a=[3.25,-2,0;1,5-1i,10];
disp(a);
complex_reindexing_tests(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
disp(a);
int_reindexing_tests(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
disp(a);
double_reindexing_tests(a);

%matrices_32_c
a=[3.25,-2;0,1;5-1i,10];
disp(a);
complex_reindexing_tests(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
disp(a);
int_reindexing_tests(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
disp(a);
double_reindexing_tests(a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17i;
end
a=a.';
disp(a);
complex_reindexing_tests(a);

function int_reindexing_tests(a)
	a=a.';
	disp([a(1)]);
	disp([a(4)]);
	disp([a(1) ; a(2) ; a(3) ; a(4)]);
	disp([a(4) ; a(3) ; a(2) ; a(1)]);
	disp([a(2) ; a(2) ; a(2)]);
	disp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
end

function double_reindexing_tests(a)
	a=a.';
	disp([a(1)]);
	disp([a(4)]);
	disp([a(1) ; a(2) ; a(3) ; a(4)]);
	disp([a(4) ; a(3) ; a(2) ; a(1)]);
	disp([a(2) ; a(2) ; a(2)]);
	disp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
end

function complex_reindexing_tests(a)
	a=a.';
	disp([a(1)]);
	disp([a(4)]);
	disp([a(1) ; a(2) ; a(3) ; a(4)]);
	disp([a(4) ; a(3) ; a(2) ; a(1)]);
	disp([a(2) ; a(2) ; a(2)]);
	disp([a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
end