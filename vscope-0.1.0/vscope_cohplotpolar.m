function vscope_cohplotpolar(coh, varargin)
% VSCOPE_COHPLOTPOLAR - Polar plot of coherence results
%    VSCOPE_COHPLOTPOLAR(coh) plots the results of VSCOPE_COHANALYSIS.
%    VSCOPE_COHPLOTPOLAR(coh, key, value, ...) specifies additional 
%    parameters:
%       qpt - filename for QPlot figure (empty for current)
%       width - width of QPlot figure (inch)
%       height - height of QPlot figure

kv = getopt('qpt=''/tmp/vscope_coh_polar'' width=5 height=5', varargin);
if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

N = length(coh.coh);

%% Draw grid
% Draw cross
qpen 777 0
qplot([-1 1], [0 0]);
qplot([0 0], [-1 1]);

% Draw minor circles
ph = [0:.01:2*pi];
qpen 888
for r = [0.25 .5 .75]
  qplot(r*cos(ph), r*sin(ph));
end

% Draw threshold
qpen k dash
if isfield(coh, 'thr')
  qplot(coh.thr*cos(ph), coh.thr*sin(ph));
end

% Draw outer circle
qpen k solid
qplot(cos(ph), sin(ph));

% Place ticks
for phi=[0:pi/2:6]
  qat(cos(phi), sin(phi), -phi);
  qline([-3 3], [0 0]);
end

% Place labels
qat(1, 0);
qalign left middle
qtext(7, 0, '0⁰');
qat(0, 1);
qalign center bottom
qtext(0, -7, '90⁰');
qat(-1, 0);
qalign right middle
qtext(-7, 0, '180⁰');
qat(0, -1);
qalign center top
qtext(0, 7, '-90⁰');
qalign left top
qtextonpath(cos([0:.01:pi]), sin([0:.01:pi]), 20, 7, 'Later →');
qalign right top
qtextonpath(cos([pi:.01:2*pi]), sin([pi:.01:2*pi]), -20, 7, '← Earlier');

%% Draw error bars
if isfield(coh, 'phase_lo')
  for k=1:N
    if ~isnan(coh.coh(k))
      qpen(coh.cc(k,:), -.5);
      ph = [coh.phase_lo(k):.01:coh.phase_hi(k)];
      qplot(coh.mag(k) .* cos(ph), coh.mag(k) .* -sin(ph));
      qplot([coh.mag_lo(k) coh.mag_hi(k)] .* cos(coh.phase(k)), ...
	  [coh.mag_lo(k) coh.mag_hi(k)] .* -sin(coh.phase(k)));
    end
  end
end

%% Draw points and labels
qmarker o solid 4
for k=1:N
  if ~isnan(coh.coh(k))
    qpen(coh.cc(k,:));
    x = coh.mag(k) .* cos(coh.phase(k));
    y = coh.mag(k) * -sin(coh.phase(k));
    % Draw point
    qmark(x, y);
    % Draw label
    qat(x, y);
    qalign left top
    qtext(3, 3, vscope_roiid(k));
  end
end

% Enforce equal x and y scale
qshrink 1 1

