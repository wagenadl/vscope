function img = microshift(img, dx, dy)
% MICROSHIFT - Microshifted image
%    img = MICROSHIFT(img, dx, dy) shifts the image IMG by DX and DY pixels
%    in the horizontal (second) and vertical (first) dimensions. 
%    The edge pixels are not trimmed, but they may become invalid (nan)

[Y X] = size(img);

if dx>0
  dx = min(dx,1);
  img = [[(1-dx)*img(:,1:end-1)+dx*img(:,2:end)] img(:,end)];
elseif dx<0
  dx = min(-dx,1);
  img = [img(:,1) [dx*img(:,1:end-1)+(1-dx)*img(:,2:end)]];
end

if dy>0
  dy = min(dy,1);
  img = [[(1-dy)*img(1:end-1,:)+dy*img(2:end,:)]; img(end,:)];
elseif dy<0
  dy = min(-dy,1);
  img = [img(1,:); [dy*img(1:end-1,:)+(1-dy)*img(2:end,:)]];
end

