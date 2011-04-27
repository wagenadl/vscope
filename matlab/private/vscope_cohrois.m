function vscope_cohrois(...
    im, ...
    rois, ...
    roi_labels, ...
    C_mag_star,...
    C_phase_star,...
    C_mag_thresh,...
    title_string)

% VSCOPE_COHROIS

% This code is based on Adam Taylor's code.

n_rois=length(rois);
NVERT=16;
roi_xx=cell(n_rois,1);
roi_yy=cell(n_rois,1);
for n=1:n_rois
  [roi_xx{n}, roi_yy{n}] = vscope_roioutline(rois{n});
end

if nargin<6
  C_mag_thresh=0;
end
if nargin<7
  title_string=[];
end

% generate the image figure
prepfig(4.5,4,1);
set(gcf,'color','w');
set(gca,'ydir','rev');
colormap(1-gray(256));
h=imagesc(unsharpmask(im,5,.5));
[Y X]=size(im);
axis([0 X*4.5/4 0 Y]);
ca = caxis;

% show the ROI colormap
n_colors=512;
cc = ones(n_colors,1,3);
ph = [0:2*pi/n_colors:2*pi-1e-9]';
cc(:,:,1) = ph/(2*pi);
cc(:,:,2) = .65;
cmap = colorspace('rgb<-hsv',cc);
image(X*[4.03 4.09]/4, Y*[.95 .05], repmat(cmap, [1 2 1]));
for ph=[0:90:360]
  text(X*4.16/4, .95*Y-.9*Y*ph/360,sprintf('%g{}^{\\circ}',ph));
end
caxis(ca);

% calculate the color for each ROI
cc = ones(n_rois,1,3);
ph = -C_phase_star'/(2*pi);
idx=find(ph<0);
ph(idx)=ph(idx)+1;
mg = C_mag_star';
cc(:,:,1) = ph;
cc(:,:,2) = mg;
cc(:,:,3) = 1;
roi_color=squeeze(colorspace('rgb<-hsv',cc));

% draw the rois, w/ labels
idx = find(C_mag_star > C_mag_thresh);
for k = idx(:)'
  patch('FaceColor',roi_color(k,:),...
      'EdgeColor','none',...
      'XData',roi_xx{k},...
      'YData',roi_yy{k});
  text(mean(roi_xx{k}),mean(roi_yy{k}),...
      roi_labels{k},...
      'HorizontalAlignment','center',...
      'VerticalAlignment','middle',...    
      'Color','k');
end

if ~isempty(title_string)
  text(X*.95,Y*.05,title_string,'verticala','middle','horizontala','right');
end
