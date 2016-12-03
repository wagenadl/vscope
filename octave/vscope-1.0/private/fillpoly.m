function img = fillpoly(Y,X,xyxy)
% FILLPOLY  Fills a polygon
%   img = FILLPOLY(Y,X,xyxy) creates an image of size YxX, with pixels
%   inside the polygon defined by XYXY set to logical one, others to zero.
%   XYXY must be Nx2, with XYXY(:,1) representing x-coordinates, and
%   XYXY(:,2) representing y-coordinates.
%   The output is suitable for IMAGESC, or for EXTRACTPOLYROI.

xx=repmat([1:X],[Y 1]); 
yy=repmat([1:Y]',[1 X]);
img = inpolygon(xx,yy,xyxy(:,1),xyxy(:,2));
return

img = logical(ones(Y,X));
xx=repmat([1:X],[Y 1]);
yy=repmat([1:Y]',[1 X]);
[N,D]=size(xyxy);
xyxy1=xyxy([[2:N] 1],:);
for n=1:N
  dx=xyxy(n,1)-xx;
  dy=xyxy(n,2)-yy;
  dx_=xyxy1(n,1)-xx;
  dy_=xyxy1(n,2)-yy;
  sg = sign(dx.*dy_ - dy.*dx_);
  if n==1
    sgn=sg;
  else
    img(sg~=sgn) = 0;
  end
end
