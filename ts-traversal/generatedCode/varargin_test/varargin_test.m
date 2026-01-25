A = [1, 2, 3];
B = myfun(A, 3, A, 4.1);

B = myfun(A, 2);

function B = myfun(A, varargin)
% Inputs:
%   A: int matrix
% Varargin:
%   a: int
%   b: int matrix
%   c: double
% Outputs:
%   B: int matrix

B = 2*A;

if nargin > 3
    a = varargin{1};
    b = varargin{2};
    c = varargin{3};
    disp(sprintf("a: %d\nb:\n", a));
    disp(b);
    disp(sprintf("c: %.4f\n", c));
    
    
elseif nargin > 2
    a = varargin{1};
    b = varargin{2};
    disp(sprintf("a: %d\nb:\n", a));
    disp(b);
    
elseif nargin > 1
    a = varargin{1};
    disp(sprintf("a: %d\n", a));
else 
    disp("No extra inputs")
    
end

end