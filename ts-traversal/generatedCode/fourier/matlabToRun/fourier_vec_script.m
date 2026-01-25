function fourier_vec_script(fileID, a)

	for i=1:20
		dispArr(fileID, fft(a,i));
		dispArr(fileID, ifft(a,i));
	end

end