function [vsd, dd, ee, ff] = microcorrect(vsd, msk, opts, varargin)
% MICROCORRECT - One step microcorrection
%    out = MICROCORRECT(vsd) performs one iteration of microshifting,
%    scaling, and perspective correction.
%    out = MICROCORRECT(vsd, msk) also applies brightness correction
%    using the pixels in the mask only. 
%    [out, dd, ee] = MICROCORRECT(vsd) returns the changes applied and 
%    the errors at all moments.
%    MICROCORRECT(vsd, opts) or MICROCORRECT(vsd, msk, opts) specifies
%    which of the steps are performed:
%      x = x-shift
%      y = y-shift
%      s = scale
%      X = x-perspective
%      Y = y-perspective
%    Default is all steps.
%    MICROCORRECT(vsd, msk, opts, key, value, ...) specifies additional
%    parameters:
%      sx: max delta for X (1 pix)
%      sy: max delta for Y (.25 pix)
%      ss: max delta for scale (.25 pix/img)
%
%    See also MICROCORRECTROIS.
%
%    Example:
%       vsd = x.ccd.dat(:,:,find(strcmp('Bot', x.ccd.info.camid)),:);
%       msk = ~allroimask(x, 'Bot', 2);
%       [out, dd, ee] = microcorrect(vsd, msk, 'xys');

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

if nargin==1
  msk = [];
  opts = 'xysXY';
elseif nargin==2
  if ischar(msk)
    opts = msk;
    msk = [];
  else
    opts = 'xysXY';
  end
elseif nargin==3
  ;
elseif nargin==0
  error('microcorrect needs one to three arguments');
end    

kv = getopt('sx=1 sy=.25 ss=.25 t0=[]', varargin);

[Y X T] = size(vsd);
if isempty(kv.t0)
  T0 = ceil(T/2);
else
  T0 = kv.t0;
end
SX = kv.sx;%1; % * max(1, round(X/128));
SY = kv.sy;%0.25; % * max(1, round(Y/128));
SS = kv.ss;%0.25;

ref = double(vsd(:,:,T0));
if isempty(msk)
  bri0 = 0;
else
  bri0 = mean(ref(msk));
end
 
margx = 4*max(1, round(X/128));
margy = 4*max(1, round(Y/128));
refbri = mean(mean(ref(margy+1:end-margy, margx+1:end-margx), 1), 2);

ee.e0 = zeros(T,1);
for t=1:T
  img = double(vsd(:,:,t));
  imbri = mean(mean(img(margy+1:end-margy, margx+1:end-margx), 1), 2);
  fac(t) = refbri / imbri;
  ee.e0(t) = marginlessdiff(fac(t)*img, ref);
end

dd.t0 = T0;

rfx1 = microshift(ref, -SX, 0);
rfx2 = microshift(ref, SX, 0);
rfy1 = microshift(ref, 0, -SY);
rfy2 = microshift(ref, 0, SY);
rfs1 = microscale(ref, -SS);
rfs2 = microscale(ref, SS);
rfpx1 = microperspective(ref, -SX, 0);
rfpx2 = microperspective(ref, SX, 0);
rfpy1 = microperspective(ref, 0, -SY);
rfpy2 = microperspective(ref, 0, SY);

for t=1:T
  fprintf(2, '%i/%i\r', t,T);
  if t==T0
    continue;
  end
  img = fac(t) * double(vsd(:,:,t));
  er0 = ee.e0(t);
  
  if any(opts=='x')
    dx = bestshift(img, rfx1, rfx2)*SX/0.5;
    out = microshift(img, dx, 0);
    err = marginlessdiff(out, ref);
    ee.dx(t) = err;
    ff.dx(t) = dx;
    if err<er0
      img = out;
      er0 = err;
      dd.dx(t) = dx;
    else
      dd.dx(t) = 0;
    end
  else
    dd.dx(t) = nan;
    ee.dx(t) = nan;
  end

  if any(opts=='y')
    dy = bestshift(img, rfy1, rfy2)*SY/0.5;
    out = microshift(img, 0, dy);
    err = marginlessdiff(out, ref);
    ee.dy(t) = err;
    ff.dy(t) = dy;
    if err<er0
      img = out;
      er0 = err;
      dd.dy(t) = dy;  
    else
      dd.dy(t) = 0;
    end
  else
    dd.dy(t) = nan;
    ee.dy(t) = nan;
  end

  if bri0>0
    bri = mean(img(msk))/fac(t);
    img = img .* bri0/bri;
    dd.dbri(t) = 100*(bri0/bri-1);
  else
    dd.dbri(t) = nan;
  end
  
  if any(opts=='s')
    ds = bestshift(img, rfs1, rfs2)*SS/0.5;
    out = microscale(img, ds);
    err = marginlessdiff(out, ref);
    ee.ds(t) = err;
    ff.ds(t) = ds;
    if err<er0
      img = out;
      er0 = err;
      dd.ds(t) = ds;
    else
      dd.ds(t) = 0;
    end
  else
    dd.ds(t) = nan;
    ee.ds(t) = nan;
  end

  if any(opts=='X')
    dx = bestshift(img, rfpx1, rfpx2)*SX/0.5;
    out = microperspective(img, dx, 0);
    err = marginlessdiff(out, ref);
    ee.dpx(t) = err;
    ff.dpx(t) = dx;
    if err<er0
      img = out;
      er0 = err;
      dd.dpx(t) = dx;
    else
      dd.dpx(t) = 0;
    end
  else
    dd.dpx(t) = nan;
    ee.dpx(t) = nan;
  end
  
  if any(opts=='Y')
    dy = bestshift(img, rfpy1, rfpy2)*SY/0.5;
    out = microperspective(img, 0, dy);
    err = marginlessdiff(out, ref);
    ee.dpy(t) = err;
    ff.dpy(t) = dy;
    if err<er0
      img = out;
      er0 = err;
      dd.dpy(t) = dy;
    else
      dd.dpy(t) = 0;
    end
  else
    dd.dpy(t) = nan;
    ee.dpy(t) = nan;
  end

  % Rotate?

  vsd(:,:,t) = img / fac(t);
end
fprintf(2,'\n');

