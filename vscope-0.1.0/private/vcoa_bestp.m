function [nbest, thrbest] = vcoa_bestp(tt, y_ref, y_sig, mag, kv)
% VCOA_BESTP - Helper function for VSCOPE_COHANALYSIS to find optimal p-value
%   [nbest, thrbest] = vcoa_bestp(tt, y_ref, y_sig, mag, kv)

% If there are k significant cells at p-value p0/n, then by
% definition there are <=nk significant cells at p-values p0/m for m>n,
% and there are >=k significant cells at p-values p0/m for m<n.
% This rule may be broken by randomness, but I think we can ignore that.
% We cannot accept k>n. We could accept k<n, but we'd _like_ k=n.
n0 = 1; % Minimum still in range
n1 = length(mag); % Maximum still in range
nbest = nan;
kbest = 0;
thrbest = nan;
kk = zeros(length(mag), 1) + nan;
while n1>n0 || isnan(nbest)
  n = round(sqrt(n0*n1)); % Middle of the range
  if isnan(kk(n))
    fprintf(1, 'Calculating control coherence at %i [%i-%i]\n', n, n0, n1);
    cohc = vscope_cohcontrol(tt, y_ref, y_sig, ...
	'df', kv.df_coh, 'p', -kv.pthresh/n, 'f', kv.f_star);
    k = length(find(mag>=cohc.thr));
    kk(n) = k;
  else
    k = kk(n);
  end
  if k<=n
    % This is acceptable in principle, but it could be that smaller n
    % is better
    if isnan(nbest) || k>kbest || (k==kbest && n>nbest)
      kbest = k;
      nbest = n;
      thrbest = cohc.thr;
    end
    n1 = n - 1;
  elseif k>n
    % This is not acceptable, so we need larger n
    n0 = n + 1;
  end
end

% Now we found our best k value. But is our n really sharpest?
% Perhaps we could increase n yet keep k.
idx = find(kk<kbest, 1); % Find the first known n value that yields worse k
if isempty(idx)
  n1 = length(mag);
else
  n1 = idx - 1;
end
n0 = nbest + 1; % No point looking at what we already have

while n1>n0
  n = round(sqrt(n0*n1)); % Middle of the range
  if isnan(kk(n))
    fprintf(1, 'Calculating control coherence at %i [%i-%i]\n', n, n0, n1);
    cohc = vscope_cohcontrol(tt, y_ref, y_sig, ...
	'df', kv.df_coh, 'p', -kv.pthresh/n, 'f', kv.f_star);
    k = length(find(mag>=cohc.thr));
    kk(n) = k;
  else
    k = kk(n);
  end
  if k>kbest && k<=nbest
    % It's a miracle! But I like it
    kbest = k;
    nbest = n;
    thrbest = cohc.thr;
  elseif k<kbest
    % That's useless
    n1 = n - 1;
  else
    if n>nbest
      nbest = n;
      thrbest = cohc.thr;
    end	  
    n0 = n + 1;
  end
end
