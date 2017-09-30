function [tt, te] = vscope_ccdtime(x, id)
% VSCOPE_CCDTIME - Construct a vector of time points for CCD data
%    tt = VSCOPE_CCDTIME(x, id), where X is the result of VSCOPE_LOAD,
%    and ID is the name of a camera contained therein, returns
%    a vector of start times (in seconds) corresponding to the frames in 
%    the ccd data for that camera.
%    If ID is not given, the first camera is used.
%    For convenience, X may also be the ccd substructure itself.
%    [tt, te] = VSCOPE_CCDTIME(...) returns the end times as well

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

if isfield(x, 'ccd')
  x = x.ccd;
end

if nargin<2 || isempty(id)
  N = length(x.info.framestart_s);
  idx = 1;
  for n=1:N
    if x.info.nframes{n}>0
      idx = n;
      break;
    end
  end
elseif ischar(id)
  idx = strcmp(id, x.info.camid);
  if isempty(idx)
    error(['No camera named ' id]);
  end
else
  idx = id;
end

if isempty(x.info.framestart_s{idx})
  t0 = x.info.delay_ms{idx}/1e3;
  f = x.info.rate_hz{idx};
  n = x.info.nframes{idx};
  tt = [0:n-1]'/f + t0;
  te = [1:n]'/f + t0;
else
  tt = x.info.framestart_s{idx};
  te = x.info.frameend_s{idx};
end
