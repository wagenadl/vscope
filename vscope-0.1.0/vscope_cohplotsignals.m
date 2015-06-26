function vscope_cohplotsignals(coh, varargin)
% VSCOPE_COHPLOTSIGNALS - Proof sheet of coherent signals
%    VSCOPE_COHPLOTSIGNALS(coh) plots the results of VSCOPE_COHANALYSIS.
%    VSCOPE_COHPLOTSIGNALS(coh, key, value, ...) specifies additional 
%    parameters:
%       qpt - filename for QPlot figure (empty for current)
%       width - width of QPlot figure (inch)
%       height - height of QPlot figure
%       threshold - override threshold for plotting. Set to zero to plot all.
%       color - 0: use blue and black on alternating traces (default)
%               1: use colors from the polar plot for traces
%       uniform - 0: let each trace have its own scale
%                 1: force all traces to the same scale
%       order - 0: in ROI id order (default)
%               1: most significant on top
%               2: phase 0 on top, phase 180 on bottom
%               3: phase 90 on top, phase 270 on bottom
%               4: phase 0 on top, phase 359 on bottom
%       nmax - maximum number of signals to plot (default: all)
%       tbar - 0: plot x-axis for time
%              1: plot scale bar for time
%       sbar - 0: label dF scale next to x-axis
%              1: label dF scale next to scale bar

kv = getopt([ 'qpt=''/tmp/vscope_coh_signals'' width=5 height=5 ' ...
      'threshold=[] color=0 uniform=0 order=0 nmax=[] tbar=0 sbar=1' ], ...
    varargin);

if isempty(kv.threshold)
  idx = find(coh.mag >= coh.thr);
else
  idx = find(coh.mag >= kv.threshold);
end

% Use only those times for calculating scale that were used for coherence
tidx = coh.extra.tt0>=coh.extra.tt(1) & coh.extra.tt0<=coh.extra.tt(end);

if isempty(kv.height)
  kv.height = kv.width * (3 + length(idx))/30;
end

if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

if kv.order==0
  % ROI ID order: lowest ID on top
  sortkey = -[1:length(coh.mag)];
elseif kv.order==1
  sortkey = coh.mag;
elseif kv.order==2
  sortkey = coh.mag .* cos(coh.phase);
elseif kv.order==3
  sortkey = coh.mag .* sin(coh.phase);
elseif kv.order==4
  sortkey = -mod(-coh.phase, 2*pi);
else
  error('Invalid value for plotting order');
end

if isempty(kv.nmax)
  kv.nmax = inf;
end

[dd, ord] = sort(sortkey(idx));
idx = idx(ord);
N = length(idx);

if N > kv.nmax
  idx = idx(N-kv.nmax+1:N);
  N = length(idx);
elseif N==0
  warning('vscope_cohplotsignals: nothing to plot');
  return
end

sig = bsxfun(@rdivide, coh.extra.sig0(2:end,:), mean(coh.extra.sig0)) - 1;

if kv.uniform
  sd = std(sig(tidx(2:end), idx));
  sd = sort(sd);
  scl = repmat(sd(ceil(.75*N)) * 5, [N 1]);
else
  scl = 5*std(sig(tidx(2:end), idx));  
  if isempty(scl)
    s0 = 1;
  else
    s0 = median(scl);
  end
  scl(scl<s0) = s0;
end
if isempty(scl)
  dy = 1;
else
  dy = sensiblestep(.95*min(scl));
end

t1 = coh.extra.tt0(end);
qpen 1 roundcap
for n=1:N
  if all(isnan(sig(:,idx(n))))
    continue;
  end
  if kv.color
    qpen(coh.cc(idx(n), :));
  elseif mod(n,2)>0
    qpen k
  else
    qpen b
  end
  qplot(coh.extra.tt0(2:end), ...
      (sig(:,idx(n))-mean(sig(tidx(2:end),idx(n))))/scl(n) + n);
  qat(coh.extra.tt0(1), n);
  qalign right middle
  qtext(-5, 0, vscope_roiid(idx(n)));
  if ~kv.uniform
    qpen 2 flatcap
    qgline({'absdata', t1, n+dy/scl(n)/2, 'relpaper', 10, 0}, ...
	{'absdata', t1, n-dy/scl(n)/2, 'relpaper', 10, 0});
    if kv.sbar==1
      qat(t1, n);
      qalign left middle
      qtext(15, 0, sprintf('%g %%', dy*100));
    end
    qpen 1 roundcap
  end
end

if kv.tbar==0
  dx = sensiblestep((t1-coh.extra.tt0(1)) / 3);
  rng = [ceil(coh.extra.tt0(1)/dx)*dx : dx : floor(t1/dx)*dx];
  qpen k 0
  qxaxis(0.25, [coh.extra.tt0(1) t1], rng, 'Time (s)');
else
  dx = sensiblestep((t1-coh.extra.tt0(1)) / 5);
  qpen k 2 flatcap
  qplot([t1-dx t1], [0 0]+.25);
  qat(t1-dx/2, 0+.25);
  qalign center top 
  qtext(0, 5, sprintf('%g s', dx));
end

if kv.sbar==0
  qat(t1, 'bottom');
  qalign right bottom
  qtext(0, 0, sprintf('Scale: %g %%', dy*100));
end

if kv.uniform
  dy = sensiblestepup(dy*1.2);
  qpen k 2 flatcap
  qgline({'absdata', t1, N+dy/scl(1)/4, 'relpaper', 10, 0}, ...
      {'absdata', t1, N-3*dy/scl(1)/4, 'relpaper', 10, 0});
  if kv.sbar==1
    qat(t1, N-dy/scl(1)/4);
    qalign left middle
    qtext(15, 0, sprintf('%g %%', dy*100));
  end
end

qshrink 1
