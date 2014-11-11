function [vsd, dd, ee] = microcorrect(vsd, msk)
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
SX = 0.5*max(1, round(X/128));
SY = 0.5*max(1, round(Y/128));

ref = vsd(:,:,T0);
ref1 = ref(2:end-1, 2:end-1);
if nargin>=2
  bri0 = mean(ref(msk));
else
  bri0 = 0;
end

ee.e0 = zeros(T,1);
for t=1:T
  ee.e0(t) = sqrt(mean(mean((vsd(:,:,t)-vsd(:,:,T0)).^2, 1), 2));
end

dd.t0 = T0;

rfx1 = microshift(ref, -SX, 0);
rfx2 = microshift(ref, SX, 0);
rfy1 = microshift(ref, 0, -SY);
rfy2 = microshift(ref, 0, SY);
rfs1 = microscale(ref, -0.5);
rfs2 = microscale(ref, 0.5);
rfpx1 = microperspective(ref, -SX, 0);
rfpx2 = microperspective(ref, SX, 0);
rfpy1 = microperspective(ref, 0, -SY);
rfpy2 = microperspective(ref, 0, SY);

for t=1:T
  fprintf(1, '%i/%i\r', t,T);
  fflush(1);
  if t==T0
    continue;
  end
  img = vsd(:,:,t);
  er0 = ee.e0(t);
  
  dx = bestshift(img, rfx1, rfx2)*SX/0.5;
  dy = bestshift(img, rfy1, rfy2)*SY/0.5;
  
  out = microshift(img, dx, dy);
  err = sqrt(mean((out(:)-ref1(:)).^2));
  ee.dshift(t) = err;
  if err<er0
    img(2:end-1, 2:end-1) = out;
    dd.dx(t) = dx;
    dd.dy(t) = dy;
    er0 = err;
  else
    dd.dx(t) = 0;
    dd.dy(t) = 0;
  end

  if bri0>0
    bri = mean(img(msk));
    img = img .* bri0/bri;
    dd.dbri(t) = 100*(bri0/bri-1);
  end
  
  ds = bestshift(img, rfs1, rfs2);
  out = microscale(img, ds);
  err = sqrt(mean((out(:)-ref1(:)).^2));
  ee.dscale(t) = err;
  if err<er0
    img(2:end-1, 2:end-1) = out;
    dd.ds(t) = ds;
    er0 = err;
  else
    dd.ds(t) = 0;
  end

  dx = bestshift(img, rfpx1, rfpx2)*SX/0.5;
  dy = bestshift(img, rfpy1, rfpy2)*SX/0.5;
  out = microperspective(img, dx, dy);
  err = sqrt(mean((out(:)-ref1(:)).^2));
  ee.dpersp(t) = err;
  if err<er0
    img(2:end-1, 2:end-1) = out;
    dd.dpx(t) = dx;
    dd.dpy(t) = dy;
    er0 = err;
  else
    dd.dpx(t) = 0;
    dd.dpy(t) = 0;
  end

  % Rotate?

  vsd(:,:,t) = img;
end
fprintf(1,'\n');
fflush(1);
