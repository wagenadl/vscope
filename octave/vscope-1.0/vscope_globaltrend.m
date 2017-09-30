function y = vscope_globaltrend(x, noroi)
% VSCOPE_GLOBALTREND - Extract global trend from VSD data
%    y = VSCOPE_GLOBALTREND(x) returns the global trend of the VSD
%    image sequence in the vscope data X (which must be as loaded by
%    VSCOPE_LOAD).
%    y = VSCOPE_GLOBALTREND(x, 1) returns the global trend based on
%    those areas of the VSD image sequence that are not in any ROIs.

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

if nargin<2
  noroi = 0;
end

[Y X C T] = size(x.ccd.dat);
KY = floor(Y/32);
KX = floor(X/32);
y = zeros(T,C);
for c=1:C
  if noroi
    msk = ~vscope_allroimask(x, c);
  else
    msk = logical(ones(Y,X));
  end
  msk(1:KY,:) = 0;
  msk(Y-KY+1:Y,:) = 0;
  msk(:,1:KX) = 0;
  msk(:,X-KX+1:X) = 0;
  for t=1:T
    img = double(x.ccd.dat(:,:,c,t));
    y(t,c) = mean(img(msk));
  end
end

    