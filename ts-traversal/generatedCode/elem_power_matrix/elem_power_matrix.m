%more off
%format short

%source octaveIncludes.m;

%ii_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
disp(a);

b = a;
disp(b);

c = a.^b;
disp(c);

%id_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%neg_id_test
a = zeros(3);
for i=1:9
	a(i) = -i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%ic_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%di_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%dd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%neg_dd_test
a = zeros(3);
for i=1:9
	a(i) = -(i+0.4);
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = -(i+0.4);
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%dc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%ci_test
a = zeros(3);
for i=1:9
	a(i) = i+1i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%cd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.5i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%cc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4i;
end
b=b.';
disp(b);

c = a.^b;
disp(c);

%overflow_test
%a = zeros(3);
%for i=1:9
%	a(i) = intmax;
%end
%a=a.';
%disp(a);

%b = 2*ones(3);
%disp(a);

%c = a.^b;
%disp(c);