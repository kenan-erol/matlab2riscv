%more off
%format short

%source octaveIncludes.m;

% trueTest
a = zeros(3);

for i=1:9
	a(i) = i*i;
end
a = a.';

disp(a);

b=a;
disp(b);

c=a;
disp(c);

d=a;
disp(d);

disp((a==b)&(a==c)&(a==d));

% falseTest
disp(a);
disp(b);

c = zeros(3);
for i=1:9
	c(i) = i*i;
end
c(2) = 10;
c(3) = 11;
c(6) = 12;
c = c.';
disp(c);

d = zeros(3);
for i=1:9
	d(i) = i*i;
end
d(4) = 13;
d(7) = 14;
d(8) = 15;
d = d.';
disp(d);

disp((a==b)&(a==c)&(a==d));