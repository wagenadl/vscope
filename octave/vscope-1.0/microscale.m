 function img = microscale(img, dx)
% MICROSCALE - Microscaleed image
%    img = MICROSCALE(img, dx) scales the image IMG by DX 'pixels'
%    in both dimensions. Pixels are measured in the finer dimension 
%    (usually, X); the scale is isotropic assuming the image is square.
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
R = max(Y,X)/2;

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = (xx-x0)*(1+dx/R)+x0;
yy = (yy-y0)*(1+dx/R)+y0;
xx = repmat(xx, [Y 1]);
yy = repmat(yy, [1 X]);
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');
