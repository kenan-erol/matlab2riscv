addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/stftM/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/stftM/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

%function fourier_vec_script(fileID, a)
%
%	for win_size=1:9
%		for inc=1:9
%			for num_coef=2:9
%				for win_type=1:3
%					dispArr(fileID, sprintf("win_size: %d, inc: %d, num_coef: %d, win_type: %d\n", win_size, inc, num_coef, win_type));
%					y = stft(a, win_size, inc, num_coef, win_type);
%					dispArr(fileID, y);
%				end
%			end
%		end
%	end
%end



%row_vectors_i
a = [-4:5];
dispArr(fileID, a);
fourier_vec_script(fileID, a);

% %row_vectors_d
% a = [-4:0.5:1.5];
% dispArr(fileID, a);
% fourier_vec_script(fileID, a);

% %row_vectors_c
% a = zeros(1,100);
% for i=1:100
% 	a(i) = 101-i + (i-1)*1i;
% end
% dispArr(fileID, a);
% fourier_vec_script(fileID, a);

% %column_vectors_i
% a = [-4:5].';
% dispArr(fileID, a);
% fourier_vec_script(fileID, a);

% %column_vectors_d
% a = [-4:0.5:1.5].';
% dispArr(fileID, a);
% fourier_vec_script(fileID, a);

% %column_vectors_c
% a = zeros(100,1);
% for i=1:100
% 	a(i) = 101-i + (i-1)*1i;
% end
% dispArr(fileID, a);
% fourier_vec_script(fileID, a);
