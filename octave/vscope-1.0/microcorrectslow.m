function [vsd, dd, ee] = microcorrectslow(vsd, msk)
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
K = 3; % Three attempts at each level
T0 = ceil(T/2);

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

  er0 = inf;
  dx0 = 0;
  for k=1:K
    [dx, err] = fminsearch(@(dx) (marginlessdiff(microshift(img, dx, 0), ref)), 0);
    if err<er0
      dx0 = dx;
      er0 = err;
    end
  end
  ee.dx(t) = er0;
  dd.dx(t) = dx0;
  img = microshift(img, dx0, 0);

  dy0 = 0;
  for k=1:K
    [dy, err] = fminsearch(@(dy) (marginlessdiff(microshift(img, 0, dy), ref)), 0);
    if err<er0
      dy0 = dy;
      er0 = err;
    end
  end
  ee.dy(t) = er0;
  dd.dy(t) = dy0;  
  img = microshift(img, 0, dy0);

  if bri0>0
    bri = mean(img(msk));
    img = img .* bri0/bri;
    dd.dbri(t) = 100*(bri0/bri-1);
  end

  ds0 = 0;
  for k=1:K
    [ds, err] = fminsearch(@(ds) (marginlessdiff(microscale(img, ds), ref)), 0);
    if err<er0
      ds0 = ds;
      er0 = err;
    end
  end
  ee.ds(t) = er0;
  dd.ds(t) = ds0;
  img = microscale(img, ds0);

  dx0 = 0;
  for k=1:K
    [dx, err] = fminsearch(@(dx) (marginlessdiff(microperspective(img, dx, 0), ref)), 0);
    if err<er0
      dx0 = dx;
      er0 = err;
    end
  end
  ee.dpx(t) = er0;
  dd.dpx(t) = dx0;
  img = microperspective(img, dx0, 0);

  dy0 = 0;
  for k=1:K
    [dy, err] = fminsearch(@(dy) (marginlessdiff(microperspective(img, 0, dy), ref)), 0);
    if err<er0
      dy0 = dy;
      er0 = err;
    end
  end
  ee.dpy(t) = er0;
  dd.dpy(t) = dy0;
  img = microperspective(img, 0, dy0);

  % Rotate?

  vsd(:,:,t) = img;
end
fprintf(2,'\n');

