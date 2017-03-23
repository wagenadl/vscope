 function img = microperspective(img, dx, dy)
% MICROPERSPECTIVE - Microperspectived image
%    img = MICROPERSPECTIVE(img, dx, dy) perspectivises the image IMG by
%    DX pixels in x and DY pixels in y. That's at the extreme edge of
%    the figure.
%    Edge pixels may become nan.

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

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = repmat(xx - x0, [Y 1]);
yy = repmat(yy - y0, [1 X]);
RX = X/2;
RY = Y/2;
if dx~=0
  x_ = xx.*(1+dx*yy/RX/RY);
end
if dy~=0
  yy = yy.*(1+dy*xx/RX/RY);
end
if dx~=0
  xx=x_;
end
xx = xx+x0;
yy = yy+y0;
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');

