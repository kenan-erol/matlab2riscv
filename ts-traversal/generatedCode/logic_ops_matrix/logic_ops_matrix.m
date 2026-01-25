%more off
%format short

%source octaveIncludes.m;

a = [0, 1, 0;
    1, 0, 1;
    0, 0, 0];

disp(a);

disp(~a);

disp(~zeros(3));
disp(~ones(3));
disp(~eye(3));

disp(a & zeros(3));
disp(a & ~eye(3));

disp(zeros(3) | ~ones(3));
disp(~ones(3) | eye(3));
