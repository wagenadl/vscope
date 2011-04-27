function f(r,theta,...
           r_lo,theta_lo,...
           r_hi,theta_hi,...
           roi_ids,roi_labels,...
           r_show_thresh,...
           r_color_thresh,...
           r_max_equals_one,...
           clr_subthresh)

% this function can also be used to plot other vector quatities, 
% not just coherence

theta = -theta;
x0 = theta_lo;
theta_lo = -theta_hi;
theta_hi = -x0;

% arg processing
if nargin<6
  r_lo=r;
  theta_lo=theta;
  r_hi=r;
  theta_hi=theta;
end
if nargin<8
  roi_labels=cell(length(r),1);
  for j=1:length(r)
    roi_labels{j}=int2str(j);
  end
end
if nargin<9
  r_show_thresh=-1;
elseif isempty(r_show_thresh)
  r_show_thresh=-1;
end  
if nargin<10
  r_color_thresh=-1;
elseif isempty(r_color_thresh)
  r_color_thresh=-1;
end

% prelims
prepfig(5,5,2);
axes('DataAspectRatio',[1 1 1],...
     'Box','off',...
     'Visible','off',...
     'Position',[0 0 1 1]);
hold on;
set(gca,'buttondownfcn',@dwc_boldme);

% define a unit circle
th = 0:pi/50:2*pi;
xunit = cos(th);
yunit = sin(th);
% now really force points on x/y axes to lie on them exactly
inds = 1:(length(th)-1)/4:length(th);
xunit(inds(2:2:4)) = zeros(2,1);
yunit(inds(1:2:5)) = zeros(3,1);

% the max r value we expect
if nargin<11
  r_max=1;
elseif isempty(r_max_equals_one)
  r_max=1;
elseif r_max_equals_one
  r_max=1;
else
  r_max=1.05*max(r);
end

% plot spokes
th=[0 pi/2 pi 3*pi/2];
th_labels={'0' '90' '180' '270'};
costh=[1 0 -1 0];
sinth=[0 1 0 -1];
for j=1:length(th)
  line([0 r_max*costh(j)],[0 r_max*sinth(j)],...
       'color',[0.75 0.75 0.75]);
end

% draw unit circle
line(r_max*xunit,r_max*yunit,'color','k');

% draw threshold circles
if length(r_show_thresh)==1
  if r_show_thresh>0
    line(r_show_thresh*xunit,...
         r_show_thresh*yunit,...
         repmat(2*length(r),size(xunit)),...
         'Color','k',...
         'LineStyle','--');
  end
end  
if length(r_color_thresh)==1
  if r_color_thresh>0
    line(r_color_thresh*xunit,...
         r_color_thresh*yunit,...
         repmat(2*length(r),size(xunit)),...
         'Color','k',...
         'LineStyle','--');
  end  
end

% annotate spokes in degrees
rt = 1.01*r_max;
for j = 1:length(th)
  [ha,va]=angle_to_alignment(th(j));
  text(rt*costh(j),rt*sinth(j),th_labels{j},...
      'color','k',...
       'horizontalalignment',ha,...
       'verticalalignment',va,...
       'Clipping','on');
end

% label the radius
if r_max~=1
  r_label_angle=pi/4;
  [ha,va]=angle_to_alignment(r_label_angle);
  text(rt*cos(r_label_angle),...
       rt*sin(r_label_angle),...
       sprintf('%0.2g',r_max),...
       'color','k',...
       'horizontalalignment',ha,...
       'verticalalignment',va,...
       'Clipping','on');
end

% set axis limits
axis(r_max*[-1 1 -1 1]*1.1);

% calculate the color for each ROI

cc = ones(length(r),1,3);
ph = theta'/(2*pi);
idx=find(ph<0);
ph(idx)=ph(idx)+1;
mg = r';
cc(:,:,1) = ph;
cc(:,:,2) = mg;
cc(:,:,3) = 1;
clr = squeeze(colorspace('rgb<-hsv',cc));
clr(r<=r_color_thresh,:) = ...
  repmat(clr_subthresh,[sum(r<=r_color_thresh) 1]);

% plot each data point and error bars
for j=length(r):-1:1
  % only plot if it shows some evidence of being interesting
  if length(r_show_thresh)==1
    show_this_one=(r(j)>r_show_thresh);
  else
    show_this_one=(r(j)>r_show_thresh(j));
  end     
  if show_this_one
    % calc cart coords of point
    x=r(j)*cos(theta(j));
    y=r(j)*sin(theta(j));  
    % calc cart coords of mag CI
    x_lo=r_lo(j)*cos(theta(j));
    y_lo=r_lo(j)*sin(theta(j));
    x_hi=r_hi(j)*cos(theta(j));
    y_hi=r_hi(j)*sin(theta(j));
    % calc cart coords of phase CI
    r_arc=r(j);
    theta_arc=linspace(theta_lo(j),...
                       theta_hi(j),...
                       round((theta_hi(j)-theta_lo(j))/(pi/180)));
    x_arc=r_arc*cos(theta_arc);
    y_arc=r_arc*sin(theta_arc);
    % plot data point
    line(x,y,r(j),...
         'MarkerFaceColor',clr(j,:),...
         'MarkerEdgeColor','none',...
         'Marker','o','MarkerSize',6,...
         'LineStyle','none');
    % plot magnitude CI
    line([x_lo x_hi],[y_lo y_hi],[r(j) r(j)],'Color',clr(j,:));
    % plot phase CI
    line(x_arc,y_arc,repmat(r(j),size(x_arc)),'Color',clr(j,:));
    % plot index
    h=text('String',roi_labels{j});
    text_extent=get(h,'Extent'); shim_size=max(text_extent(3:4))/2;
    dr_hat=[cos(theta(j)) sin(theta(j))];
    dtheta_hat=[-dr_hat(2) dr_hat(1)];
    pos_text=(r(j)+shim_size)*dr_hat+shim_size*dtheta_hat;
    set(h,...
        'Position',[pos_text r(j)],...
        'HorizontalAlignment','center',...
        'VerticalAlignment','middle',...
        'Color','k',...
        'Clipping','on', 'buttondownfcn',@dwc_boldme);
  end
end 

% change the size so that two fit comfortably on a page
%set(gcf,'PaperPosition',[1.5833 3.5 5.3333 4]);

% free the plot
hold off;

