more off
format short

source octaveIncludes.m;

function fourier_script(a)
	complexDisp(fft(a));
	complexDisp(ifft(a));
endfunction

function fourier_vec_script(a)

	for i=1:20
		complexDisp(fft(a,i));
		complexDisp(ifft(a,i));
	end

endfunction

%row_vectors_i
a = [3,-5,0,1];
intDisp(a);
fourier_vec_script(a);
fourier_script(a);

%row_vectors_d
a = [3.25,-2,0,10.1];
doubleDisp(a);
fourier_vec_script(a);
fourier_script(a);

%row_vectors_c
a = [3.25,-2,0,1-I];
complexDisp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_i
a = [3;-5;0;1];
intDisp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_d
a = [3.25;-2;0;10.1];
doubleDisp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_c
a = [3.25;-2;0;1-I];
complexDisp(a);
fourier_vec_script(a);
fourier_script(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
intDisp(a);
fourier_script(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
doubleDisp(a);
fourier_script(a);

%matrices_23_c
a=[3.25,-2,0;1,5-I,10];
complexDisp(a);
fourier_script(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
intDisp(a);
fourier_script(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
doubleDisp(a);
fourier_script(a);

%matrices_32_c
a=[3.25,-2;0,1;5-I,10];
complexDisp(a);
fourier_script(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
intDisp(a);
fourier_script(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
doubleDisp(a);
fourier_script(a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17*I;
end
a=a.';
complexDisp(a);
fourier_script(a);