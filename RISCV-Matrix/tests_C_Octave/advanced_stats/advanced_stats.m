more off
format short

source octaveIncludes.m;
pkg load statistics;

function two_t_test(a, b)
	[h, pval, ci, stats] = ttest(a, b);
	printf("h: %d\npval: %.2f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h, pval, stats.tstat, stats.df, stats.sd);
endfunction

function int_vec_stats(a)
	[greatest, index] = max(a);
	intDisp(greatest);
	fprintf("max index: %d\n\n", index);

	[least, index] = min(a);
	intDisp(least);
	fprintf("min index: %d\n\n", index);

	for i=least:0.5:greatest
		printf("mu: %.3f\n", i);
		[h, pval, ci, stats] = ttest(a, i);
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h, pval, ci(1), ci(2), stats.tstat, stats.df, stats.sd);

		[h, pval, ci, z, zcrit] = ztest(a, i, std(a, 1));
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\nzcrit: %.3f\n", h, pval, ci(1), ci(2), z, zcrit);
	end

	for i=(var(a, 1)-5):1.0:(var(a, 1)+5)
		printf("v: %.3f\n", i);
		[h, pval, ci, stats] = vartest(a, i);
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df);
		% printf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df);
	end

endfunction

function double_vec_stats(a)
	[greatest, index] = max(a);
	doubleDisp(greatest);
	fprintf("max index: %d\n", index);

	[least, index] = min(a);
	doubleDisp(least);
	fprintf("min index: %d\n", index);

	for i=least:0.5:greatest
		printf("mu: %.3f\n", i);
		[h, pval, ci, stats] = ttest(a, i);
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h, pval, ci(1), ci(2), stats.tstat, stats.df, stats.sd);

		[h, pval, ci, z, zcrit] = ztest(a, i, std(a, 1));
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\nzcrit: %.3f\n", h, pval, ci(1), ci(2), z, zcrit);
	end

	for i=(var(a, 1)-5):1.0:(var(a, 1)+5)
		printf("v: %.3f\n", i);
		[h, pval, ci, stats] = vartest(a, i);
		% printf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df);
		% printf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df);
	end
endfunction

function double_stats(a)
	fun_qs = [0, -1, 3, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17];

	% Beta PDF
	for i=0:0.05:0.95
		for j=0:0.05:0.95
			doubleDisp(betapdf(a, i, j));
		end
	end

	% Exponential PDF
	for lambda=0.05:0.05:4.95
		doubleDisp(exppdf(a, lambda));
	end

	% Chi-square PDF
	for n=0.05:0.05:4.95
		printf("n = %.3f\n", n);
		doubleDisp(chi2pdf(a, n));
	end

	% Gamma PDF
	for i=0.25:0.25:1.75
		for j=0.25:0.25:1.75
			doubleDisp(gampdf(a, i, j));
		end
	end

	% Lognormal PDF
	for mu = -2:0.5:5
		for sd=0.5:0.5:5
			doubleDisp(lognpdf(a, mu, sd));
		end
	end

	% Normal PDF
	for mu = -2:0.5:5
		for sd=0.5:0.5:5
			doubleDisp(normpdf(a, mu, sd));
		end
	end

	% Uniform discrete PDF
	for n=1:9
		printf("n = %d\n",n);
		doubleDisp(unidpdf(a, n));
	end

endfunction

%row_vectors_i
a = [3,-5,0,1];
intDisp(a);
two_t_test(a, ones(size(a)));
int_vec_stats(a);
double_stats(a);

%row_vectors_d
a = [0.5,0.25,0,0.6];
doubleDisp(a);
two_t_test(a, ones(size(a)));
double_vec_stats(a);
double_stats(a);

%column_vectors_i
a = [3;-5;0;1];
intDisp(a);
int_vec_stats(a);
double_stats(a);

%column_vectors_d
a = [0.25;0.5;0;0.6];
doubleDisp(a);
double_vec_stats(a);
double_stats(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
intDisp(a);
double_stats(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
doubleDisp(a);
double_stats(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
intDisp(a);
double_stats(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
doubleDisp(a);
double_stats(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
intDisp(a);
double_stats(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
doubleDisp(a);
double_stats(a);

%big_matrix
a=ones(32,32);
intDisp(a);
double_stats(a);

%big_vector
a=ones(1010,1);
intDisp(a);
int_vec_stats(a);
double_stats(a);
