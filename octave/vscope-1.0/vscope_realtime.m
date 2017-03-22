function [tstart, tend] = vscope_realtime(x)
% VSCOPE_REALTIME - Time of start and end of trial of vscope data
%    [tstart, tend] = VSCOPE_REALTIME(x), where X is the result of
%    VSCOPE_LOAD, returns the start and end times of the trial X
%    as Matlab time vectors (see CLOCK).

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

tstart = vsc_rt(x.info.starts);
if nargout>=2
  tend = vsc_rt(x.info.ends);
end	

function tv = vsc_rt(s)
tv = [ atoi(s(1:2))+2000 atoi(s(3:4)) atoi(s(5:6)) ...
	atoi(s(8:9)) atoi(s(10:11)) atoi(s(12:13)) ];

