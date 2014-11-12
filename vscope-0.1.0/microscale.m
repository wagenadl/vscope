 function img = microscale(img, dx)
% MICROSCALE - Microscaleed image
%    img = MICROSCALE(img, dx) scales the image IMG by DX 'pixels'
%    in both dimensions. Pixels are measured in the finer dimension 
%    (usually, X); the scale is isotropic assuming the image is square.
%    Edge pixels may become nan.

[Y X] = size(img);
R = max(Y,X)/2;

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = (xx-x0)*(1+dx/R)+x0;
yy = (yy-y0)*(1+dx/R)+y0;
xx = repmat(xx, [Y 1]);
yy = repmat(yy, [1 X]);
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');
