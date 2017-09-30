function [N,nw,K] = ntapers_mtm0(t,f_res)
% NTAPERS_MTM0 - Calculate information about tapers that PDS_MTM0 would use
%   [N,nw,K] = NTAPERS_MTM0(tt,f_res) returns the number of time samples N,
%   as well as the bandwidth number NW and the number of tapers K.
%   (Actually, the half-bandwidth is W=NW/N.)

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

N=length(t);  % n of time samples
dt=(t(N)-t(1))/(N-1);
fs=1/dt;

% compute nw and K
nw=N*dt*f_res;
K=floor(2*nw-1);
