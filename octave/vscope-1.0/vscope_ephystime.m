function tt = vscope_ephystime(x)
% VSCOPE_EPHYSTIME - Construct a vector of time points for ephys data
%    tt = VSCOPE_EPHYSTIME(x) where X is the result of VSCOPE_LOAD returns
%    a vector of times (in seconds) corresponding to the samples in the
%    analog data contained in X.analog.dat
%    For convenience, X may also be the analog substructure itself.

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

if isfield(x,'analog')
  tt = [0:size(x.analog.dat,1)-1]'/x.analog.info.rate_hz;
else
  tt = [0:size(x.dat,1)-1]'/x.info.rate_hz;
end
