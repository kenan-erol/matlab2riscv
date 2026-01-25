function double_reindexing_tests(fileID, a)
	a=a.';
	dispArr(fileID, [a(1)]);
	dispArr(fileID, [a(4)]);
	dispArr(fileID, [a(1) ; a(2) ; a(3) ; a(4)]);
	dispArr(fileID, [a(4) ; a(3) ; a(2) ; a(1)]);
	dispArr(fileID, [a(2) ; a(2) ; a(2)]);
	dispArr(fileID, [a(1) ; a(2) ; a(2) ; a(3) ; a(3) ; a(3) ; a(4) ; a(4) ; a(4) ; a(4)]);
end