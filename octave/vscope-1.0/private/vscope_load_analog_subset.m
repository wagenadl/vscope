function dat = vscope_load_analog_subset(ifn, info, offset, count)
% VSCOPE_LOAD_ANALOG_SUBSET - Load a subset of analog data from VScope
%   dat = VSCOPE_LOAD_ANALOG_SUBSET(ifn, info, offset, len) read COUNT scans
%   of analog data after skipping the first OFFSET scans.
%   INFO must be the ANALOG.INFO field from a prior VSCOPE_LOAD with
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

ifn = strrep(ifn,'.xml','-analog.dat'); % for ease of use.

ifd = fopen(ifn,'rb');
C = length(info.channo);
fseek(ifd, offset*C*2, 'bof');
dat = fread(ifd,[1 count*C],'int16');
L = length(dat);
dat = reshape(dat,[C L/C]);
for k=1:length(info.chunk)
  i0 = info.chunk(k).istart - offset;
  i1 = min(info.chunk(k).iend,L/C) - offset;
  if i0<1
    i0 = 1;
  end
  if i1 >= i0
    for c=1:C
      dat(c,i0:i1) = dat(c,i0:i1) * info.chunk(k).scale(c) ...
          + info.chunk(k).offset(c);
    end
  end
end
dat = dat';
fclose(ifd);
