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

if nargin<3
  deblch=0;
end

[Y X C T] = size(ccd.dat);

xy = vscope_roicoords(roi,X,Y);

if iscell(xy)
  K=length(xy);
  idx=cell(K,1);
  for k=1:K
    idx{k} = xy{k}(:,2) + (xy{k}(:,1)-1)*Y;
  end
else
  K=1;
  idx=cell(K,1);
  idx{1} = xy(:,2) + (xy(:,1)-1)*Y;
end

ccd.dat = reshape(ccd.dat,[X*Y C T]);

tt=[1:T]';
tidx=[3:T-3]';
tt=tt-mean(tt(tidx));

rat = zeros(T,K);
don = zeros(T,K);
acc = zeros(T,K);


for k=1:K
  px = squeeze(mean(ccd.dat(idx{k},:,:)))';
  for q=1:2
    switch deblch
      case 0
	;
      case 1
	p=polyfit(tt(tidx),px(tidx,q),1);
	px(:,q) = px(:,q) - p(1)*tt;
      case 2
	p=polyfit(tt(tidx),px(tidx,q),2);
	px(:,q) = px(:,q) - p(1)*tt.^2 - p(2)*tt;
      otherwise
	error('Unknown debleaching method');
    end
  end
  don(:,k) = px(:,1) / mean(px(:,1)) - 1;
  acc(:,k) = px(:,2) / mean(px(:,2)) - 1;
  rat(:,k) = don(:,k) - acc(:,k);
end

if nargout<3
  clear acc
end
if nargout<2
  clear don
end
