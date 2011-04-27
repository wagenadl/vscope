function [hp,ht]=f(im,roi_stack,...
           C_mag_star,C_phase_star,...
           C_mag_thresh,...
	   clr_subthresh,...
           title_string,rois,roi_labels)

% assume C_phase_star is in radians, and is corrected for dye-induced
% phase shifts

% get the number of rois
% n_rois=length(roi_list);
n_rois=length(rois);
NVERT=16;
roi_xx=cell(n_rois,1);
roi_yy=cell(n_rois,1);
for n=1:n_rois
  [roi_xx{n}, roi_yy{n}] = vscope_roioutline(rois{n})
end

hp=zeros(n_rois,1)+nan;
ht=zeros(n_rois,1)+nan;


% truncate C_mag_star and C_phase_star
C_mag_star=C_mag_star(1:n_rois);
C_phase_star=C_phase_star(1:n_rois);

% deal with inputs
%if nargin<6
%  below_thresh_special=0;
%elseif isempty(below_thresh_special)
%  below_thresh_special=0;
%end
if nargin<5
  C_mag_thresh=0;
elseif isempty(C_mag_thresh)
  C_mag_thresh=0;
end
if nargin<6
  title_string=[];
end

% generate the image figure
prepfig(5,4,1);
set(gcf,'color','w');
colormap(1-gray(256));
set(gcf,'PaperPosition',[1.5833 3.5 5.3333 4]);
h=imagesc(unsharpmask(im,5,.5));
set(h,'buttondownfcn',@dwc_boldme);
axis image;
set(gca,'XTick',[]);
set(gca,'YTick',[]);
h=colorbar;
set(h,'position',[.85 .1 .05 .8]);
title(title_string);

% generate the ROI color map (for display only)
n_colors=1024;
cc = ones(n_colors,1,3);
ph = [0:2*pi/n_colors:2*pi-1e-9]';
cc(:,:,1) = ph/(2*pi);
cc(:,:,2) = .75;
cmap = colorspace('rgb<-hsv',cc);

% change the colorbar to show the ROI colormap
colorbar_axes_h=findobj(gcf,'Tag','Colorbar');
colorbar_image_h=findobj(colorbar_axes_h,'Tag','TMW_COLORBAR');
set(colorbar_image_h,'CData',reshape(cmap,[n_colors 1 3]));
set(colorbar_image_h,'YData',[0 360]);
set(colorbar_axes_h,'YLim',[0 360]);
set(colorbar_axes_h,'YTick',[0:90:360]);

% calculate the color for each ROI
% make the subthresh ones a special color

cc = ones(n_rois,1,3);
ph = -C_phase_star'/(2*pi);
idx=find(ph<0);
ph(idx)=ph(idx)+1;
mg = C_mag_star';
cc(:,:,1) = ph;
cc(:,:,2) = mg;
cc(:,:,3) = 1;
roi_color=squeeze(colorspace('rgb<-hsv',cc));
roi_color(C_mag_star<=C_mag_thresh,:)=...
  repmat(clr_subthresh,[sum(C_mag_star<=C_mag_thresh) 1]);

% draw the rois, w/ labels
for j=1:n_rois
  %   border=roi_list(j).border;
  %   com=border_com(border);
  if(C_mag_star(j)>C_mag_thresh)
    clr=roi_color(j,:);
    z=C_mag_star(j);
    hp(j) = patch('FaceColor',clr,...
	'EdgeColor','none',...
	'XData',roi_xx{j},...
	'YData',roi_yy{j});
    ht(j) = text(mean(roi_xx{j}),mean(roi_yy{j}),...
	roi_labels{j},...
	'HorizontalAlignment','center',...
	'VerticalAlignment','middle',...    
	'Color','k', 'buttondownfcn',@dwc_boldme);     
  end
end

set(gca,'ydir','rev');

if nargout<1
  clear hp
end
if nargout<2
  clear ht
end
