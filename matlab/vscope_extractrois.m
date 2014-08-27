function [dat] = vscope_extractrois(ccd, roi)
% VSCOPE_EXTRACTROIS - Extract averages from ROIs in CCD images
%    dat = VSCOPE_EXTRACTROIS(ccd, rois) extracts the number of photons
%    in each ROI. This currently uses a fixed PHOTONS_PER_DIGI value.
%    DAT will be shaped TxNxC where T is the number of frames, N the number
%    of ROIs, and C the number of cameras.
%    dat = VSCOPE_EXTRACTROIS(x) where X is straight from VSCOPE_LOAD also
%    works.

if nargin==1
  roi = ccd.rois;
  roicams = ccd.roicams;
  ccd = ccd.ccd;
else
  roicams = {};
end

PHOTONS_PER_DIGI = 4.25;
DIGI_BASE = 1000;

[Y X C T] = size(ccd.dat);
for c=1:C
  p = vscope_roicoords_cam(roi, ccd.info, c);
  N = length(p);
  for n=1:N
    idx{n,c} = p(n).y + (p(n).x-1)*Y;
  end
end

ccd.dat = reshape(ccd.dat,[X*Y C T]);

dat = zeros(T,N,C) + nan;

for n=1:N
  for c=1:C
    if isempty(roicams) || ~isempty(strmatch(ccd.info.camid{c}, roicams{n}, 'exact'))
      dat(:,n,c) = squeeze(sum(ccd.dat(idx{n,c},c,:)-DIGI_BASE,1));
    end
  end
end

dat = dat * PHOTONS_PER_DIGI;
