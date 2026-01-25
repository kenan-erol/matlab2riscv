more off
format short

source octaveIncludes.m;

%ii_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*i)^exponent;
end
b=b.';
intDisp(b);

c = a.^exponent;
intDisp(c);

%id_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i^exponent;
end
b=b.';
doubleDisp(b);

c = a.^exponent;
doubleDisp(c);

%neg_id_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*i)^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%ic_test
exponent=4+0.3*I;
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = i^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%di_test
exponent=5;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*(i+0.4))^exponent;
end
b=b.';
doubleDisp(b);

c = a.^exponent;
doubleDisp(c);

%dd_test
exponent=1.4;
a = zeros(3);
for i=1:9
	a(i) = (i+0.4);
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4)^exponent;
end
b=b.';
doubleDisp(b);

c = a.^exponent;
doubleDisp(c);

%neg_dd_test
exponent=1.4;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*(i+0.4))^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%dc_test
exponent=-0.5*I;
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
doubleDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4)^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%ci_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = i+0.5*I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5*I)^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%cd_test
exponent=-0.9;
a = zeros(3);
for i=1:9
	a(i) = i+0.5*I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5*I)^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%cc_test
exponent=2-2*I;
a = zeros(3);
for i=1:9
	a(i) = i+0.5*I;
end
a=a.';
complexDisp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5*I)^exponent;
end
b=b.';
complexDisp(b);

c = a.^exponent;
complexDisp(c);

%overflow_test
exponent=2;
a = zeros(3);
for i=1:9
	a(i) = INT_MAX;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = INT_MAX^exponent;
end
b=b.';
doubleDisp(b);

c = a.^exponent;
doubleDisp(c);

%underflow_test
exponent=2;
a = zeros(3);
for i=1:9
	a(i) = INT_MIN;
end
a=a.';
intDisp(a);

b = zeros(3);
for i=1:9
	b(i) = INT_MIN^exponent;
end
b=b.';
doubleDisp(b);

c = a.^exponent;
doubleDisp(c);