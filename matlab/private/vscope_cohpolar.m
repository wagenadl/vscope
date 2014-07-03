function vscope_cohpolar(rr, theta, labels, r_thresh)

% Adapted from Adam Taylor's code
% arg processing
if iscell(rr)
  r_lo = rr{2};
  r_hi = rr{3};
  rr = rr{1};
else
  r_lo = rr;
  r_hi = rr;
end

if iscell(theta)
  theta_lo = theta{2};
  theta_hi = theta{3};
  theta = theta{1};
else
  theta_lo = theta;
  theta_hi = theta;
end

N = length(rr);

if nargin<3
  labels=cell(N,1);
  for k=1:N
    labels{k}=sprintf('%i', k);
  end
end  

if nargin<4
  r_thresh = 0;
end

[theta, theta_lo, theta_hi] = identity(-theta, -theta_hi, -theta_lo);

prepfig(5,5,2);

% plot spokes
th=[0 pi/2 pi 3*pi/2];
th_labels={'0' '90' '180' '270'};
for k=1:length(th)
  line([0 cos(th(k))],[0 sin(th(k))],...
       'color',[0.75 0.75 0.75]);
end

% draw unit circle
th=[0:pi/60:2*pi];
line(cos(th), sin(th), 'color', 'k');

% draw threshold circles
line(r_thresh*cos(th),...
    r_thresh*sin(th),...
    'Color','k',...
    'LineStyle','--');

% annotate spokes in degrees
rt = 1.01;
text(rt, 0, '0^\circ', 'horizontalalignment', 'left', 'verticalalignment', 'middle');
text(-rt, 0, '180^\circ', 'horizontalalignment', 'right', 'verticalalignment', 'middle');
text(0, rt, '90^\circ', 'horizontalalignment', 'center', 'verticalalignment', 'bottom');
text(0, -rt, '270^\circ', 'horizontalalignment', 'center', 'verticalalignment', 'top');

% set axis limits
axis([-1 1 -1 1]*1.12);

% calculate the color for each ROI
cc = ones(N,1,3);
ph = theta(:)/(2*pi);
idx=find(ph<0);
ph(idx)=ph(idx)+1;
mg = rr(:);
cc(:,:,1) = ph;
cc(:,:,2) = mg;
cc(:,:,3) = 1;
clr = squeeze(colorspace('rgb<-hsv',cc));

% plot each data point and error bars
[dd, ord] = sort(rr);
for k=ord(:)'  
  % calc cart coords of point
  x=rr(k)*cos(theta(k));
  y=rr(k)*sin(theta(k));  

  % calc cart coords of mag CI
  x_lo=r_lo(k)*cos(theta(k));
  y_lo=r_lo(k)*sin(theta(k));
  x_hi=r_hi(k)*cos(theta(k));
  y_hi=r_hi(k)*sin(theta(k));
  
  % calc cart coords of phase CI
  r_arc=rr(k);
  theta_arc=linspace(theta_lo(k), theta_hi(k), ...
      round((theta_hi(k)-theta_lo(k))/(pi/180)));
  x_arc=r_arc*cos(theta_arc);
  y_arc=r_arc*sin(theta_arc);
  
  myclr = clr(k,:);
  if rr(k)<r_thresh
    myclr = [.75 .75 .75];
  end

  % plot data point
  plot(x, y, '.', 'color', myclr, 'markersize', 8);
  
  % plot magnitude CI
  plot([x_lo x_hi],[y_lo y_hi], 'color', myclr);
  % plot phase CI
  plot(x_arc, y_arc, 'color', myclr);
end

% Label points
for k=ord(:)'
  x = rr(k)*cos(theta(k));
  y = rr(k)*sin(theta(k));
  if x>0 
    ha = 'left';
  else
    ha = 'right';
  end
  va = 'middle';
  h=text(x, y, [ ' ' labels{k} ' ' ], ...
      'horizontalalignment', ha, 'verticalalignment', va);
end 
