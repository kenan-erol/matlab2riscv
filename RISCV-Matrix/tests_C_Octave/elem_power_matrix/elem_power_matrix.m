more off
format short

source octaveIncludes.m;

%ii_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
intDisp(a);

b = a;
intDisp(b);

c = a.^b;
intDisp(c);

%id_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
doubleDisp(b);

c = a.^b;
doubleDisp(c);

%neg_id_test
a = zeros(3);
for i=1:9
	a(i) = -i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4;
end
b=b.';
doubleDisp(b);

c = a.^b;
complexDisp(c);

%ic_test
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4*I;
end
b=b.';
complexDisp(b);

c = a.^b;
complexDisp(c);

%di_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
intDisp(b);

c = a.^b;
doubleDisp(c);

%dd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
doubleDisp(b);

c = a.^b;
doubleDisp(c);

%neg_dd_test
a = zeros(3);
for i=1:9
	a(i) = -(i+0.4);
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = -(i+0.4);
end
b=b.';
doubleDisp(b);

c = a.^b;
complexDisp(c);

%dc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4*I;
end
b=b.';
complexDisp(b);

c = a.^b;
complexDisp(c);

%ci_test
a = zeros(3);
for i=1:9
	a(i) = i+I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i;
end
b=b.';
intDisp(b);

c = a.^b;
complexDisp(c);

%cd_test
a = zeros(3);
for i=1:9
	a(i) = i+0.5*I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4);
end
b=b.';
doubleDisp(b);

c = a.^b;
complexDisp(c);

%cc_test
a = zeros(3);
for i=1:9
	a(i) = i+0.4*I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i+0.4*I;
end
b=b.';
complexDisp(b);

c = a.^b;
complexDisp(c);

%overflow_test
a = zeros(3);
for i=1:9
	a(i) = INT_MAX;
end
a=a.';
intDisp(a);

b = 2*ones(3);
intDisp(a);

c = a.^b;
doubleDisp(c);