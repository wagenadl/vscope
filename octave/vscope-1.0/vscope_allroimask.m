function msk = vscope_allroimask(x, cam, marg)
% VSCOPE_ALLROIMASK - Return mask corresponding to all ROIs
%    msk = VSCOPE_ALLROIMASK(x, cam) returns a mask corresponding to
%    the joint area of all ROIs defined for camera CAM. X must be as 
%    from VSCOPE_LOAD. CAM may be a number or an ID.
%    msk = VSCOPE_ALLROIMASK(x, cam, marg) grows the mask by MARG
%    pixels in each direction. If MARG may be a scalar or a [Y X] pair.

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

[Y X C T] = size(x.ccd.dat);
msk = zeros(Y,X);
if ischar(cam)
  cam = find(strcmp(cam, x.ccd.info.camid));
end
xyxy = vscope_roicoords_cam(x, cam);
for k = 1:length(xyxy)
  msk(xyxy(k).y + Y*(xyxy(k).x-1)) = 1;
end

if nargin>2
  marg = ceil(marg)*2 + 1;
  if length(marg)==1
    marg = [marg marg];
  end
  msk = convn(msk, ones(marg(1), 1), 'same');
  msk = convn(msk, ones(1, marg(2)), 'same');  
end
msk = msk>0;
