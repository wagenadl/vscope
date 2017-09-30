function y = vscope_unpackbits(x)
% VSCOPE_UNPACKBITS - Unpack a bit array stored in uint32 to booleans
%   y = VSCOPE_UNPACKBITS(x) where X is an Nx1 or 1xN vector of uint32s
%   returns a Nx32 or 32xN matrix of booleans with the bits of X
%   unpacked.

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

N=length(x);
S=size(x);
x=x(:);

y=logical(zeros(N,32));
msk=1;
for k=1:32
  y(:,k) = bitand(x,uint32(msk))>0;
  msk=msk*2;
end
if S(1)==1
  y=y';
end
