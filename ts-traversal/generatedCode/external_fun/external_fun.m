[F,G] = myfun1(1,2);
b = myfun3(4);
disp(b);

function [F, G] = myfun1(f,g)
    F = f + g;
    G = f - g;
    for i = 1:2:5
        disp(i);
    end
end

function outstr = myfun2()
    outstr = "hello world";
end