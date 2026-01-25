function B = myfun(fileID, A)
    B = A';
    dispArr(fileID, B(end));
    dispArr(fileID, B(end, 1));
end