function vscope_plotpolargrid(majorc, minorc)
% VSCOPE_PLOTPOLARGRID - Helper function for VSCOPE_COHPLOTPOLAR

% This file is part of VScope. (C) Daniel Wagenaar 2008-1017.

% VScope is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% VScope is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with VScope.  If not, see <http://www.gnu.org/licenses/>.

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
