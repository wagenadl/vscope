function img = microshift(img, dx, dy)
% MICROSHIFT - Microshifted image
%    img = MICROSHIFT(img, dx, dy) shifts the image IMG by DX and DY pixels
%    in the horizontal (second) and vertical (first) dimensions. 
%    The edge pixels are not trimmed, but they may become invalid (nan)

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

if dx~=0
  xx=[1:X]+dx;
  img = interp1(img', xx, 'linear')';
end

if dy~=0
  yy=[1:Y]+dy;
  img = interp1(img, yy, 'linear');
end

