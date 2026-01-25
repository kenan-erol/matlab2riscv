more off
format short

fid=fopen(argv{1},"a");

tic;
for j=1:str2double(argv{2})
	run(argv{3});
end

a = toc;

fprintf(fid, "%.8f @\n", a);
fclose(fid);

% fprintf("%s, %s, %s\n", argv{1}, argv{2}, argv{3});