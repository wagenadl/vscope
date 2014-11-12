function [vsd, dd, ee, ff] = microcorrect(vsd, msk)
% MICROCORRECT - One step microcorrection
%    out = MICROCORRECT(vsd) performs one iteration of microshifting,
%    scaling, and perspective correction.
%    out = MICROCORRECT(vsd, msk) also applies brightness correction
%    using the pixels in the mask only. 
%    [out, dd, ee] = MICROCORRECT(vsd) returns the changes applied and 
%    the errors at all moments.
%
%    Example:
%       vsd = x.ccd.dat(:,:,find(strcmp('Bot', x.ccd.info.camid)),:);
%       msk = ~allroimask(x, 'Bot', 2);
%       [out, dd, ee] = microcorrect(vsd, msk);

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
end

ee.e0 = zeros(T,1);
for t=1:T
  ee.e0(t) = marginlessdiff(vsd(:,:,t), ref);
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
  img = vsd(:,:,t);
  er0 = ee.e0(t);
  
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

  if bri0>0
    bri = mean(img(msk));
    img = img .* bri0/bri;
    dd.dbri(t) = 100*(bri0/bri-1);
  end
  
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

  % Rotate?

  vsd(:,:,t) = img;
end
fprintf(2,'\n');

