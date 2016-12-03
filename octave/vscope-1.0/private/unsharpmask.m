function img = unsharpmask(img,radius,strength)
% UNSHARPMASK - Unsharp mask sharpening of an image
%    img = UNSHARPMASK(img,radius,strength)

[Y X C]=size(img);
for c=1:C
  blr=gaussianblur(img(:,:,c),radius);
  blr0=mean(blr(:));
  img(:,:,c) = img(:,:,c) - (blr - blr0) * strength;
end
