function [dat, got] = vscope_extractrois(ccd, roi)
% VSCOPE_EXTRACTROIS - Extract averages from ROIs in CCD images
%    dat = VSCOPE_EXTRACTROIS(ccd, rois) extracts the number of photons
%    in each ROI. This currently uses a fixed PHOTONS_PER_DIGI value.
%    DAT will be shaped TxNxC where T is the number of frames, N the number
%    of ROIs, and C the number of cameras.
%    dat = VSCOPE_EXTRACTROIS(x) where X is straight from VSCOPE_LOAD also
%    works.
%    [dat, got] = VSCOPE_EXTRACTROIS(...) returns an additional NxC matrix
%    that reports whether ROI #n exists on camera #c.

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
1
dat = zeros(T,N,C) + nan;
got = logical(zeros(N,C));
for n=1:N
  for c=1:C
    if isempty(roicams) ...
	  || ~isempty(find(strcmp(ccd.info.camid{c}, roicams{n})))
      dat(:,n,c) = squeeze(sum(ccd.dat(idx{n,c},c,:)-DIGI_BASE,1));
      got(n,c) = 1;
    end
  end
end

dat = dat * PHOTONS_PER_DIGI;
