function p = vscope_roicoords_cam(rois, info, camno)
% VSCOPE_ROICOORDS_CAM - Returns a list of pixels inside ROIs for a camera
%   p = VSCOPE_ROICOORDS_CAM(roi, info, camno) returns coordinates
%   for a single ROI or cell array of ROIs transformed into the coordinate
%   system of the camera identified by CAMNO. INFO must be a CCD.INFO field
%   from VSCOPE_LOAD. ROIs may be specified xyrra-style (aka elliptic), or
%   polygon-style (aka blob).
%   p = VSCOPE_ROICOORDS_CAM(x, camno), where X is straight from VSCOPE_LOAD
%   is also supported.
%   P will be a struct array (one struct per ROI) with fields X and Y that
%   are vectors containing the coordinates.
%   In all cases, coordinates will be restricted to what's actually inside
%   the camera frame.

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
  camno = info;
  info = rois.ccd.info;
  rois = rois.rois;
end

if ~iscell(rois)
  rois = {rois};
end

K = length(rois);

for k=1:K
  S = size(rois{k});
  if prod(S)==0
    % Empty ROI
    p(k).x = zeros(0,1);
    p(k).y = zeros(0,1);
  elseif prod(S)==5
    % This is xyrra style
    el = elbuild_xyrra(rois{k});
    
    % Next bit adapted from elfill.m:
    r = max(el.R,el.r);
    x0 = floor(vrc_inv_x(el.x0-r, info.xform{camno}));
    y0 = floor(vrc_inv_y(el.y0-r, info.xform{camno}));
    x1 = ceil(vrc_inv_x(el.x0+r, info.xform{camno}));
    y1 = ceil(vrc_inv_y(el.y0+r, info.xform{camno}));
    x0 = max(x0, 0);
    y0 = max(y0, 0);
    x1 = min(x1, info.pix{camno}(1)-1);
    y1 = min(y1, info.pix{camno}(2)-1);

    X = 1+x1-x0;
    Y = 1+y1-y0;
    
    xx = repmat([x0:x1], [Y 1]);
    yy = repmat([y0:y1]', [1 X]);
    xx_ = vrc_xform_x(xx, info.xform{camno}) - el.x0;
    yy_ = vrc_xform_y(yy, info.xform{camno}) - el.y0;
    
    xi = cos(el.phi)*xx_ + sin(el.phi)*yy_;
    eta = -sin(el.phi)*xx_ + cos(el.phi)*yy_;
 
    idx = find(xi.^2/el.R^2 + eta.^2/el.r^2 <= 1);
    p(k).x = xx(idx) + 1;
    p(k).y = yy(idx) + 1;
  else
    % This is polygon style
    xx = vrc_inv_x(rois{k}(:,1), info.xform{camno});
    yy = vrc_inv_y(rois{k}(:,2), info.xform{camno});
    xmin = floor(min(xx))-1;
    ymin = floor(min(yy))-1;
    xmax = ceil(max(xx))+1;
    ymax = ceil(max(yy))+1;
    xmin = max(xmin, 0);
    ymin = max(ymin, 0);
    xmax = min(xmax, info.pix{camno}(1));
    ymax = min(ymax, info.pix{camno}(2));
    xx0 = xx-xmin;
    yy0 = yy-ymin;
    X = xmax-xmin;
    Y = ymax-ymin;
    img = fillpoly(Y,X,[xx0 yy0]);
    idx = find(img(:)>0);
    p(k).y = mod(idx-1,Y) + ymin + 1;
    p(k).x = div(idx-1,Y) + xmin + 1;
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function xx = vrc_inv_x(xx, xform)
xx = (xx-xform.bx) ./ xform.ax;

function yy = vrc_inv_y(yy, xform)
yy = (yy-xform.by) ./ xform.ay;

function xx = vrc_xform_x(xx, xform)
xx = xform.ax .* xx + xform.bx;

function yy = vrc_xform_y(yy, xform)
yy = xform.ay .* yy + xform.by;
