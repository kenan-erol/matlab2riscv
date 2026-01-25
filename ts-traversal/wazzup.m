for i=(var(a, 0, 1)-5):1.0:(var(a, 0, 1)+5)
	sprintf("v: %.3f\n", i);
	[h, pval, ci, stats] = vartest(a, i);
	% sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df);
	% sprintf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df);
end