function dx = bestshift(img, rf1, rf2, margx, margy)
% BESTSHIFT - Calculate best shift for image based on two reference images
%   dx = BESTSHIFT(img, rf1, rf2) where RF1 and RF2 are two copies of a 
%   reference image microshifted by 0.5 pixels in opposite directions, 
%   returns the optimal microshift amount for IMG to match the original
%   reference image.
%   A margin of 16 pixels in X and 4 pixels in Y is ignored for the calculation
%   if the images are 512x128. This default may be changed:
%   dx = BESTSHIFT(img, rf1, rf2, margx, margy)

[Y X] = size(img);

if nargin<4
  margx = 4*max(1, round(X/128));
  margy = 4*max(1, round(Y/128));
elseif nargin<5
  margy = margx;
end

img = img(margy+1:end-margy, margx+1:end-margx);
rf1 = rf1(margy+1:end-margy, margx+1:end-margx);
rf2 = rf2(margy+1:end-margy, margx+1:end-margx);
drf = rf1(:)-rf2(:);
a = sum(drf.*(img(:)-rf2(:))) ./ sum(drf.^2);
dx = a - 0.5;
