function f(t_o,roi_dff,...
           t_e,e_phys,plot_tog,...
           title_string,common_scale_flag,...
           roi_labels,roi_starred)

% this is a function to make a single figure showing all traces from a
% given trial.  It looks poorly on-screen, but it's very nice once
% printed, or when 'printed' to an image at => 300dpi.

% process args
if nargin<5
  plot_tog=ones(1,size(e_phys,2));
elseif isempty(plot_tog)
  plot_tog=zeros(1,10);
end
if nargin<6
  title_string='';
end
if nargin<7
  common_scale_flag=0;
elseif isempty(common_scale_flag)
  common_scale_flag=0;
end  
if nargin<8
  n_rois=size(roi_dff,2);
  roi_labels=cell(n_rois,1);
  for j=1:n_rois
    roi_labels{j}=int2str(j);
  end
elseif isempty(roi_labels);
  n_rois=size(roi_dff,2);
  roi_labels=cell(n_rois,1);
  for j=1:n_rois
    roi_labels{j}=int2str(j);
  end
end
if nargin<9
  roi_starred=zeros(1,size(roi_dff,2));
end

% get dims
n_rois=size(roi_dff,2);
n_frames=length(t_o);

idx=find(roi_starred);
if ~isempty(idx)
  n_rois = max(idx);
end

% figure out how many total plots there will be
n_e_plots=sum(plot_tog);
n_plots=n_e_plots+n_rois;

% setup time axis limits
if n_rois==0
  t_min=t_e(1);
  t_max=t_e(end);
elseif n_e_plots==0
  t_min=t_o(1);
  t_max=t_o(end);
else
  t_min=min(t_o(1),t_e(1));
  t_max=max(t_o(end),t_e(end));
end

% if a common scale is desired, figure out what that scale will be
if common_scale_flag
  data_max=100*max(roi_dff(:)); data_min=100*min(roi_dff(:));
  y_max=data_max; y_min=data_min;
end
  
% spec out the fig -- want the fig to fit on a single letter page w/
% set margins, but not too large an aspect ratio per plot
page_width_print=8.5;
page_height_print=11;
fig_height_print=9.5;
left_gutter_width_print=0.25;  % where the trace label goes
top_gutter_height_print=0.25;  % where the fig title goes
bottom_gutter_height_print=0.25;  % where the scale bars go
plot_height_print=(fig_height_print...
                   -top_gutter_height_print...
                   -bottom_gutter_height_print)/n_plots;
scale_bar_space_width_print=6/72;
margin_height_print=(page_height_print-fig_height_print)/2;
fig_width_print=3;
plot_width_print=fig_width_print-left_gutter_width_print;
% make the plot narrower if the individual plot aspect ratio will be
%   greater than crit_ratio:1
crit_ratio=15;
aspect_ratio=plot_width_print/plot_height_print;
if aspect_ratio > crit_ratio
  plot_width_print=crit_ratio*plot_height_print;
  fig_width_print=plot_width_print+left_gutter_width_print;
end
margin_width_print=(page_width_print-fig_width_print)/2;

% now compute normalized coordinates
margin_width_normed=margin_width_print/fig_width_print;
margin_height_normed=margin_height_print/fig_height_print;
fig_width_normed=1;
fig_height_normed=1;
left_gutter_width_normed=left_gutter_width_print/fig_width_print;
top_gutter_height_normed=top_gutter_height_print/fig_height_print;
bottom_gutter_height_normed=bottom_gutter_height_print/fig_height_print;
plot_width_normed=plot_width_print/fig_width_print;
plot_height_normed=plot_height_print/fig_height_print;

% compute fig size on-screen
% preserve the print aspect ratio, but make it fit on screen
margin_width_screen=30;  % pels
% need top & bot due to menu, toolbar
top_margin_height_screen=margin_width_screen+72;  
bottom_margin_height_screen=margin_width_screen;
cur_units=get(0,'Units');
set(0,'Units','pixels');
screen_size=get(0,'ScreenSize');
set(0,'Units',cur_units);
screen_width=screen_size(3);
screen_height=screen_size(4);
eff_screen_width=screen_width-2*margin_width_screen;
eff_screen_height=screen_height...
  -top_margin_height_screen-bottom_margin_height_screen;
eff_screen_aspect_ratio=eff_screen_width/eff_screen_height;
fig_aspect_ratio=fig_width_print/fig_height_print;
if fig_aspect_ratio>=eff_screen_aspect_ratio
  fig_width_screen=eff_screen_width;
  fig_height_screen=floor(fig_width_screen/fig_aspect_ratio);
else
  fig_height_screen=eff_screen_height;
  fig_width_screen=floor(fig_height_screen*fig_aspect_ratio);
end
fig_x_screen=round(screen_width-fig_width_screen)/2+1;
fig_y_screen=screen_height-fig_height_screen-top_margin_height_screen+1;

% correct time axis limits to fit in the scale bars
t_left=t_min;
frac=scale_bar_space_width_print/plot_width_print;
T_bar=(t_max-t_min)*frac/(1-frac);
t_right=t_max+T_bar;

% figure out how big the scale bars are going to be
dff_bar_size=0.1;
%v_bar_size=matrix_good_scale_bar(e_phys,plot_tog&isvoltage);
%i_bar_size=matrix_good_scale_bar(e_phys,plot_tog&iscurrent);
%e_bar_size=matrix_good_scale_bar(e_phys,plot_tog&isextra);

% set up the figure
figure(3); clf;
set(3,'PaperPosition',[margin_width_print ...
      margin_height_print ...
      fig_width_print ...
      fig_height_print],...
    'Color',[1 1 1],...
    'Position',[fig_x_screen fig_y_screen ...
      fig_width_screen fig_height_screen]);

% do the optical plots
plot_index=1;
for j=1:n_rois
  % set up an axes
  axes('Position',[left_gutter_width_normed ...
                   bottom_gutter_height_normed+...
                     plot_height_normed*(n_plots-plot_index) ...
                   plot_width_normed ...
                   plot_height_normed],...
       'Visible','off');
  % this is where the actual plotting happens  
  line(t_o,100*roi_dff(:,j),'Color',[0 0 0]);
  % this is where the plotting ends
  xlim([t_left t_right]);
  if ~common_scale_flag
    data_max=100*max(roi_dff(:,j)); data_min=100*min(roi_dff(:,j));
    y_max=data_max; y_min=data_min;
  end
  ylim([y_min y_max]);
  % draw the scale bar
  line([t_max+T_bar/2 t_max+T_bar/2],...
       [-dff_bar_size/2 +dff_bar_size/2],...
       'Color',[0 0 0],...
       'LineWidth',3);
  % draw the label
  text('Units','normalized',...
       'Position',[-left_gutter_width_normed/plot_width_normed/2 ...
                   0.5],...
       'HorizontalAlignment','center',...
       'VerticalAlignment','middle',...
       'String',sprintf('%s',roi_labels{j}),'buttondownfcn',@dwc_boldme);
  plot_index=plot_index+1;
end

% do the e-phys plots
labels={'v1','v2','ie1','i2',...
        'e2','e3',...
        'ac','e4'};
n_traces=size(e_phys,2);
for j=1:n_traces
  if plot_tog(j)
    % set up an axes
    axes('Position',[left_gutter_width_normed ...
                     bottom_gutter_height_normed+...
                       plot_height_normed*(n_plots-plot_index) ...
                     plot_width_normed ...
                     plot_height_normed],...
         'Visible','off');
    % this is where the actually plotting happens  
    line(t_e,e_phys(:,j),'Color',[0 0 0]);
    % this is where the plotting ends
    % set the axes
    xlim([t_left t_right]);
    if 0 % isttl(j)
      ylim([-1 6]);
    else
      data_max=max(e_phys(:,j)); data_min=min(e_phys(:,j));
      y_max=data_max; y_min=data_min;
      if y_min>=y_max
	dy=1;
      else
	dy=0;
      end
      ylim([y_min-dy y_max+dy]);
    end
    % draw scale bar, if necessary
    y_mid=mean(ylim);
    %if isvoltage(j)
    %  line([t_max+T_bar/2 t_max+T_bar/2],...
    %       [y_mid-v_bar_size/2 y_mid+v_bar_size/2],...
    %        'Color',[0 0 0],...
    %        'LineWidth',3);
    %elseif iscurrent(j)
    %  line([t_max+T_bar/2 t_max+T_bar/2],...
    %       [y_mid-i_bar_size/2 y_mid+i_bar_size/2],...
    %        'Color',[0 0 0],...
    %        'LineWidth',3);
    %elseif isextra(j)
    %  line([t_max+T_bar/2 t_max+T_bar/2],...
    %       [y_mid-e_bar_size/2 y_mid+e_bar_size/2],...
    %        'Color',[0 0 0],...
    %        'LineWidth',3);
    %end
    % add the label
    text('Units','normalized',...
         'Position',[-left_gutter_width_normed/plot_width_normed/2 ...
                     0.5],...
         'HorizontalAlignment','center',...
         'VerticalAlignment','middle',...
         'String',labels{j});
    plot_index=plot_index+1;
  end
end

% draw the bottom gutter decorations
% figure out how big the time scale bar will be
t_bar_size=1;  %s
% set up an axes
axes('Position',[left_gutter_width_normed ...
                 0 ...
                 plot_width_normed ...
                 bottom_gutter_height_normed],...
     'Visible','off');
% this is where the actually plotting happens  
line([t_right-t_bar_size*3/2 t_right-t_bar_size*1/2],...
     [0 0],...
     'Color',[0 0 0],...
     'LineWidth',3,...
     'Clipping','off');
% this is where the plotting ends
% set the axes limits
xlim([t_left t_right]);
ylim([-1 +1]);

% add the string the tells what the scales are
scale_string='';
if n_rois>0
  scale_string=sprintf('%s%.1f%%, ',scale_string,dff_bar_size);
end
%if any(plot_tog&isvoltage)
%  scale_string=sprintf('%s%.0f mV, ',scale_string,v_bar_size);
%end
%if any(plot_tog&iscurrent)
%  scale_string=sprintf('%s%.0f nA, ',scale_string,i_bar_size);
%end
%if any(plot_tog&isextra)
%  scale_string=sprintf('%s%.1g V, ',scale_string,e_bar_size);
%end
scale_string=sprintf('%s%.0f s',scale_string,t_bar_size);
text('Units','normalized',...
     'Position',[1/2-left_gutter_width_normed/plot_width_normed/2 1/2],...
     'HorizontalAlignment','center',...
     'VerticalAlignment','middle',...
     'String',scale_string);

% draw the top gutter decorations
% set up an axes
axes('Position',[0 ...
                 1-top_gutter_height_normed ...
                 1 ...
                 top_gutter_height_normed],...
     'Visible','off');
text('Units','normalized',...
     'Position',[1/2 1/2],...
     'HorizontalAlignment','center',...
     'VerticalAlignment','middle',...
     'String',title_string);

set(gca,'buttondownfcn',@dwc_boldme);
