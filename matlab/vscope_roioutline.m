function [xx,yy] = vscope_roioutline(roi)
% VSCOPE_ROIOUTLINE - Returns a polygon specifying the edge of a ROI
%    [xx,yy] = VSCOPE_ROIOUTLINE(roi) returns coordinates of the edge
%    of a ROI, which can either be specified xyrra-style (aka
%    elliptic), or polygon-style (aka blob).
%    xy = VSCOPE_ROIOUTLINE(roi) returns an Nx2 matrix.
%    xy = VSCOPE_ROIOUTLINE(rois), where ROIS is a cell array of K ROI
%    specifications, returns an array of K cells containing the
%    coordinates of each of those ROIs.
%    Caution: xyrra-defined ROIs can result in off-image coordinates.
if iscell(roi) 
  K = length(roi);
  if nargout==2
    yy=cell(K,1);
  end
  xx = cell(K,1);
  for k=1:K
    if nargin==3
      [x,y] = vscope_roioutline(roi{k},X,Y);
    else
      [x,y] = vscope_roioutline(roi{k});
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
  if prod(S)==5
    % This is xyrra style
    el = elbuild_xyrra(roi);
    [xx,yy] = elplot_xyrra(roi);
  else
    % This is polygon style
    xx = roi(:,1);
    yy = roi(:,2);
  end
  if nargout<2
    xx=[xx yy];
  end
end
