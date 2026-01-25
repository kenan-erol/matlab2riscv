function two_t_test(fileID, a, b)
	[h, pval, ci, stats] = ttest(a, b);
	dispArr(fileID, sprintf("h: %d\npval: %.2f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h, pval, stats.tstat, stats.df, stats.sd));
end