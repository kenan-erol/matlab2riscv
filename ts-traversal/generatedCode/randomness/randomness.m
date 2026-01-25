%more off
%format short

%source octaveIncludes.m;


% rand_test
rand("seed", 1);
a = rand(3, 4);
s1 = rand("seed");

rand("seed", 1);
b = rand(3, 4);
s2 = rand("seed");


%randi_test
rand("seed", "reset");
a = randi(10, 3, 4);
disp(a <= 10);


%randn_test
rand("seed", "reset");
a = randn(1000);
mu = mean(a);
sigma = var(a);