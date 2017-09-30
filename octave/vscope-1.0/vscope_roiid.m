function str = vscope_roiid(n)
% VSCOPE_ROIID - Convert ROI number to string
%    str = VSCOPE_ROIID(n) returns the ID for ROI n (e.g., 'b' for n=2; 
%    'aa' for n=27). If n is an array, str will be a cell array.

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

if prod(size(n))>1
  str = cell(size(n));
  for k = 1:prod(size(n))
    str{k}  = vscope_roiid(n(k));
  end
else
  n = n-1;
  if n<26
    str = char('a' + n);
  else
    str = char([ 'a' + floor(n/26-1), 'a' + mod(n, 26) ]);
  end
end
