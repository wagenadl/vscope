function ff = vscope_showrois(x, ff)
% VSCOPE_SHOWROIS - Show ROIs overlaid on camera images
%    VSCOPE_SHOWROIS(x) where X comes directly from VSCOPE_LOAD creates
%    a figure for each camera and displays the first frame of CCD data
%    with ROIs overlaid.
%    VSCOPE_SHOWROIS(x, ff) reuses figures FF.
%    ff = VSCOPE_SHOWROIS(...) returns a vector of figure handles.

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

ids = x.ccd.info.camid;
if nargin<2
  ff = zeros(size(ids));
end
for c=1:length(ids);
  cid = ids{c};
  xform = x.ccd.info.xform{c};
  if ff(c)==0
    ff(c) = figure;
  else
    figure(ff(c)); clf;
  end
  imagesc(x.ccd.dat(:,:,c,1));
  colormap(gray);
  hold on
  cc = jet(length(x.rois));
  for r=1:length(x.rois)
    if ~isempty(x.roicams{r}) && ~isempty(strmatch(cid, x.roicams{r}, 'exact'))
      xy = vscope_roioutline(x.rois{r});
      xx = (xy(:,1) - xform.bx) / xform.ax;
      yy = (xy(:,2) - xform.by) / xform.ay;
      x0 = mean(xx);
      y0 = mean(yy);
      xx(end+1) = xx(1);
      yy(end+1) = yy(1);
      plot(xx, yy, 'color', cc(r,:));
      text(x0, y0, vscope_roiid(r), 'horizontalalignment', 'center',...
	  'verticalalignment', 'middle', 'color', cc(r,:));
    end
  end
  title(cid)
end

if nargout<1
  clear ff;
end
