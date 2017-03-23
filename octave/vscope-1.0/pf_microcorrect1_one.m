function [imo, ddd, eee, fff] = pf_microcorrect1_one(tt)
global d0
global img
for t = tt(:)'
er0 = d0.ee0(t);
vsd = img(:,:,t).*d0.fac(t);
dx = bestshift(vsd, d0.rfx1, d0.rfx2)*d0.SX/0.5;
out = microshift(vsd, dx, 0);
err = marginlessdiff(out, d0.ref);
ee.dx = err;
ff.dx = dx;
if err<er0
  vsd = out;
  er0 = err;
  dd.dx = dx;
else
  dd.dx = 0;
end

dy = bestshift(vsd, d0.rfy1, d0.rfy2)*d0.SY/0.5;
out = microshift(vsd, 0, dy);
err = marginlessdiff(out, d0.ref);
ee.dy = err;
ff.dy = dy;
if err<er0
  vsd = out;
  er0 = err;
  dd.dy = dy;  
else
  dd.dy = 0;
end

if d0.bri0>0
  bri = mean(vsd(d0.msk))/fac;
  vsd = img .* d0.bri0/bri;
  dd.dbri = 100*(d0.bri0/bri-1);
end

ds = bestshift(vsd, d0.rfs1, d0.rfs2)*d0.SS/0.5;
out = microscale(vsd, ds);
err = marginlessdiff(out, d0.ref);
ee.ds = err;
ff.ds = ds;
if err<er0
  vsd = out;
  er0 = err;
  dd.ds = ds;
else
  dd.ds = 0;
end

dx = bestshift(vsd, d0.rfpx1, d0.rfpx2)*d0.SX/0.5;
out = microperspective(vsd, dx, 0);
err = marginlessdiff(out, d0.ref);
ee.dpx = err;
ff.dpx = dx;
if err<er0
  vsd = out;
  er0 = err;
  dd.dpx = dx;
else
  dd.dpx = 0;
end

dy = bestshift(vsd, d0.rfpy1, d0.rfpy2)*d0.SY/0.5;
out = microperspective(vsd, 0, dy);
err = marginlessdiff(out, d0.ref);
ee.dpy = err;
ff.dpy = dy;
if err<er0
  vsd = out;
  er0 = err;
  dd.dpy = dy;
else
  dd.dpy = 0;
end

% Rotate?

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

imo{t} = vsd / d0.fac(t);
ddd{t} = dd;
eee{t} = ee;
fff{t} = ff;
end

