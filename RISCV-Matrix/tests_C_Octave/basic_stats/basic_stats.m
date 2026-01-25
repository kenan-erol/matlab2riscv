more off
format short

source octaveIncludes.m;

function [mu, sd] = normfit(a)
	mu = mean(a);
	sd = std(a);
endfunction

function [ahat, bhat] = unifit(a)
	ahat = min(a);
	bhat = max(a);
endfunction

function int_vec_stats(a)
	[greatest, index] = max(a);
	intDisp(greatest);
	fprintf("max index: %d\n", index);

	[least, index] = min(a);
	intDisp(least);
	fprintf("min index: %d\n", index);

	[mu, sd] = normfit(a);
	printf("mean: %.3f\n", mu);
	printf("sd: %.3f\n", sd);

	[ahat, bhat] = unifit(a);
	printf("a: %d\n", ahat);
	printf("b: %d\n", bhat);
endfunction

function double_vec_stats(a)
	[greatest, index] = max(a);
	doubleDisp(greatest);
	fprintf("max index: %d\n", index);

	[least, index] = min(a);
	doubleDisp(least);
	fprintf("min index: %d\n", index);

	[mu, sd] = normfit(a);
	printf("mean: %.3f\n", mu);
	printf("sd: %.3f\n", sd);

	[ahat, bhat] = unifit(a);
	printf("a: %.3f\n", ahat);
	printf("b: %.3f\n", bhat);
endfunction

function complex_vec_stats(a)
	[greatest, index] = max(a);
	complexDisp(greatest);
	fprintf("max index: %d\n", index);

	[least, index] = min(a);
	complexDisp(least);
	fprintf("min index: %d\n", index);

	[mu, sd] = normfit(a);
	printf("mean: %.3f + %.3fi\n", real(mu), imag(mu));
	printf("sd: %.3f + %.3fi\n", real(sd), imag(sd));

	[ahat, bhat] = unifit(a);
	printf("a: %.3f + %.3fi\n", real(ahat), imag(ahat));
	printf("b: %.3f + %.3fi\n", real(bhat), imag(bhat));

endfunction

function int_stats(a)
	fun_qs = [0, -1, 3, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17];

	doubleDisp(mean(a));
	doubleDisp(var(a));
	doubleDisp(var(a,1));
	doubleDisp(std(a));
	doubleDisp(std(a,1));
	intDisp(sort(a));
	intDisp(sort(a, "descend"));
	intDisp(median(a));
	intDisp(min(a));
	intDisp(max(a));
	doubleDisp(quantile(a));
	doubleDisp(quantile(a, fun_qs));
endfunction

function double_stats(a)
	fun_qs = [0, -1, 3, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17];

	doubleDisp(mean(a));
	doubleDisp(var(a));
	doubleDisp(var(a,1));
	doubleDisp(std(a));
	doubleDisp(std(a,1));
	doubleDisp(sort(a));
	doubleDisp(sort(a, "descend"));
	doubleDisp(median(a));
	doubleDisp(min(a));
	doubleDisp(max(a));
	doubleDisp(quantile(a));
	doubleDisp(quantile(a, fun_qs));
endfunction

function complex_stats(a)
	fun_qs = [0, -1, 3, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17];

	complexDisp(mean(a));
	complexDisp(var(a));
	complexDisp(var(a,1));
	complexDisp(std(a));
	complexDisp(std(a,1));
	complexDisp(sort(a));
	complexDisp(sort(a, "descend"));
	complexDisp(median(a));
	complexDisp(min(a));
	complexDisp(max(a));
	complexDisp(quantile(a));
	complexDisp(quantile(a, fun_qs));
endfunction

%row_vectors_i
a = [3,-5,0,1];
intDisp(a);
int_vec_stats(a);
int_stats(a);

%row_vectors_d
a = [3.25,-2,0,10.1];
doubleDisp(a);
double_vec_stats(a);
double_stats(a);

%row_vectors_c
a = [3.25,-2,0,1-I];
complexDisp(a);
complex_vec_stats(a);
complex_stats(a);

%column_vectors_i
a = [3;-5;0;1];
intDisp(a);
int_vec_stats(a);
int_stats(a);

%column_vectors_d
a = [3.25;-2;0;10.1];
doubleDisp(a);
double_vec_stats(a);
double_stats(a);

%column_vectors_c
a = [3.25;-2;0;1-I];
complexDisp(a);
complex_vec_stats(a);
complex_stats(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
intDisp(a);
int_stats(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
doubleDisp(a);
double_stats(a);

%matrices_23_c
a=[3.25,-2,0;1,5-I,10];
complexDisp(a);
complex_stats(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
intDisp(a);
int_stats(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
doubleDisp(a);
double_stats(a);

%matrices_32_c
a=[3.25,-2;0,1;5-I,10];
complexDisp(a);
complex_stats(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
intDisp(a);
int_stats(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
doubleDisp(a);
double_stats(a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17*I;
end
a=a.';
complexDisp(a);
complex_stats(a);

%basic_quantile_test
a = 1:100;
doubleDisp(quantile(a, 0:0.01:1).');

b = zeros(1,1004);
for i=1:1004
	b(i) = i*i/17;
end
b=b.';
doubleDisp(quantile(b, 0:0.01:1).');

c = zeros(1,57);
for i=1:57
	c(i) = i-i/17*I;
end
c=c.';
complexDisp(quantile(c, 0:0.01:1).');