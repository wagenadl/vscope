function [xx, yy, cam] = vscope_roioutline_cam(rois, info, camid)
% VSCOPE_ROIOUTLINE_CAM - Returns a polygon specifying the edge of a ROI
%    [xx, yy] = VSCOPE_ROIOUTLINE_CAM(rois, info, camid) is like 
%    VSCOPE_ROIOUTLINE, except that coordinates are transformed to
%    those of the given camera.
%    CAMID may be a number or the name of a camera.
%    [xx, yy, cam] = VSCOPE_ROIOUTLINE_CAM(x), where X is straight
%    from VSCOPE_LOAD is also supported. In that case, each ROI is 
%    transformed to the coordinates of the first of its cameras, and CAM
%    will be a vector of camera numbers (1 to N).

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

if isstruct(rois)
  camid = [];
  info = rois.ccd.info;
  camids = rois.roicams;
  rois = rois.rois;
end

if ischar(camid)
  camid = find(strcmp(info.camid, camid));
  if isempty(camid)
    error('No such camera');
  end
end

[xx, yy] = vscope_roioutline(rois);
R = length(xx);
cam = zeros(R, 1);
for r=1:R
  if isempty(camid)
    id = find(strcmp(info.camid, camids{r}{1}));
  else
    id = camid;
  end
  xx{r} = vrc_inv_x(xx{r}, info.xform{id});
  yy{r} = vrc_inv_y(yy{r}, info.xform{id});
  cam(r) = id;
end

if nargout<3
  clear cam
end
if nargout==1
  xx = [xx yy];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Following copied from vscope_roicoords_cam.
function xx = vrc_inv_x(xx, xform)
xx = (xx-xform.bx) ./ xform.ax;

function yy = vrc_inv_y(yy, xform)
yy = (yy-xform.by) ./ xform.ay;
