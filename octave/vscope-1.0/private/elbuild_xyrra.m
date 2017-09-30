function el = elbuild_xyrra(x,y,R,r,phi)
% ELBUILD_XYRRA  Build a struct for ELPLOT_XYRRA
%    el = ELBUILD_XYRRA(x,y,R,r,a) or el = ELBUILD_XYRRA([x,y,R,r,a])
%    builds a xyrra structure.
%    el = ELBUILD_XYRRA(el) where EL is already a xyrra structure just
%    returns el.

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

if isstruct(x)
  el = x;
elseif nargin==5
  el.x0=x;
  el.y0=y;
  el.R=R;
  el.r=r;
  el.phi=phi;
elseif nargin==1
  el.x0=x(1);
  el.y0=x(2);
  el.R=x(3);
  el.r=x(4);
  el.phi=x(5);
else
  error('ELBUILD_XYRRA needs one or five arguments');
end
