function imgs = vscope_imagestack(root, tri)
% VSCOPE_IMAGESTACK - Image stacking for VSCOPE
%    imgs = VSCOPE_IMAGESTACK(root, tri) loads the trials TRI (a vector of
%    integer trial numbers) from the directory ROOT and returns an image stack
%    for each of the cameras.
%    The result is a cell array of XxYxN arrays, one cell for each camera.
%    Any trials that are /not/ a single snapshot are ignored.
%    Thus, each cell can be fed to FOCUSSTACK without further processing.

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

imgs = {};
for f=1:length(tri)
  fn = sprintf('%s/%03i.xml', root, tri(f));
  fprintf(1, 'vscope_imagestack: loading %s\n', fn);
  x = vscope_load(fn);
  if isfield(x, 'ccd') && isfield(x.ccd, 'dat')
    XYCT = size(x.ccd.dat);
    if prod(XYCT)>0 && length(XYCT)==3
      % snapshot!
      if isempty(imgs)
        imgs = cell(XYCT(3), 1);
      end
    end
    for c=1:XYCT(3)
      if isempty(imgs{c})
        imgs{c} = zeros([XYCT(1:2) 0]);
      end
      XYF = size(imgs{c});
      if XYF(1)~=XYCT(1) || XYF(2)~=XYCT(2)
        error('Image size mismatch');
      end
      imgs{c}(:,:,end+1) = x.ccd.dat(:,:,c);
    end
  end
end
