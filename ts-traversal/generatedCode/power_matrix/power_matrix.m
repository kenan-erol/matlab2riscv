%more off
%format short

%source octaveIncludes.m;

% i_zero
disp("i_zero");
a = eye(3);
disp(a);
disp(a^0);

% d_zero
disp("d_zero");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
disp(a);
disp(a^0);

% c_zero
disp("c_zero");
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
disp(a);
disp(a^0);

% i_one
disp("i_one");
a = eye(3);
disp(a);
disp(a^1);

% d_one
disp("d_one");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
disp(a);
disp(a^1);

% c_one
disp("c_one");
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
disp(a);
disp(a^1);

% i_large
disp("i_large");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
disp(a);
disp(a^20);

% i_negative
disp("i_neg");
a = zeros(3,3);
for i=1:9
	%a(i) = i*i;
	a(i) = ((-1)^i)*i*i;
end
a = a.';
disp(a);
disp(floor(a^-2));

% d_small
disp("d_small");
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
disp(a);
disp(a^0.05);

% d_negative
disp("d_neg");
a = zeros(3,3);
for i=1:9
	a(i) = ((-1)^i)*i*i;
end
a = a.';
disp(a);
disp(a^-1.1);

% c_large
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
disp(a);
disp(a^(-10 + 7.8i));

% c_small
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5i;
end
a = a.';
disp(a);
disp(a^(-0.8i));

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
	disp(sprintf('Original\n'));
	disp(matrices{index});

	disp(sprintf('Integer exponents\n'));
	for i=-4:4
		disp(matrices{index}^i);
	end

	disp(sprintf('Double exponents\n'));
	for i=-3:0.2:1.9
		disp(sprintf('Exponent: %.4f\n', i));
		disp(matrices{index}^i);
	end

	disp(sprintf('Complex exponents\n'));
	for i=-3:0.2:3
		for j=-3:0.2:3
			if j ~= 0
   			mystr = sprintf('Exponent: %.4f + %.4fi\n', i, j);
   			disp(mystr);
   			disp(matrices{index}^(i+j*1i));
			end
		end
	end
end

% % non_diag1
% a = [1,1;0,1];
% disp(a);
% disp(a^1.5);

% % non_diag2
% a = [3,4,3;-1,0,-1;1,2,3];
% disp(a);
% disp(a^-4.25);