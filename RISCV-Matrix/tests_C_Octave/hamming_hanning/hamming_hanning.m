more off
format short

source octaveIncludes.m;


iterations = 100;

for i=1:iterations
	doubleDisp(hamming(i));
	doubleDisp(hamming(i, "periodic"));
	doubleDisp(hanning(i));
	doubleDisp(hanning(i, "periodic"));
end