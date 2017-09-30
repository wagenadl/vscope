function str = vscope_cohplotsignals(coh, varargin)
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
%               Alternatively, order may be a vector of ROI numbers to plot
%       labels - An optional vector of alternative labels. Either same length
%                as number of cells, or as length of the "order" vector.
%       nmax - maximum number of signals to plot (default: all)
%       tbar - 0: plot x-axis for time
%              1: plot scale bar for time
%              nan: plot nothing for time
%       sbar - 0: label dF scale next to x-axis
%              1: label dF scale next to scale bar
%       scale - size of scale bar (default: automatic)
%    str = VSCOPE_COHPLOTSIGNALS(...) returns some potentially useful
%    information:
%       yy - y-coordinates of baseline of each of the signals plotted
%       x0, x1 - left and right edges of the signals
%       y0, y1 - bottom and top edges

% This file is part of VScope. (C) Daniel Wagenaar 2008-1017.

% VScope is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% VScope is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with VScope.  If not, see <http://www.gnu.org/licenses/>.

kv = getopt([ 'qpt=''/tmp/vscope_coh_signals'' width=5 height=5 ' ...
      'threshold=[] color=0 uniform=0 order=0 nmax=[] tbar=0 sbar=1 ' ...
      'labels=[] scale=[]' ], ...
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

if isempty(kv.nmax)
  kv.nmax = inf;
end

if length(kv.order)>1
  idx = kv.order(end:-1:1);
  N = length(idx);
  if ~isempty(kv.labels) && length(kv.labels)==N
    kv.labels = kv.labels(end:-1:1);
  end
else
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
end

sig = bsxfun(@rdivide, coh.extra.sig0(2:end,:), mean(coh.extra.sig0)) - 1;
if kv.uniform
  sd = std(sig(tidx(2:end), idx));
  sd = sort(nonan(sd));
  scl = repmat(sd(ceil(.75*length(sd))) * 5, [N 1]);
else
  scl = 5*std(sig(tidx(2:end), idx));  
  if isempty(scl)
    s0 = 1;
  else
    s0 = median(scl);
  end
  scl(scl<s0) = s0;
end
if isempty(kv.scale)
  if isempty(scl)
    dy = 1;
  else
    dy = sensiblestep(.95*min(scl));
  end
else
  dy = kv.scale/2e2;
end

if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

t1 = coh.extra.tt0(end);
qpen 1 roundcap
str.y0 = nan;
str.y1 = nan;
for n=1:N
  if all(isnan(sig(:,idx(n))))
    str.yy(n) = nan;
    continue;
  end
  if kv.color
    qpen(coh.cc(idx(n), :));
  elseif mod(n,2)>0
    qpen k
  else
    qpen b
  end
  x_ = coh.extra.tt0(2:end);
  y_ = (sig(:,idx(n))-mean(sig(tidx(2:end),idx(n))))/scl(n) + n; 

  qplot(x_, y_);
      
  qat(x_(1), n);
  qalign right middle
  if isempty(kv.labels)
    lbl = vscope_roiid(idx(n));
  elseif length(kv.labels)==N
    lbl = kv.labels{n};
  else
    lbl = kv.labels{idx(n)};
  end
  qtext(-5, 0, lbl);
  
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

  str.yy(n) = n;
  str.x0 = x_(1);
  str.x1 = x_(end);
  str.y0 = min(str.y0, min(y_));
  str.y1 = max(str.y1, max(y_));
end

axy = min(str.yy) - .75;

if kv.tbar==0
  dx = sensiblestep((t1-coh.extra.tt0(1)) / 3);
  rng = [ceil(coh.extra.tt0(1)/dx)*dx : dx : floor(t1/dx)*dx];
  qpen k 0
  qxaxis(axy, [coh.extra.tt0(1) t1], rng, 'Time (s)');
elseif kv.tbar==1
  dx = sensiblestep((t1-coh.extra.tt0(1)) / 5);
  qpen k 2 flatcap
  qplot([t1-dx t1], [axy axy]);
  qat(t1-dx/2, axy);
  qalign center top 
  qtext(0, 5, sprintf('%g s', dx));
end

if kv.sbar==0
  qat(t1, 'bottom');
  qalign right bottom
  qtext(0, 0, sprintf('Scale: %g %%', dy*100));
end

if kv.uniform
  N = max(str.yy);
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
