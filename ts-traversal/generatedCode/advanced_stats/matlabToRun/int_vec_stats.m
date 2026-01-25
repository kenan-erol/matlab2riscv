function int_vec_stats(fileID, a)
	[greatest, index] = max(a);
	dispArr(fileID, greatest);
	dispArr(fileID, sprintf("max index: %d\n\n", index));

	[least, index] = min(a);
	dispArr(fileID, least);
	dispArr(fileID, sprintf("min index: %d\n\n", index));

	for i=least:0.5:greatest
		dispArr(fileID, sprintf("mu: %.3f\n", i));
		[h, pval, ci, stats] = ttest(a, i);
		dispArr(fileID, sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\ntstat: %.3f\ndf: %.3f\nsd: %.3f\n", h, pval, ci(1), ci(2), stats.tstat, stats.df, stats.sd));

		[h, pval, ci, z] = ztest(a, i, std(a, 0, "all"));
		dispArr(fileID, sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\nz: %.3f\n", h, pval, ci(1), ci(2), z));
	end
    
    %if var(a, 0, "all") == 0
    %    i_range = [0:1.0:(var(a, 0, "all")+5)];
    %else 
    %    i_range = [0:1.0:2*var(a, 0, "all")];
    %end
    i_range = [0:1.0:2*var(a, 0, "all")];
	for i=i_range
		dispArr(fileID, sprintf("v: %.3f\n", i));
		%[h, pval, ci, stats] = vartest(a, i);
		% dispArr(fileID, sprintf("h: %d\npval: %.2f\nci: %.3f, %.3f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, ci(1), ci(2), stats.chisqstat, stats.df));
		% dispArr(fileID, sprintf("h: %d\npval: %.2f\nchisqstat: %.3f\ndf: %.3f\n", h, pval, stats.chisqstat, stats.df));
	end

end