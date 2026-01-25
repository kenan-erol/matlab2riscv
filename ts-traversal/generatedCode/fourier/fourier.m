%more off
%format short

%source octaveIncludes.m;

%row_vectors_i
a = [3,-5,0,1];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%row_vectors_d
a = [3.25,-2,0,10.1];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%row_vectors_c
a = [3.25,-2,0,1-1i];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_i
a = [3;-5;0;1];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_d
a = [3.25;-2;0;10.1];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%column_vectors_c
a = [3.25;-2;0;1-1i];
disp(a);
fourier_vec_script(a);
fourier_script(a);

%matrices_23_i
a=[3,-2,0;1,5,10];
disp(a);
fourier_script(a);

%matrices_23_d
a=[3.25,-2,0;1,5,10];
disp(a);
fourier_script(a);

%matrices_23_c
a=[3.25,-2,0;1,5-1i,10];
disp(a);
fourier_script(a);

%matrices_32_i
a=[3,-2;0,1;5,10];
disp(a);
fourier_script(a);

%matrices_32_d
a=[3.25,-2;0,1;5,10];
disp(a);
fourier_script(a);

%matrices_32_c
a=[3.25,-2;0,1;5-1i,10];
disp(a);
fourier_script(a);

%matrices_97_i
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2;
end
a=a.';
disp(a);
fourier_script(a);

%matrices_97_d
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i^2/17;
end
a=a.';
disp(a);
fourier_script(a);

%matrices_97_c
a=zeros(7,9);
for i=1:63
	a(i) = (-1)^i*i-i/17i;
end
a=a.';
disp(a);
fourier_script(a);

function fourier_script(a)
	disp(fft(a));
	disp(ifft(a));
end

function fourier_vec_script(a)

	for i=1:20
		disp(fft(a,i));
		disp(ifft(a,i));
	end

end