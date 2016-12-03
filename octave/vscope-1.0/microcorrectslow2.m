function [vsd, dd, ee, ff] = microcorrectslow(vsd, msk)
% MICROCORRECTSLOW - One step microcorrection - optimization version
%    out = MICROCORRECTSLOW(vsd) performs one iteration of microshifting,
%    scaling, and perspective correction.
%    out = MICROCORRECTSLOW(vsd, msk) also applies brightness correction
%    using the pixels in the mask only. 
%    [out, dd, ee] = MICROCORRECT(vsd) returns the changes applied and 
%    the errors at all moments.
%
%    Example:
%       vsd = x.ccd.dat(:,:,find(strcmp('Bot', x.ccd.info.camid)),:);
%       msk = ~allroimask(x, 'Bot', 2);
%       [out, dd, ee] = microcorrectslow(vsd, msk);

[Y X T] = size(vsd);
T0 = ceil(T/2);
SX = 0.5;
SY = 0.125;
SS = 0.125;

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

for t=1:T
  fprintf(2, '%i/%i\r', t,T);
  if t==T0
    continue;
  end
  img = vsd(:,:,t);

  % [dx, err] = fminsearch(@(dx) (marginlessdiff(microshift(img, dx, 0), ref)), 0);
  [dx, err, info] = fminbnd(@(dx) (marginlessdiff(microshift(img, dx, 0), ref)), -SX, SX);
  ee.dx(t) = err;
  ff.dx(t) = dx;
  if info>0
    img = microshift(img, dx, 0);
    dd.dx(t) = dx;
  else
    dd.dx(t) = 0;
  end

  % [dy, err] = fminsearch(@(dy) (marginlessdiff(microshift(img, 0, dy), ref)), 0);
  [dy, err, info] = fminbnd(@(dy) (marginlessdiff(microshift(img, 0, dy), ref)), -SY, SY);  
  ee.dy(t) = err;
  ff.dy(t) = dy;
  if info>0
    img = microshift(img, 0, dy);
    dd.dy(t) = dy;  
  else
    dd.dy(t) = 0;
  end

  if bri0>0
    bri = mean(img(msk));
    img = img .* bri0/bri;
    dd.dbri(t) = 100*(bri0/bri-1);
  end
  
  % [ds, err] = fminsearch(@(ds) (marginlessdiff(microscale(img, ds), ref)), 0);
  [ds, err, info] = fminbnd(@(ds) (marginlessdiff(microscale(img, ds), ref)), -SS, SS);
  ee.ds(t) = err;
  ff.ds(t) = ds;
  if info>0
    img = microscale(img, ds);
    dd.ds(t) = ds;
  else
    dd.ds(t) = 0;
  end

  % [dx, err] = fminsearch(@(dx) (marginlessdiff(microperspective(img, dx, 0), ref)), 0);
  [dx, err, info] = fminbnd(@(dx) (marginlessdiff(microperspective(img, dx, 0), ref)), -SX, SX);
  ee.dpx(t) = err;
  ff.dpx(t) = dx;
  if info>0
    img = microperspective(img, dx, 0);
    dd.dpx(t) = dx;
  else
    dd.dpx(t) = 0;
  end
  
  [dy, err, info] = fminbnd(@(dy) (marginlessdiff(microperspective(img, 0, dy), ref)), -SY, SY);
  ee.dpy(t) = err;
  ff.dpy(t) = dy;
  if info>0
    img = microperspective(img, 0, dy);
    dd.dpy(t) = dy;
  else
    dd.dpy(t) = 0;
  end

  % Rotate?

  vsd(:,:,t) = img;
end
fprintf(2,'\n');

