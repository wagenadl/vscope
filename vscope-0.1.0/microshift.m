function img = microshift(img, dx, dy)
% MICROSHIFT - Microshifted image
%    img = MICROSHIFT(img, dx, dy) shifts the image IMG by DX and DY pixels
%    in the horizontal (second) and vertical (first) dimensions. 
%    The edge pixels are not trimmed, but they may become invalid (nan)

[Y X] = size(img);

if dx~=0
  xx=[1:X]+dx;
  img = interp1(img', xx, 'linear')';
end

if dy~=0
  yy=[1:Y]+dy;
  img = interp1(img, yy, 'linear');
end

