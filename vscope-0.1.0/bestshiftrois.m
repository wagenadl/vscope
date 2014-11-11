function dx = bestshiftrois(img, rf1, rf2, rois)
% BESTSHIFTROIS - Calculate best shift for image based on two reference images
%   dx = BESTSHIFT(img, rf1, rf2, rois) where RF1 and RF2 are two copies of a 
%   reference image microshifted by 0.5 pixels in opposite directions, 
%   returns the optimal microshift amounts for IMG to match the original
%   reference image for each of the ROIs in ROIS, which much be a struct array
%   from VSCOPE_ROICOORDS_CAM.

[Y X] = size(img);
[Y_ X_] = size(rf1);
N = length(rois);
idx = cell(N,1);
if Y_<Y
  img = img(2:end-1, 2:end-1);
  for n=1:N
    use = find(rois(n).x>1 & rois(n).x<X & rois(n).y>1 & rois(n).y<Y);
    idx{n} = Y_*(rois(n).x(use)-2) + rois(n).y(use)-1;
  end
else
  for n=1:N
    idx{n} = Y_*(rois(n).x-1) + rois(n).y;
  end
end

dx = zeros(N, 1);
for n=1:N
  if ~isempty(idx{n})
    drf = rf1(idx{n}) - rf2(idx{n});
    a = sum(drf.*(img(idx{n})-rf2(idx{n}))) ./ sum(drf.^2);
    dx(n) = a - 0.5;
  end
end
