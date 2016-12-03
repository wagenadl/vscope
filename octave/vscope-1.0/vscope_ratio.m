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
