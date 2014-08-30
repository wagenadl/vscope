function x = vscope_downsample(x, newsize)
% VSCOPE_DOWNSAMPLE - Reduce resolution of CCD images in VSCOPE data
%   y = VSCOPE_DOWNSAMPLE(x, newsize) where X is the result of VSCOPE_LOAD,
%   takes the CCD data in X and downsamples it to NEWSIZE.
%   NEWSIZE may be either a [W H] tuple or just a single number that will
%   be used for both width and height.
%   This function will scale "blob" style ROIs correctly. "xyrra" style is
%   supported by conversion to blob.
%   NEWSIZE *must* evenly divide the old size of the images
%   The CCD transformation matrices in Y will be unity.
%   Note that ROI coordinates are counted from 0, not 1, on input and output.
%   Take care in Matlab!

% Note that x.ccd.dat = Y x X x CAMS x T
% and that x.rois{n} = [xvec yvec];

if prod(size(newsize))==1
  newsize = [newsize newsize];
end

S = size(x.ccd.dat);
Sy = S(1);
NSy = newsize(2);
Ry = Sy/NSy;

Sx = S(2);
NSx = newsize(1);
Rx = Sx/NSx;

x.ccd.dat = reshape(mean(mean(reshape(x.ccd.dat,[Ry NSy Rx NSx S(3:end)]), ...
    1), 3),[NSy NSx S(3:end)]);

N = length(x.rois);
for n=1:N
  xy = vscope_roioutline(x.rois{n});
  if ~isempty(xy)
    xform = x.ccd.info.xform{1}; % Should be the same for all cams in this dataset
    xx = (xy(:,1) - xform.bx) / xform.ax;
    yy = (xy(:,2) - xform.by) / xform.ay;
    x.rois{n} = [xx/Rx yy/Ry];
  end
end

for k=1:length(x.ccd.info.xform)
  x.ccd.info.xform{k}.ax = 1;
  x.ccd.info.xform{k}.ay = 1;
  x.ccd.info.xform{k}.bx = 0;
  x.ccd.info.xform{k}.by = 0;
end
