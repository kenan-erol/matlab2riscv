function fourier_script(fileID, a)
	dispArr(fileID, fft(a));
	dispArr(fileID, ifft(a));
end