 function img = microrotate(img, dx)
% MICROROTATE - Microrotated image
%    img = MICROROTATE(img, dx) rotates the image IMG by
%    DX fine pixels at the extreme end. 
%    Edge pixels may become nan as a result.


[Y X] = size(img);

R = max(X,Y)/2;
dx = dx/R; % Convert to radians

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = repmat(xx - x0, [Y 1]);
yy = repmat(yy - y0, [1 X]);
yy = yy*X/Y;
x_ = xx*cos(dx) + yy*sin(dx);
yy = -xx*sin(dx) + yy*cos(dx);
xx = x_;
xx = xx+x0;
yy = yy*Y/X;
yy = yy+y0;
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');


