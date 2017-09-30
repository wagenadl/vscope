function [vsd, dd, ee] = microcorrectroisslow(vsd, rois)
% MICROCORRECTROISSLOW - One step microcorrection - slow version with optimization
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
K = 3;
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
  out = microshiftrois(img, dx, 0, rois);
  err = zeros(N,1);
  for n=1:N
    if ~isempty(idx{n})
      err(n) = sqrt(mean((out(idx{n})-ref(idx{n})).^2));
      ee.dx(t,n) = err(n);
      if err(n)<er0(n)
	img(idx{n}) = out(idx{n});
	dd.dx(t,n) = dx(n);
	er0(n) = err(n);
      end
    end
  end

  dy = bestshiftrois(img, rfy1, rfy2, rois)*SY/0.5;
  out = microshiftrois(img, 0, dy, rois);
  err = zeros(N,1);
  for n=1:N
    if ~isempty(idx{n})
      err(n) = sqrt(mean((out(idx{n})-ref(idx{n})).^2));
      ee.dy(t,n) = err(n);
      if err(n)<er0(n)
	img(idx{n}) = out(idx{n});
	dd.dy(t,n) = dy(n);
	er0(n) = err(n);
      end
    end
  end
  vsd(:,:,t) = img;
end
fprintf(1,'\n');
fflush(1);
