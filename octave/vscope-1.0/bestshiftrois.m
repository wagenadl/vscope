function dx = bestshiftrois(img, rf1, rf2, rois)
% BESTSHIFTROIS - Calculate best shift for image based on two reference images
%   dx = BESTSHIFT(img, rf1, rf2, rois) where RF1 and RF2 are two copies of a 
%   reference image microshifted by 0.5 pixels in opposite directions, 
%   returns the optimal microshift amounts for IMG to match the original
%   reference image for each of the ROIs in ROIS, which much be a struct array
%   from VSCOPE_ROICOORDS_CAM.

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
[Y_ X_] = size(rf1);
N = length(rois);
idx = cell(N,1);
if Y_<Y
  img = img(2:end-1, 2:end-1);
  for n=1:N
    use = find(rois(n).x>1 & rois(n).x<X & rois(n).y>1 & rois(n).y<Y);
    idx{n} = Y_*(rois(n).x(use)-2) + rois(n).y(use)-1;
  end
else
  for n=1:N
    idx{n} = Y_*(rois(n).x-1) + rois(n).y;
  end
end

dx = zeros(N, 1);
for n=1:N
  if ~isempty(idx{n})
    drf = rf1(idx{n}) - rf2(idx{n});
    a = sum(drf.*(img(idx{n})-rf2(idx{n}))) ./ sum(drf.^2);
    dx(n) = a - 0.5;
  end
end
