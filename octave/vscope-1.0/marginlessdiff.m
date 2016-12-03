function e = marginlessdiff(img1, img2, margx, margy)
% MARGINLESSDIFF - RMS difference between images sans margins
%   e = MARGINLESSDIFF(img1, img2) calculates the RMS difference between
%   the two images IMG1 and IMG2.
%   A margin of 16 pixels in X and 4 pixels in Y is ignored for the calculation
%   if the images are 512x128. This default may be changed:
%   dx = MARGINLESSDIFF(img1, img2, margx, margy)

[Y X] = size(img1);

if nargin<3
  margx = 4*max(1, round(X/128));
  margy = 4*max(1, round(Y/128));
elseif nargin<4
  margy = margx;
end

e = sqrt(sum(sum((img1(margy+1:end-margy, margx+1:end-margx) ...
    - img2(margy+1:end-margy, margx+1:end-margx)).^2, 1), 2));
