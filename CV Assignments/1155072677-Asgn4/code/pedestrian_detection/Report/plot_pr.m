clear;clc;
fileID = fopen('report.txt');
res = fread(fileID,'float');
ap = res(1);
res(1) = [];
rec = res(1:end/2)';
prec = res(end/2+1:end)';
plot(rec,prec,'-');
grid;
xlabel 'recall'
ylabel 'precision'
title(sprintf('AP = %.3f',ap));