function img = vscope_cohprettyimage(coh, varargin)
% VSCOPE_COHPRETTYIMAGE - Pretty version of VSCOPE_COHPLOTIMAGE
%    VSCOPE_COHPRETTYIMAGE(coh) creates an elegant overlay of ROIs
%    over a CCD image.
%    The result is prettier than plain-old VSCOPE_COHPLOTIMAGE, because
%    the CCD image is first upsampled to 512x512 pixels and coloration
%    is handled in a more elegant manner.
%    img = VSCOPE_COHPRETTYIMAGE(coh) returns an RGB image rather than
%    plotting the result in a QPlot window.
%    VSCOPE_COHPRETTYIMAGE(coh, key, val, ...) specifies additional
%    options:
%      magcurve - function to apply to coh.mag for creating saturation.
%      imgcurve - function to apply to source image brightness
%      maggamma - gamma correction for coh.mag
%      imggamma - gamma correction for image
%      qpt      - alternative name for QPlot window

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

kv = getopt('label=0 nslabel=0 labelcolor=''w'' imggamma=1 imgcurve=[] maggamma=1 magcurve=[] qpt=''/tmp/vscope_cohprettyimage''', varargin);

img = double(coh.extra.img);
[Y X] = size(img);
SY = 512/Y;
SX = 512/X;
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
  
[xx, yy] = vscope_roicoords(coh.extra.rois);
R = length(xx);


s0 = 'cielchuv';
imr = repmat(img, [1 1 3])/max(img(:)); % make RGB image
imr = reshape(imr, [512*512 3]);
cc0 = colorconvert([50 0 0], ...
      'from', s0, 'to', 'srgb');
gg = exp(-.5*[-5:5].^2/2^2); gg=gg./sum(gg);
for r=1:R
  if ~isnan(coh.mag(r))
    idx = (xx{r}-1)*512 + yy{r};
    rimg = zeros(512,512);
    rimg(idx) = 1;
    rimg = convn(convn(rimg, gg, 'same'), gg', 'same');
    rimc = repmat(rimg(:), [1 3]);
    mg = coh.mag(r).^kv.maggamma;
    if ~isempty(kv.magcurve)
      mg = feval(kv.magcurve, mg);
    end
    cc = colorconvert([50 75*mg, coh.phase(r)+pi/2], ...
	'from', s0, 'to', 'srgb');
    rimc = rimc .* (cc./cc0);
    
    imr = imr .* (1 + rimc - rimg(:));
  end
end

imr = reshape(imr,[512 512 3]);
imr(:,:,4) = 1;
imr = imr ./ max(imr, [], 3);
img = imr(:,:,1:3);
if nargout==0
  qfigure(kv.qpt, 8, 8);
  qimage(img);
  clear img
end


