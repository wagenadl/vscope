function n = vscope_roinum(str)
% VSCOPE_ROINUM - Convert ROI string to number
%    n = VSCOPE_ROIID(str) returns the number for named ROI str (e.g., 2
%    for 'b', 27 for 'aa'). If n is a cell array, str will be a numeric array.

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

if iscell(str)
  n  = zeros(size(str));
  for k = 1:prod(size(str))
    n(k) = vscope_roinum(str{k});
  end
else
  if length(str)==1
    n = 1 + str - 'a';
  else
    n = 27 + 26*(str(1) - 'a') + str(2) - 'a';
  end
end
