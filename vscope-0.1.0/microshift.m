function img = microshift(img, dx, dy)
% MICROSHIFT - Microshifted image
%    img = MICROSHIFT(img, dx, dy) shifts the image IMG by DX and DY pixels
%    in the horizontal (second) and vertical (first) dimensions. If the
%    original image is YxX images, the output is (Y-2)x(X-2), because edge
%    pixels are trimmed.

[Y X] = size(img);

if dx~=0
  xx=[2:X-1]+dx;
  img = interp1(img', xx, 'linear')';
else
  img = img(:,2:X-1);
end

if dy~=0
  yy=[2:Y-1]+dy;
  img = interp1(img, yy, 'linear');
else
  img = img(2:Y-1,:);
end

img(isnan(img)) = 0;