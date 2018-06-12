function dat = vscope_load_digital_subset(ifn, info, offset, count)
% VSCOPE_LOAD_DIGITAL_SUBSET - Load a subset of digital data from VScope
%   dat = VSCOPE_LOAD_DIGITAL_SUBSET(ifn, info, offset, len) read COUNT scans
%   of digital data after skipping the first OFFSET scans.
%   INFO must be the DIGITAL.INFO field from a prior VSCOPE_LOAD with
%   'i' in the LETTERS.

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

if nargin ~= 4
  error('Bad argument count');
end

ifn = strrep(ifn,'.xml','-digital.dat'); % for ease of use.
W = info.typebytes;
typ = sprintf('uint%i', W*8);
ifd = fopen(ifn,'rb');
fseek(ifd, offset*W, 'bof');
dat = fread(ifd,[count 1],['*' typ]);
fclose(ifd);
