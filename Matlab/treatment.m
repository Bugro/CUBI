function [imgAAfficher]=treatment(imgATraiter)

imgNoir = rgb2gray(imgATraiter);
result = edge(imgNoir, 'log');

imgAAfficher=result;
end