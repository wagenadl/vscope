function zz = vscope_ephysatccd(x, yy, varargin)
% VSCOPE_EPHYSATCCD - Interpolate ephys data at camera frame times
%    zz = VSCOPE_EPHYSATCCD(x, yy) takes the values of the signal YY,
%    which must be measured at the ephys times of the vscope trial X 
%    calculates each average over each of the frames of the first camera.
%    zz = VSCOPE_EPHYSATCCD(x, yy, key, value, ...) specifies additional
%    parameters:
%       camera - camera number
%       func - alternative function to apply to the data within a frame
%              (the default being 'mean')
%    YY may be a Tx1 vector or a TxN matrix, in which case the columns
%    are processed independently.

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

kv = getopt('camera=[] func=''mean''', varargin);

[t_on, t_off] = vscope_ccdtime(x, kv.camera);
t_e = vscope_ephystime(x);

S = size(yy);
T = length(t_on);
N = prod(S(2:end));
yy = reshape(yy, [S(1) N]);
zz = zeros(T, N);
for t = 1:T
  idx = find(t_e>=t_on(t) & t_e<=t_off(t));
  if isempty(idx)
    z = zeros(1,N) + nan;
  else
    y = yy(idx, :);
    y = reshape(y, [length(idx) S(2:end)]);
    z = feval(kv.func, y);
  end
  zz(t,:) = z(:);
end

zz = reshape(zz, [T S(2:end)]);
