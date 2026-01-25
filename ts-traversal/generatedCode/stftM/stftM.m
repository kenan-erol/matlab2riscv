%more off
%format short

%source octaveIncludes.m;

%function fourier_vec_script(a)
%
%	for win_size=1:9
%		for inc=1:9
%			for num_coef=2:9
%				for win_type=1:3
%					disp(sprintf("win_size: %d, inc: %d, num_coef: %d, win_type: %d\n", win_size, inc, num_coef, win_type));
%					y = stft(a, win_size, inc, num_coef, win_type);
%					disp(y);
%				end
%			end
%		end
%	end
%end

function fourier_vec_script(a)

	for win_size=4:4
		for num_coef=8:8
            for overlap=2:2
				disp(sprintf("win_size: %d, overlap: %d, num_coef: %d\n", win_size, overlap, num_coef));
                y = stft(a, 'Window', hamming(win_size),'OverlapLength', overlap, 'FFTLength', num_coef);
                disp(y);
			end
		end
	end
end

%row_vectors_i
a = [-4:5];
disp(a);
fourier_vec_script(a);

% %row_vectors_d
% a = [-4:0.5:1.5];
% disp(a);
% fourier_vec_script(a);

% %row_vectors_c
% a = zeros(1,100);
% for i=1:100
% 	a(i) = 101-i + (i-1)*1i;
% end
% disp(a);
% fourier_vec_script(a);

% %column_vectors_i
% a = [-4:5].';
% disp(a);
% fourier_vec_script(a);

% %column_vectors_d
% a = [-4:0.5:1.5].';
% disp(a);
% fourier_vec_script(a);

% %column_vectors_c
% a = zeros(100,1);
% for i=1:100
% 	a(i) = 101-i + (i-1)*1i;
% end
% disp(a);
% fourier_vec_script(a);
