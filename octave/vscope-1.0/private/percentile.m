function y=percentile(xx,p)
% y = PERCENTILE(xx,p) returns the p-th percentile of xx.
% Note: P must range 0-100, not 0-1 as for a fractile!
% This is done by interpolating linearly between two nearest data points.

N=length(xx);
if N==0
  y = nan;
  return
end
n=p/100*N;
n0 = max([1 floor(n)]);
n1 = ceil(n);
dn = max([0, n-n0]);

zz = sort(xx);

y = zz(n0)*(1-dn) + zz(n1)*dn;

