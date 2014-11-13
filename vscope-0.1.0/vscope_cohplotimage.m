function vscope_cohplotimage(x, coh, varargin)
% VSCOPE_COHPLOTIMAGE - Color ROIs by coherence on top of CCD image
%    VSCOPE_COHPLOTIMAGE(x, coh) plots the results of VSCOPE_COHANALYSIS on
%    top of an image.
%    X must be from VSCOPE_LOAD.
%    VSCOPE_COHPLOTIMAGE(x, coh, key, value, ...) specifies additional 
%    options:
%       qpt - filename for QPlot figure (empty for current)
%       width - width of QPlot figure (inch)
%       height - height of QPlot figure
%       alpha - transparency (1=solid, 0=invisible) of ROIs

kv = getopt('qpt=''/tmp/vscope_coh_image'' width=5 height=5, alpha=1', ...
    varargin);
if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

[xx,yy] = vscope_roioutline(x.rois);
idx = find(coh.mag >= coh.thr);

% Draw image
qimsc(coh.extra.xx, -coh.extra.yy, coh.extra.img);

% Draw ROIs
qpen none
for k = idx
  if ~isempty(xx{k})
    qbrush(coh.cc(k,:));
    qbrush(kv.alpha); % Opacity
    qpatch(xx{k}, -yy{k});
  end
end

% Draw labels
qbrush none
qpen k
for k = idx
  if ~isempty(xx{k})
    qat(mean(xx{k}), mean(-yy{k}));
    qalign center middle
    qtext(0, 0, vscope_roiid(k));
  end
end
