A = [1, 2.1, 1;
    3, 4, 1];
A_transposed = A';
B = A * A_transposed;
B_scaled = 3 * B;
[F, G] = myfun1(1, 2);
function [F, G] = myfun1(f, g)
    F = f + g;
    G = f - g;
end