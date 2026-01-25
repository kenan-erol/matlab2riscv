function complex_vec_stats(fileID, a)
	[greatest, index] = max(a);
	dispArr(fileID, greatest);
	dispArr(fileID, sprintf("max index: %d\n", index));

	[least, index] = min(a);
	dispArr(fileID, least);
	dispArr(fileID, sprintf("min index: %d\n", index));

	[mu, sd] = normfit(fileID, a);
	dispArr(fileID, sprintf("mean: %.3f + %.3fi\n", real(mu), imag(mu)));
	dispArr(fileID, sprintf("sd: %.3f + %.3fi\n", real(sd), imag(sd)));

	[ahat, bhat] = unifit(fileID, a);
	dispArr(fileID, sprintf("a: %.3f + %.3fi\n", real(ahat), imag(ahat)));
	dispArr(fileID, sprintf("b: %.3f + %.3fi\n", real(bhat), imag(bhat)));

end