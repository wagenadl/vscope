function vscope_cohplotsignals(coh, varargin)
% VSCOPE_COHPLOTSIGNALS - Proof sheet of coherent signals
%    VSCOPE_COHPLOTSIGNALS(coh) plots the results of VSCOPE_COHANALYSIS.
%    VSCOPE_COHPLOTSIGNALS(coh, key, value, ...) specifies additional 
%    parameters:
%       qpt - filename for QPlot figure (empty for current)
%       width - width of QPlot figure (inch)
%       height - height of QPlot figure
%       thr - override threshold for plotting. set to zero to plot all

kv = getopt('qpt=''/tmp/vscope_coh_signals'' width=5 height=5 thr=[]', ...
    varargin);

if isempty(kv.thr)
  idx = find(coh.mag >= coh.thr);
else
  idx = find(coh.mag >= kv.thr);
end

kv.height
if isempty(kv.height)
  kv.height = kv.width * (3 + length(idx))/30;
end

if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end


prio = coh.mag .* cos(coh.phase);

[prio, ord] = sort(prio(idx));
idx = idx(ord);

N = length(idx);

sig = bsxfun(@rdivide, coh.extra.sig, mean(coh.extra.sig0));

sd = std(sig(:,idx));
sd = sort(sd);
scl = sd(ceil(.75*N)) * 5;

qpen 1
for n=1:N
  qpen(coh.cc(idx(n), :));
  qplot(coh.extra.tt, sig(:,idx(n))/scl + n);
  qat(coh.extra.tt(1), n);
  qalign right middle
  qtext(-5, 0, vscope_roiid(idx(n)));
end

dx = sensiblestep((coh.extra.tt(end)-coh.extra.tt(1)) / 3);
rng = [ceil(coh.extra.tt(1)/dx)*dx : dx : floor(coh.extra.tt(end)/dx)*dx];
qpen k 0
qxaxis(0, [coh.extra.tt(1) coh.extra.tt(end)], rng, 'Time (s)');

dy = sensiblestepup(scl);
qpen k 2
qaxshift 10
qytitlerot 0
qyaxis('r', coh.extra.tt(end), [-dy/scl 0]+n+.2, [], sprintf('%g %%', dy*100));

qshrink 1
