%more off
%format short

%source octaveIncludes.m;

% int_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter;
		counter = counter + 1;
	end
end

disp(a);

disp(a.');

b=a.';
disp(b.');

% double_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5;
		counter = counter + 1;
	end
end

disp(a);

disp(a.');

b=a.';
disp(b.');

% complex_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5i;
		counter = counter + 1;
	end
end

disp(a);

disp(a.');

b=a.';
disp(b.');

% complex_conjugate_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5i;
		counter = counter + 1;
	end
end

disp(a);

disp(a');

b=a';
disp(b');