function [mu, sd] = normfit(fileID, a)
	mu = mean(a);
	sd = std(a);
end