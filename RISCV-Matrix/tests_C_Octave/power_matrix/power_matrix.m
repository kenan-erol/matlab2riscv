more off
format short

source octaveIncludes.m;

% i_zero
a = eye(3);
intDisp(a);
intDisp(a^0);

% d_zero
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
doubleDisp(a);
intDisp(a^0);

% c_zero
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5*I;
end
a = a.';
complexDisp(a);
intDisp(a^0);

% i_one
a = eye(3);
intDisp(a);
intDisp(a^1);

% d_one
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
doubleDisp(a);
doubleDisp(a^1);

% c_one
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5*I;
end
a = a.';
complexDisp(a);
complexDisp(a^1);

% i_large
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
intDisp(a);
doubleDisp(a^20);

% i_negative
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
intDisp(a);
doubleDisp(floor(a^-20));

% d_small
a = zeros(3,3);
for i=1:9
	a(i) = i*i;
end
a = a.';
doubleDisp(a);
complexDisp(a^0.05);

% d_negative
a = zeros(3,3);
for i=1:9
	a(i) = ((-1)^i)*i*i;
end
a = a.';
doubleDisp(a);
complexDisp(a^-1.1);

% c_large
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5*I;
end
a = a.';
complexDisp(a);
complexDisp(a^(-10 + 7.8*I));

% c_small
a = zeros(3,3);
for i=1:9
	a(i) = i*i+0.5*I;
end
a = a.';
complexDisp(a);
complexDisp(a^(-0.8*I));

% brutal_test
matrices = cell(11,1);

matrices{1} = zeros(3);
matrices{2} = ones(3);
matrices{3} = eye(3);
matrices{4} = (4.2 - 0.03*I)*eye(3);

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
	matrices{7}(i) = i*i+0.5*I;
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
	matrices{10}(i) = (i-5.89)*(i)+((0.5)*(4-i))*I;
end
matrices{10} = matrices{10}.';

matrices{11} = [3,-2,0;   4,-1,0;   0,0,1];

matrices{12} = [11.25, -7.525, -1.45;    11, -6.9, -2.2;    5.5, -5.45, 2.9];


for index=3:12
	fprintf(stdout, 'Original\n');
	complexDisp(matrices{index});

	fprintf(stdout, 'Integer exponents\n');
	for i=-4:4
		complexDisp(matrices{index}^i);
	end

	fprintf(stdout, 'Double exponents\n');
	for i=-3:0.2:1.9
		fprintf(stdout, 'Exponent: %.4f\n', i);
		complexDisp(matrices{index}^i);
	end

	fprintf(stdout, 'Complex exponents\n');
	for i=-3:0.2:3
		for j=-3:0.2:3
			if j == 0
				continue
			end
			fprintf(stdout, 'Exponent: %.4f + %.4fi\n', i, j);
			complexDisp(matrices{index}^(i+j*I));
		end
	end
end

% % non_diag1
% a = [1,1;0,1];
% intDisp(a);
% doubleDisp(a^1.5);

% % non_diag2
% a = [3,4,3;-1,0,-1;1,2,3];
% intDisp(a);
% doubleDisp(a^-4.25);