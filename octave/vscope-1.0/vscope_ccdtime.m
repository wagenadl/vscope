function [tt, te] = vscope_ccdtime(x, id)
% VSCOPE_CCDTIME - Construct a vector of time points for CCD data
%    tt = VSCOPE_CCDTIME(x, id), where X is the result of VSCOPE_LOAD,
%    and ID is the name of a camera contained therein, returns
%    a vector of start times (in seconds) corresponding to the frames in 
%    the ccd data for that camera.
%    If ID is not given, the first camera is used.
%    For convenience, X may also be the ccd substructure itself.
%    [tt, te] = VSCOPE_CCDTIME(...) returns the end times as well.
%    If, for some reason, there are no frame markers for the given camera
%    in the digital data, VSCOPE_CCDTIME first tries to find another camera
%    with the same configuration to supplement the missing info. If that 
%    cannot be found, the numbers from the "settings" are trusted.
%    In either case, a warning is issued.

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
      id = x.info.camid{idx};
      break;
    end
  end
  printf('Reporting on camera “%s”\n', id);
elseif ischar(id)
  idx = strcmp(id, x.info.camid);
  if isempty(idx)
    error(sprintf('No camera named “%s”', id));
  end
else
  idx = id;
  id = x.info.camid{idx};
end

if isempty(x.info.framestart_s{idx})
  got = 0;
  for idx1=1:length(x.info.camid)
    if idx1~=idx
      if abs(log(x.info.rate_hz{idx}/x.info.rate_hz{idx1})) < .1 ...
            && x.info.delay_ms{idx}==x.info.delay_ms{idx1} ...
            && x.info.nframes{idx}==x.info.nframes{idx1} ...
            && ~isempty(x.info.framestart_s{idx1})
        got = idx1;
        break
      end
    end
  end
  if got
    tt = x.info.framestart_s{got};
    te = x.info.frameend_s{got};
    printf('Caution: Estimating frame times for “%s” from “%s”\n', ...
           id, x.info.camid{got});
  else
    t0 = x.info.delay_ms{idx}/1e3;
    f = x.info.rate_hz{idx};
    n = x.info.nframes{idx};
    tt = [0:n-1]'/f + t0;
    te = [1:n]'/f + t0;
    printf('Warning: Estimating frame times for “%s” from nominal\n', id);
  end
else
  tt = x.info.framestart_s{idx};
  te = x.info.frameend_s{idx};
end

if nargout<2
  clear te
end
