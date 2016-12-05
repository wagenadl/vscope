function xx = vscope_debleach(xx, ord, skipstart, skipend)
% VSCOPE_DEBLEACH - Debleach vsd data
%    yy = VSCOPE_DEBLEACH(xx, ord) subtracts an n-th order polynomial from
%    the data XX, but not the 0-th order term.
%    yy = VSCOPE_DEBLEACH(xx, ord, skipstart, skipend) ignores the first
%    SKIPSTART and the last SKIPEND frames for the fit.
%    (The default is to ignore 2 frames at the start and 3 at the end.)
%    If XX is multidimensional, this function operates on the first dimension.
%    If ORD is 0, we do an "exponential+constant" fit. This requires PHYSFIT.
%    If ORD<0, we use SALPA with tau=-ord. In this case, skipstart and skipend
%    are ignored.

if nargin<3
  skipstart=2;
end
if nargin<4
  skipend=3;
end

S = size(xx);
T = S(1);
N = prod(S)/T;
xx = reshape(xx, [T N]);

if ord<0
  for n=1:N
    m0 = mean(xx(:,n));
    yy = salpa(xx(:, n) - m0, 'tau', -ord) + m0;
    yy(isnan(yy)) = m0; % VSCOPE does not like SALPA's NANs
    xx(:, n) = yy;
  end
  return;
end

tt=[1:T]';
tidx=[1+skipstart:T-skipend]';
tt=tt-mean(tt(tidx));

for n=1:N
  if any(isnan(xx(:,n)))
    xx(:,n) = nan;
  elseif ord==0
    p = physfit('expc', tt(tidx), xx(tidx,n));
    xx(:,n) = xx(:,n) - physfit_apply(p, tt)  + mean(xx(:,n));
  else
    p = polyfit(tt(tidx), xx(tidx,n), ord);
    for k=1:ord
      xx(:,n) = xx(:,n) - p(1+ord-k)*tt.^k;
    end
  end
end

xx = reshape(xx, S);