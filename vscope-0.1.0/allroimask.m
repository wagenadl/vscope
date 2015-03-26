function msk = allroimask(x, cam, marg)
[Y X C T] = size(x.ccd.dat);
msk = zeros(Y,X);
xyxy = vscope_roicoords_cam(x, find(strcmp(cam, x.ccd.info.camid)));
for k = 1:length(xyxy)
  msk(xyxy(k).y + Y*(xyxy(k).x-1)) = 1;
end

if nargin>2
  marg = ceil(marg)*2 + 1;
  msk = convn(msk, ones(marg, 1), 'same');
  msk = convn(msk, ones(1, marg), 'same');  
end
msk = msk>0;
