function vscope_plotpolargrid(majorc, minorc)
% VSCOPE_PLOTPOLARGRID - Helper function for VSCOPE_COHPLOTPOLAR

if nargin<2
  majorc = 'k';
  minorc = '777';
end

%% Draw grid
% Draw cross
qpen(minorc, 0, 'solid');
qplot([-1 1], [0 0]);
qplot([0 0], [-1 1]);

% Draw minor circles
ph = [0:.01:2*pi];
for r = [0.25 .5 .75]
  qplot(r*cos(ph), r*sin(ph));
end

% Draw outer circle
qpen(majorc);
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
