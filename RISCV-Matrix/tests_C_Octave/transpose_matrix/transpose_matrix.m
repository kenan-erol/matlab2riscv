more off
format short

source octaveIncludes.m;

% int_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter;
		counter++;
	end
end

intDisp(a);

intDisp(a.');

b=a.';
intDisp(b.');

% double_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5;
		counter++;
	end
end

doubleDisp(a);

doubleDisp(a.');

b=a.';
doubleDisp(b.');

% complex_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5*I;
		counter++;
	end
end

complexDisp(a);

complexDisp(a.');

b=a.';
complexDisp(b.');

% complex_conjugate_test
a = zeros(3,6);
counter=1;
for i=1:3
	for j=1:6
		a(i,j) = counter*counter+0.5 - 0.5*I;
		counter++;
	end
end

complexDisp(a);

complexDisp(a');

b=a';
complexDisp(b');