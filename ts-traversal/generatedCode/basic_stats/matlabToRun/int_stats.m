function int_stats(fileID, a)
	fun_qs = [0, 0.2, 0.9, 0.53, 0.75, 1, 0.34, 0.17];

	dispArr(fileID, mean(a));
	dispArr(fileID, var(a));
	dispArr(fileID, var(a,1));
	dispArr(fileID, std(a));
	dispArr(fileID, std(a,1));
	dispArr(fileID, sort(a));
	dispArr(fileID, sort(a, "descend"));
	dispArr(fileID, median(a));
	dispArr(fileID, min(a));
	dispArr(fileID, max(a));
	dispArr(fileID, quantile(a, 4));
	dispArr(fileID, quantile(a, fun_qs));
end