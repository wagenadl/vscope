function [vsd, dd, ee, ff] = pf_microcorrect1(vsd, msk)
% PF_MICROCORRECT - One step microcorrection using PARFARM
%    This is identical to MICROCORRECT, except that it splits the job 
%    across multiple CPUs using PARFARM.
%    Unfortunately, because of IPC copying of data, the speedup is modest.
%    This could be improved if I allowed PARFARM to read global variables.

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

[Y X T] = size(vsd);
T0 = ceil(T/2);
SX = 1; % * max(1, round(X/128));
SY = 0.25; % * max(1, round(Y/128));
SS = 0.25;

ref = vsd(:,:,T0);
if nargin>=2
  bri0 = mean(ref(msk));
else
  bri0 = 0;
  msk = [];
end
 
margx = 4*max(1, round(X/128));
margy = 4*max(1, round(Y/128));
refbri = mean(mean(ref(margy+1:end-margy, margx+1:end-margx), 1), 2);

ee0 = zeros(T,1);
for t=1:T
  img = vsd(:,:,t);
  imbri = mean(mean(img(margy+1:end-margy, margx+1:end-margx), 1), 2);
  fac(t) = refbri / imbri;
  ee0(t) = marginlessdiff(fac(t)*img, ref);
end

dd.t0 = T0;

global d0
d0 = struct();
d0.rfx1  = microshift(ref, -SX, 0);
d0.rfx2  = microshift(ref, SX, 0);
d0.rfy1  = microshift(ref, 0, -SY);
d0.rfy2  = microshift(ref, 0, SY);
d0.rfs1  = microscale(ref, -SS);
d0.rfs2  = microscale(ref, SS);
d0.rfpx1 = microperspective(ref, -SX, 0);
d0.rfpx2 = microperspective(ref, SX, 0);
d0.rfpy1 = microperspective(ref, 0, -SY);
d0.rfpy2 = microperspective(ref, 0, SY);
d0.ee0 = ee0;
d0.fac = fac;

global img
img = vsd;

d0.ref = ref;
d0.SY = SY;
d0.SX = SX;
d0.SS = SS;
d0.bri0 = bri0;
d0.msk = msk;
P = 2;
parfarm(P);
ttt = cell(2*P,1);
for p=1:2*P
ttt{p} = [p:2*P:T];
end

[vsd, dd, ee, ff] = parfarm(@pf_microcorrect1_one, ttt);

vsd = pf_join(vsd);

if nargout>=2
  dd = pf_joinstr(dd);
  dd.dx(T0) = 0;
  dd.dy(T0) = 0;
  dd.ds(T0) = 0;
  dd.dpx(T0) = 0;
  dd.dpy(T0) = 0;
else
  clear dd
end

if nargout>=3
  ee = pf_joinstr(ee);
  ee.e0 = ee0;
else
  clear ee
end

if nargout>=4
  ff = pf_joinstr(ff);
else
  clear ff
end
