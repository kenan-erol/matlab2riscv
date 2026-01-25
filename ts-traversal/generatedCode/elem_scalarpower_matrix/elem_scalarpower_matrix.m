%more off
%format short

%source octaveIncludes.m;

%ii_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*i)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%id_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%neg_id_test
exponent=1.2;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*i)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%ic_test
exponent=4+0.3i;
a = zeros(3);
for i=1:9
	a(i) = i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = i^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%di_test
exponent=5;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*(i+0.4))^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%dd_test
exponent=1.4;
a = zeros(3);
for i=1:9
	a(i) = (i+0.4);
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%neg_dd_test
exponent=1.4;
a = zeros(3);
for i=1:9
	a(i) = ((-1)^(i+1))*(i+0.4);
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (((-1)^(i+1))*(i+0.4))^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%dc_test
exponent=-0.5i;
a = zeros(3);
for i=1:9
	a(i) = i+0.4;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.4)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%ci_test
exponent=3;
a = zeros(3);
for i=1:9
	a(i) = i+0.5i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5i)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%cd_test
exponent=-0.9;
a = zeros(3);
for i=1:9
	a(i) = i+0.5i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5i)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%cc_test
exponent=2-2i;
a = zeros(3);
for i=1:9
	a(i) = i+0.5i;
end
a=a.';
disp(a);

b = zeros(3);
for i=1:9
	b(i) = (i+0.5i)^exponent;
end
b=b.';
disp(b);

c = a.^exponent;
disp(c);

%overflow_test
%exponent=2;
%a = zeros(3);
%for i=1:9
%	a(i) = INT_MAX;
%end
%a=a.';
%disp(a);

%b = zeros(3);
%for i=1:9
%	b(i) = INT_MAX^exponent;
%end
%b=b.';
%disp(b);

%c = a.^exponent;
%disp(c);

%underflow_test
%exponent=2;
%a = zeros(3);
%for i=1:9
%	a(i) = INT_MIN;
%end
%a=a.';
%disp(a);

%b = zeros(3);
%for i=1:9
%	b(i) = INT_MIN^exponent;
%end
%b=b.';
%disp(b);

%c = a.^exponent;
%disp(c);