function [rat, don, acc] = vscope_ratio(ccd, roi, deblch)
% VSCOPE_RATIO - Extract ratioed data from CCD images
%    rat = VSCOPE_RATIO(ccd, roi, deblch) calculates the ratio between
%    the donor fluorescence change and the acceptor fluorescence change.
%    CCD must be as from VSCOPE_LOAD. 
%    ROI must either be polyblob or xyrra style. (That is, something that
%    VSCOPE_ROICOORDS understands.) If ROI is a cell array, results for
%    all ROIs will be returned in a TxN matrix.
%    DEBLCH is optional, and can be 0 for no debleaching, 1 for linear
%    debleaching or 2 for quadratic debleaching.
%    [rat, don, acc] = VSCOPE_RATIO(...) also returns the donor and acceptor
%    traces.
%    rat = VSCOPE_RATIO(x, debleach) where X is straight from VSCOPE_LOAD
%    is also permitted.

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

if nargin<3
  deblch=0;
end

if isfield(ccd, 'ccd')
  if nargin>=2
    deblch = roi;
  end
  roi = ccd.rois;
  ccd = ccd.ccd;
end

dat = vscope_extractrois(ccd, roi);

[T K C] = size(dat);

rat = zeros(T,K);
don = zeros(T,K);
acc = zeros(T,K);

for k=1:K
  px = vscope_debleach(squeeze(dat(:,k,:)), deblch);
  don(:,k) = px(:,1) / mean(px(:,1)) - 1;
  if C>=2
    acc(:,k) = px(:,2) / mean(px(:,2)) - 1;
  else
    acc(:,k) = 0;
  end
  rat(:,k) = don(:,k) - acc(:,k);
end  

if nargout<3
  clear acc
end
if nargout<2
  clear don
end
