 function img = microperspective(img, dx, dy)
% MICROPERSPECTIVE - Microperspectived image
%    img = MICROPERSPECTIVE(img, dx, dy) perspectivises the image IMG by
%    DX pixels in x and DY pixels in y. That's at the extreme edge of
%    the figure. If the original image is YxX 
%    pixels, the output is (Y-2)x(X-2), because edge pixels are trimmed.
%    Edges are zerod if that if DX or DY is so big that trimming one pixel
%    is not enough. 

[Y X] = size(img);

xx=[1:X];
yy=[1:Y]';
x0 = mean(xx);
y0 = mean(yy);
xx = repmat(xx - x0, [Y 1]);
yy = repmat(yy - y0, [1 X]);
RX = X/2;
RY = Y/2;
if dx~=0
  x_ = xx.*(1+dx*yy/RX/RY);
end
if dy~=0
  yy = yy.*(1+dy*xx/RX/RY);
end
if dx~=0
  xx=x_;
end
xx = xx+x0;
yy = yy+y0;
img = interp2([1:X], [1:Y], img, xx, yy, 'linear');
img=img(2:end-1,2:end-1);
img(isnan(img))=0;

