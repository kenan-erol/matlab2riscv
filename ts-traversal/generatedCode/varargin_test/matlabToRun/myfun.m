function B = myfun(fileID, A, varargin)
% Inputs:
%   A: int matrix
% Varargin:
%   a: int
%   b: int matrix
%   c: double
% Outputs:
%   B: int matrix

B = 2*A;

if (nargin - 1) > 3
    a = varargin{1};
    b = varargin{2};
    c = varargin{3};
    dispArr(fileID, sprintf("a: %d\nb:\n", a));
    dispArr(fileID, b);
    dispArr(fileID, sprintf("c: %.4f\n", c));
    
    
elseif (nargin - 1) > 2
    a = varargin{1};
    b = varargin{2};
    dispArr(fileID, sprintf("a: %d\nb:\n", a));
    dispArr(fileID, b);
    
elseif (nargin - 1) > 1
    a = varargin{1};
    dispArr(fileID, sprintf("a: %d\n", a));
else 
    dispArr(fileID, "No extra inputs")
    
end

end