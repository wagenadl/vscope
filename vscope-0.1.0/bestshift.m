function dx = bestshift(img, rf1, rf2)
% BESTSHIFT - Calculate best shift for image based on two reference images
%   dx = BESTSHIFT(img, rf1, rf2) where RF1 and RF2 are two copies of a 
%   reference image microshifted by 0.5 pixels in opposite directions, 
%   returns the optimal microshift amount for IMG to match the original
%   reference image.

[Y X] = size(img);
[Y_ X_] = size(rf1);
if Y_<Y
  img = img(2:end-1, 2:end-1);
end

if 0
  % In my testing, normalizing doesn't matter, but it could
  img = img-mean(img(:));
  rf1 = rf1-mean(rf1(:));
  rf2 = rf2-mean(rf2(:));
  img = img./std(img(:));
  rf1 = rf1./std(rf1(:));
  rf2 = rf2./std(rf2(:));
end
if 1
  img = img./mean(img(:));
  rf1 = rf1./mean(rf1(:));
  rf2 = rf2./mean(rf2(:));
end

drf = rf1(:) - rf2(:);
a = sum(drf.*(img(:)-rf2(:))) ./ sum(drf.^2);
dx = a - 0.5;
