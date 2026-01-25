function dispArr(fileID, A)
    if ischar(A)
       fprintf(fileID, "%s\n", A);
    else
        for l = 0:size(A, 4)-1
            for k = 0:size(A,3)-1
                for i = 0:size(A,1)-1
                    for j = 0:size(A, 2)-1
                        index = i + j*size(A,1) + k*size(A,1)*size(A,2) + ...
                            l*size(A,1)*size(A,2)*size(A,3);
                        if isstring(A)
                            fprintf(fileID, "%s\t", A(index+1));
                        elseif ~isreal(A)
                            real_A = real(A(index+1));
                            imag_A = imag(A(index+1));
                            if isinf(A(index+1))
                                fprintf(fileID, "inf\t");
                            elseif imag_A < 0
                                fprintf(fileID, "%.4f %.4fi\t", real_A, imag_A);
                            else
                                fprintf(fileID, "%.4f +%.4fi\t", real_A, imag_A);
                            end
                        else
                            fprintf(fileID, "%.4f\t", A(index+1));
                        end
                    end
                    fprintf(fileID, "\n");
                end
                fprintf(fileID, "\n");
            end
            fprintf(fileID, "\n");
        end
    end
end