function vscope_proofsheet(t_o, roi_dff, roi_labels, ...
           t_e, e_phys, eph_info, plot_tog,...
           title_string);

% this is a function to make a single figure showing all traces from a
% given trial.  It looks poorly on-screen, but it's very nice once
% printed, or when 'printed' to an image at => 300dpi.

% process args
if nargin<6
  plot_tog=ones(1,size(e_phys,2));
end
if nargin<7
  title_string='';
end

% get dims
[n_frames, n_rois] = size(roi_dff);

% figure out how many total plots there will be
n_e_plots=sum(plot_tog);
n_plots=n_e_plots+n_rois;

% setup time axis limits
if n_rois==0
  t_min = t_e(1);
  t_max = t_e(end);
elseif n_e_plots==0
  t_min = t_o(1);
  t_max = t_o(end);
else
  t_min = min(t_o(1), t_e(1));
  t_max = max(t_o(end), t_e(end));
end
t_range = t_max-t_min;

dff_bar_size=0.1;

% set up the figure
prepfig(7, 10, 3);

% do the optical plots
plot_index=1;
dy0 = .94/n_plots;
yy0 = .03 + [n_plots-1:-1:0]*dy0;
for k=1:n_rois
  axes('Position',[0 yy0(k) 1 dy0], 'visible', 'off');
  hold on
  plot(t_o, 100*roi_dff(:,k), 'k');
  axis tight
  a=axis;
  y0 = (a(3)+a(4))/2;
  plot([0 0] + t_max + .02*t_range, y0+[-.5 .5]*dff_bar_size, ...
      'k', 'linewidth', 3);
  text(t_min - .01*t_range, y0, roi_labels{k}, 'horizontala', 'right');
  axis([t_min - .05*t_range, t_max + .03*t_range, a(3), a(4)]);
end

% do the e-phys plots
n_traces=size(e_phys,2);
idx = n_rois + cumsum(plot_tog);
for k=1:n_traces
  if plot_tog(k)
    axes('position', [0 yy0(idx(k)) 1 dy0], 'visible', 'off');
    hold on
    plot(t_e, e_phys(:,k), 'k');
    axis tight
    a=axis;
    y0 = (a(3)+a(4))/2;
    text(t_min - .05*t_range, y0, [ ' ' eph_info.chanid{k} ],...
	'horizontala', 'left', 'interp', 'none');
    dy = sensiblestep(a(4)-a(3));
    plot([0 0] + t_max + .02*t_range, y0+[-.5 .5]*dy, ...
	'k', 'linewidth', 3);
    text(t_max + .02*t_range, y0, sprintf('%g %s ',dy, eph_info.units{k}), ...
	'horizontala', 'right');
    axis([t_min - .05*t_range, t_max + .03*t_range, a(3), a(4)]);
  end
end

% draw the bottom gutter decorations
% figure out how big the time scale bar will be
t_bar_size=1;  % seconds
% set up an axes
axes('position', [0 0 1 .03], 'visible', 'off');
hold on
plot(t_max - [0, t_bar_size], [0 0], ...
    'k', 'linewidth', 3);
axis([t_min - .05*t_range, t_max + .03*t_range, -.5, .5]);

% add the string the tells what the scales are
scale_string = 'Scale bars: ';
if n_rois>0
  scale_string = [ scale_string sprintf('%.1f%%', dff_bar_size) ];
end
scale_string = [ scale_string ', ' sprintf('%g s',t_bar_size) ];

text((t_min+t_max)/2, 0, scale_string, ...
     'horizontalalignment', 'center', ...
     'verticalalignment', 'middle');

% draw the top gutter decorations
axes('position',[0 .97 1 .03], 'visible', 'off');
axis([t_min - .05*t_range, t_max + .03*t_range, -.5, .5]);
text((t_min+t_max)/2, 0, title_string, ...
     'horizontalalignment', 'center',...
     'verticalalignment', 'middle');
