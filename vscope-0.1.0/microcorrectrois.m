function [vsd, dd, ee] = microcorrectrois(vsd, rois)
% MICROCORRECTROIS - One step microcorrection
%    out = MICROCORRECTROIS(vsd, rois) performs one iteration of 
%    microshifting, for each of the ROIs in ROIS, which much be a 
%    struct array from VSCOPE_ROICOORDS_CAM.
%    [out, dd, ee] = MICROCORRECT(vsd, rois) returns the changes 
%    applied and the errors at all moments.
%
%    Example:
%       cam = find(strcmp('Bot', x.ccd.info.camid));
%       vsd = x.ccd.dat(:,:,cam,:);
%       rois = vscope_roicoords_cam(x, cam);
%       [out, dd, ee] = microcorrectrois(vsd, rois);

[Y X T] = size(vsd);
SX = 0.5; % * round(X/128);
SY = 0.5; % * round(Y/128);
T0 = ceil(T/2);

ref = vsd(:,:,T0);

N = length(rois);
idx = cell(N,1);
for n=1:N
  ok = find(rois(n).x>1 & rois(n).x<X & rois(n).y>1 & rois(n).y<Y);
  rois(n).x = rois(n).x(ok);
  rois(n).y = rois(n).y(ok);
  idx{n} = Y*(rois(n).x-1) + rois(n).y;
end

ee.e0 = zeros(T,N);
for t=1:T
  v0 = vsd(:,:,t);
  for n=1:N
    if ~isempty(idx{n})
      ee.e0(t,n) = sqrt(mean((v0(idx{n}) - ref(idx{n})).^2));
    end
  end
end

dd.t0 = T0;

rfx1_ = microshift(ref, -SX, 0);
rfx2_ = microshift(ref, SX, 0);
rfy1_ = microshift(ref, 0, -SY);
rfy2_ = microshift(ref, 0, SY);

rfx1 = ref; rfx1(2:end-1,2:end-1) = rfx1_;
rfx2 = ref; rfx2(2:end-1,2:end-1) = rfx2_;
rfy1 = ref; rfy1(2:end-1,2:end-1) = rfy1_;
rfy2 = ref; rfy2(2:end-1,2:end-1) = rfy2_;

dd.dx = zeros(T,N);
dd.dy = zeros(T,N);

for t=1:T
  fprintf(1, '%i/%i\r', t,T);
  fflush(1);
  if t==T0
    continue;
  end
  img = vsd(:,:,t);
  er0 = ee.e0(t,:);
  
  dx = bestshiftrois(img, rfx1, rfx2, rois)*SX/0.5;
  dy = bestshiftrois(img, rfy1, rfy2, rois)*SY/0.5;
  
  out = microshiftrois(img, dx, dy, rois);
  err = zeros(N,1);
  for n=1:N
    if ~isempty(idx{n})
      err(n) = sqrt(mean((out(idx{n})-ref(idx{n})).^2));
      ee.dshift(t,n) = err(n);
      if err(n)<er0(n)
	img(idx{n}) = out(idx{n});
	dd.dx(t,n) = dx(n);
	dd.dy(t,n) = dy(n);
	er0(n) = err(n);
      end
    end
  end
  vsd(:,:,t) = img;
end
fprintf(1,'\n');
fflush(1);
