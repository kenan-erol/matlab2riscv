more off
format short

source octaveIncludes.m;

%test1
a=[1];
intDisp(a);
disp(det(a));

%test2
a = [26 + 1*I, 3 - 8*I; 20*I, 1 + 25*I];
complexDisp(a);
disp(det(a));

%test3
a = zeros(3);
counter=1;
for i=1:3
	for j=1:3
		a(i,j) = counter*counter;
		counter++;
	end
end
doubleDisp(a);
fprintf('%.5f\n',det(a));

%testn
a = zeros(5);
counter = 0;
for i=0:4
	for j=0:4
		counter++;
		if (mod((counter-1),2) == 0)
			a(counter) = mod((counter + i),7);
		else
			a(counter) = -1*mod((counter + j),7);
		end
	end
end
a=a.';
doubleDisp(a);
fprintf('%.5f\n',det(a));

%non_square
a = zeros(3,2);
a(1) = 26 + 1*I;
a(2) = 3 - 8*I;
a(3) = 20*I;
a(4) = 1 + 25*I;
a(5) = 0;
a(6) = 1;
a=a.';
complexDisp(a);