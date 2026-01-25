function int_vec_stats(fileID, a)
	[greatest, index] = max(a);
	dispArr(fileID, greatest);
	dispArr(fileID, sprintf("max index: %d\n", index));

	[least, index] = min(a);
	dispArr(fileID, least);
	dispArr(fileID, sprintf("min index: %d\n", index));

	[mu, sd] = normfit(fileID, a);
	dispArr(fileID, sprintf("mean: %.3f\n", mu));
	dispArr(fileID, sprintf("sd: %.3f\n", sd));

	[ahat, bhat] = unifit(fileID, a);
	dispArr(fileID, sprintf("a: %d\n", ahat));
	dispArr(fileID, sprintf("b: %d\n", bhat));
end