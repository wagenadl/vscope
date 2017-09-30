 function img = microrotate(img, dx)
% MICROROTATE - Microrotated image
%    img = MICROROTATE(img, dx) rotates the image IMG by
%    DX fine pixels at the extreme end. 
%    Edge pixels may become nan as a result.

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

R = max(X,Y)/2;
dx = dx/R; % Convert to radians

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = repmat(xx - x0, [Y 1]);
yy = repmat(yy - y0, [1 X]);
yy = yy*X/Y;
x_ = xx*cos(dx) + yy*sin(dx);
yy = -xx*sin(dx) + yy*cos(dx);
xx = x_;
xx = xx+x0;
yy = yy*Y/X;
yy = yy+y0;
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');


