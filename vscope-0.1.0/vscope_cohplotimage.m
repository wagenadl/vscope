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
%       ns - mark nonsignificant cells (1=dots, 2=id, 0=none, 3=ROI)
%    VSCOPE_COHPLOTIMAGE(coh, key, value, ...) also works since 8/11/15, because
%    the only thing we actually needed from the X structure, the ROIS field,
%    is now also stored in the COH structure.

if isfield(x, 'extra')
  vscope_cohplotimage([], coh, varargin{:});
end

kv = getopt('qpt=''/tmp/vscope_coh_image'' width=5 height=5 alpha=1 ns=0', ...
    varargin);
if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

if isempty(x)
  clear x
  x.rois = coh.extra.rois;
end

[xx,yy] = vscope_roioutline(x.rois);
idx = find(coh.mag >= coh.thr);

% Draw image
qimsc(coh.extra.xx, -coh.extra.yy, coh.extra.img);

if kv.ns==1
  % Draw dots in n.s. cells
  idns = find(coh.mag < coh.thr);
  qpen k
  qmarker o 3 solid
  for k = idns
    if ~isempty(xx{k})
      qmark(mean(xx{k}), -mean(yy{k}));
    end
  end
elseif kv.ns==3
  % Draw ROIs in n.s. cells
  idns = find(coh.mag < coh.thr);
  qpen none
  qbrush 777
  qbrush(kv.alpha);
  for k = idns
    if ~isempty(xx{k})
      qpatch(xx{k}, -yy{k});
    end
  end
end

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
if kv.ns==2
  idx = [1:length(coh.mag)];
end
qbrush none
qpen k
for k = idx
  if ~isempty(xx{k})
    qat(mean(xx{k}), -mean(yy{k}));
    qalign center middle
    qtext(0, 0, vscope_roiid(k));
  end
end
