function img = microshiftrois(img, dx, dy, rois)
% MICROSHIFTROIS - Microshifted image
%    img = MICROSHIFTROIS(img, dx, dy, rois) shifts the image IMG by 
%    DX and DY pixels in the horizontal (second) and vertical (first)
%    dimensions. 
%    Only pixels inside the ROIS are shifted; DX and DY should be vectors.
%    Edge pixels are not trimmed.

[Y X] = size(img);
N = length(rois);
im0 = img;
for n=1:N
  x0 = min(rois(n).x);
  x1 = max(rois(n).x);
  y0 = min(rois(n).y);
  y1 = max(rois(n).y);
  im1 = im0(y0-1:y1+1, x0-1:x1+1);
  [Y_ X_] = size(im1);
  if dx(n)~=0
    im1 = interp1(im1', [2:X_-1]+dx(n), 'linear')';
  else
    im1 = im1(:, 2:end-1);
  end
  if dy(n)~=0
    im1 = interp1(im1, [2:Y_-1]+dy(n), 'linear');
  else
    im1 = im1(2:end-1, :);
  end
  img(y0:y1, x0:x1) = im1;
end
