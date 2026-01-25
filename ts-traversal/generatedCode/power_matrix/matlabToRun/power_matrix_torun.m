addpath('/gpfs/gibbs/project/manohar/dlg59/ts-traversal/generatedCode');
addpath('/home/dlg59/project/ts-traversal/generatedCode/power_matrix/matlabToRun');
fileID = fopen('/home/dlg59/project/ts-traversal/generatedCode/power_matrix/output.txt','w');
%more off
%format short

%source octaveIncludes.m;

% i_zero
dispArr(fileID, "i_zero");
a = eye(3);
dispArr(fileID, a);
dispArr(fileID, a^0);

% d_zero
dispArr(fileID, "d_zero");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^0);

% c_zero
dispArr(fileID, "c_zero");
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^0);

% i_one
dispArr(fileID, "i_one");
a = eye(3);
dispArr(fileID, a);
dispArr(fileID, a^1);

% d_one
dispArr(fileID, "d_one");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^1);

% c_one
dispArr(fileID, "c_one");
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^1);

% i_large
dispArr(fileID, "i_large");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^20);

% i_negative
dispArr(fileID, "i_neg");
a = zeros(3,3);
for i=1:9
	%a(i) = i*i;
	a(i) = ((-1)^i)*i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, floor(a^-2));

% d_small
dispArr(fileID, "d_small");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^0.05);

% d_negative
dispArr(fileID, "d_neg");
a = zeros(3,3);
for i=1:9
	a(i) = ((-1)^i)*i*i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^-1.1);

% c_large
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^(-10 + 7.8i));

% c_small
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
dispArr(fileID, a);
dispArr(fileID, a^(-0.8i));

% brutal_test
matrices = cell(11,1);

matrices{1} = zeros(3);
matrices{2} = ones(3);
matrices{3} = eye(3);
matrices{4} = (4.2 - 0.03i)*eye(3);

matrices{5} = zeros(3);
for i=1:9
	matrices{5}(i) = i*i;
end
matrices{5} = matrices{5}.';


matrices{6} = zeros(3);
for i=1:9
	matrices{6}(i) = i*i+0.5;
end
matrices{6} = matrices{6}.';


matrices{7} = zeros(3);
for i=1:9
	matrices{7}(i) = i*i+0.5i;
end
matrices{7} = matrices{7}.';


matrices{8} = zeros(3);
for i=1:9
	matrices{8}(i) = (i-5)*i;
end
matrices{8} = matrices{8}.';


matrices{9} = zeros(3);
for i=1:9
	matrices{9}(i) = (i-8.2)*i+0.5;
end
matrices{9} = matrices{9}.';

matrices{10} = zeros(3);
for i=1:9
	matrices{10}(i) = (i-5.89)*(i)+((0.5)*(4-i))*1i;
end
matrices{10} = matrices{10}.';

matrices{11} = [3,-2,0;   4,-1,0;   0,0,1];

matrices{12} = [11.25, -7.525, -1.45;    11, -6.9, -2.2;    5.5, -5.45, 2.9];

%for index=3:12 -> removed 12 because yields small rounding error
for index=3:11
	dispArr(fileID, sprintf('Original\n'));
	dispArr(fileID, matrices{index});

	dispArr(fileID, sprintf('Integer exponents\n'));
	for i=-4:4
		dispArr(fileID, matrices{index}^i);
	end

	dispArr(fileID, sprintf('Double exponents\n'));
	for i=-3:0.2:1.9
		dispArr(fileID, sprintf('Exponent: %.4f\n', i));
		dispArr(fileID, matrices{index}^i);
	end

	dispArr(fileID, sprintf('Complex exponents\n'));
	for i=-3:0.2:3
		for j=-3:0.2:3
			if j ~= 0
   			mystr = sprintf('Exponent: %.4f + %.4fi\n', i, j);
   			dispArr(fileID, mystr);
   			dispArr(fileID, matrices{index}^(i+j*1i));
			end
		end
	end
end

% % non_diag1
% a = [1,1;0,1];
% dispArr(fileID, a);
% dispArr(fileID, a^1.5);

% % non_diag2
% a = [3,4,3;-1,0,-1;1,2,3];
% dispArr(fileID, a);
% dispArr(fileID, a^-4.25);