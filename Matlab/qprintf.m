function nbytes = qprintf(value,uncertainty,unit)
%QPRINTF Prints value, uncertainty and unit of a quantity
uncertainty = roundn(abs(uncertainty),2,'significant');
n = -floor(log10(abs(uncertainty)));
value = roundn(value,n+1);
m = ceil(log10(abs(value)));

if nargin < 3
    nbytes = fprintf('%#.*g +/- %#3.2g', m+n+1, value, uncertainty);
else
    nbytes = fprintf('(%#.*g +/- %#3.2g) %s', m+n+1, value, uncertainty,unit);
end
end