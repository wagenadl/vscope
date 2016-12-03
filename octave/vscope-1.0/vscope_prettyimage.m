function img = vscope_prettyimage(img, data, rois, varargin)
% VSCOPE_PRETTYIMAGE - Overlay some color info over VSD image
%   img = VSCOPE_PRETTYIMAGE(img, data, rois) takes the image IMG from
%   the CCD, and overlays the per-roi information DATA over it. ROIS must
%   be from VSCOPE_LOAD. Set DATA to NaN for ROIs on other cameras.
%   DATA must be specified as RGB triplets in an Rx3 vector, or in any
%   other color space.

kv = getopt('imggamma=1 imgcurve=[] space=''srgb''', varargin);

img = double(img);
% Normalize:
img = img - min(img(:));
img = img ./ max(img(:));

% Upscale to larger size
R = 512; % VSCOPE_ROICOORDS only works if R=512 for right now
[Y X] = size(img);
SY = R/Y;
SX = R/X;
img = reshape(repmat(reshape(img, [1 Y 1 X]), [SY 1 SX 1]), [SY*Y SX*X]);
if SY>1
  gg = exp(-.5*[-2*SY:2*SY]'.^2/(SY/3)^2);
  gg = gg ./ sum(gg);
  img = convn(img, gg, 'same');
end
if SX>1
  gg = exp(-.5*[-2*SX:2*SX].^2/(SX/3)^2);
  gg = gg ./ sum(gg);
  img = convn(img, gg, 'same');
end

img = img.^kv.imggamma;
if ~isempty(kv.imgcurve)
  img = feval(kv.imgcurve, img);
end

img = img(:);
img = repmat(img, [1 3]);

cc0 = [0.5 0.5 0.5];

gg = exp(-.5*[-5:5].^2/2^2); gg=gg./sum(gg);

[xx, yy] = vscope_roicoords(rois);
N = length(xx);
for n=1:N
  rgb = data(n,:);
  if ~any(isnan(rgb))
    idx = (xx{n}-1) * R  +  yy{n};
    roiimg = zeros([R R]);
    roiimg(idx) = 1;
    roiimg = convn(convn(roiimg, gg, 'same'), gg', 'same'); % Blur gently
    roiimg = roiimg(:);
    colimg = repmat(roiimg, [1 3]);
    cc = colorconvert(rgb, 'from', kv.space, 'to', 'srgb', 'clip', 2);
    colimg = colimg .* (cc./cc0);
    
    img = img .* (1 + colimg - roiimg);
  end
end

img = reshape(img, [R R 3]);
% Normalize:
img = img(:,:,1:3);
