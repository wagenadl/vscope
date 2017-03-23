function x = algcsqr(y)

% all elements of y must be in the interval (0,1)

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

z=y.^2;
dims=size(z);
lower_bound=repmat(1e-15,dims);
upper_bound=repmat(1-1e-15,dims);
z=max(z,lower_bound);
z=min(z,upper_bound);
x = -log(z.^(-1)-1);
