function img = fillpoly(Y,X,xyxy)
% FILLPOLY  Fills a polygon
%   img = FILLPOLY(Y,X,xyxy) creates an image of size YxX, with pixels
%   inside the polygon defined by XYXY set to logical one, others to zero.
%   XYXY must be Nx2, with XYXY(:,1) representing x-coordinates, and
%   XYXY(:,2) representing y-coordinates.
%   The output is suitable for IMAGESC, or for EXTRACTPOLYROI.

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

xx=repmat([1:X],[Y 1]); 
yy=repmat([1:Y]',[1 X]);
img = inpolygon(xx,yy,xyxy(:,1),xyxy(:,2));
return

img = logical(ones(Y,X));
xx=repmat([1:X],[Y 1]);
yy=repmat([1:Y]',[1 X]);
[N,D]=size(xyxy);
xyxy1=xyxy([[2:N] 1],:);
for n=1:N
  dx=xyxy(n,1)-xx;
  dy=xyxy(n,2)-yy;
  dx_=xyxy1(n,1)-xx;
  dy_=xyxy1(n,2)-yy;
  sg = sign(dx.*dy_ - dy.*dx_);
  if n==1
    sgn=sg;
  else
    img(sg~=sgn) = 0;
  end
end
