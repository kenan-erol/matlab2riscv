function fourier_vec_script(fileID, a)

	for win_size=4:4
		for num_coef=8:8
            for overlap=2:2
				dispArr(fileID, sprintf("win_size: %d, overlap: %d, num_coef: %d\n", win_size, overlap, num_coef));
                y = stft(a, 'Window', hamming(win_size),'OverlapLength', overlap, 'FFTLength', num_coef);
                dispArr(fileID, y);
			end
		end
	end
end