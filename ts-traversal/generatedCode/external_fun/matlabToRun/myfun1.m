function [F, G] = myfun1(fileID, f,g)
    F = f + g;
    G = f - g;
    for i = 1:2:5
        dispArr(fileID, i);
    end
end