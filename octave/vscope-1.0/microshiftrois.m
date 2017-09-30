function img = microshiftrois(img, dx, dy, rois)
% MICROSHIFTROIS - Microshifted image
%    img = MICROSHIFTROIS(img, dx, dy, rois) shifts the image IMG by 
%    DX and DY pixels in the horizontal (second) and vertical (first)
%    dimensions. 
%    Only pixels inside the ROIS are shifted; DX and DY should be vectors.
%    Edge pixels are not trimmed.

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
