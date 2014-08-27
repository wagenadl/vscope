function [xx,yy] = vscope_roicoords(roi,X,Y)
% VSCOPE_ROICOORDS - Returns a list of all pixels inside an ROI
%    DEPRECATED in favor of VSCOPE_ROICOORDS_CAM.

%    [xx,yy] = VSCOPE_ROICOORDS(roi) returns coordinates of all pixels
%    inside an ROI, which can either be specified xyrra-style (aka
%    elliptic), or polygon-style (aka blob).
%    xy = VSCOPE_ROICOORDS(roi) returns an Nx2 matrix.
%    xy = VSCOPE_ROICOORDS(rois), where ROIS is a cell array of K ROI
%    specifications, returns an array of K cells containing the
%    coordinates of each of those ROIs.
%    Caution: xyrra-defined ROIs can result in off-image coordinates.
%    Calling as VSCOPE_ROICOORDS(roi,X,Y) to specify image size avoids this
%    problem.
%    Note: here X refers to the first dimension of the data, Y to the second;
%    at least for polygon-style ROIs. I clearly need to check this.
if iscell(roi) 
  K = length(roi);
  if nargout==2
    yy=cell(K,1);
  end
  xx = cell(K,1);
  for k=1:K
    if nargin==3
      [x,y] = vscope_roicoords(roi{k},X,Y);
    else
      [x,y] = vscope_roicoords(roi{k});
    end
    if nargout==2
      xx{k} = x;
      yy{k} = y;
    else
      xx{k} = [x y];
    end
  end
else
  S = size(roi);
  if prod(S)==0
      % Empty ROI
      xx = zeros(0,1);
      yy = zeros(0,1);
  elseif prod(S)==5
    % This is xyrra style
    el = elbuild_xyrra(roi);
    
    % Following copied from elfill.m:
    r = max(el.R,el.r);
    x0 = floor(el.x0-r);
    x1 = ceil(el.x0+r);
    y0 = floor(el.y0-r);
    y1 = ceil(el.y0+r);
    
    X_ = 1+x1-x0;
    Y_ = 1+y1-y0;
    
    xx = repmat([x0:x1],[Y_ 1]) - el.x0;
    yy = repmat([y0:y1]',[1 X_]) - el.y0;
    
    xi = cos(el.phi)*xx + sin(el.phi)*yy;
    eta = -sin(el.phi)*xx + cos(el.phi)*yy;
    % End of copied code, but next line almost copied.
    idx = find(xi.^2/el.R^2 + eta.^2/el.r^2 <= 1);
    
    xx = xx(idx) + el.x0;
    yy = yy(idx) + el.y0;
  else
    % This is polygon style
    xmin = floor(min(roi(:,1)))-1;
    xmax = ceil(max(roi(:,1)))+1;
    ymin = floor(min(roi(:,2)))-1;
    ymax = ceil(max(roi(:,2)))+1;
    xx0 = roi(:,1)-xmin;
    yy0 = roi(:,2)-ymin;
    X_ = xmax-xmin;
    Y_ = ymax-ymin;
    img = fillpoly(Y_,X_,[xx0 yy0]);
    idx = find(img(:)>0);
    yy = mod(idx-1,Y_) + ymin + 1;
    xx = div(idx-1,Y_) + xmin + 1;
  end
  if nargin>=3
    ok = find(xx>=1 & xx<=X & yy>=1 & yy<=Y);
    xx = xx(ok,1);
    yy = yy(ok,1);
  end
  if nargout<2
    xx=[xx yy];
  end
end
