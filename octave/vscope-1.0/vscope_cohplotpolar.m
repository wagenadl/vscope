function vscope_cohplotpolar(coh, varargin)
% VSCOPE_COHPLOTPOLAR - Polar plot of coherence results
%    VSCOPE_COHPLOTPOLAR(coh) plots the results of VSCOPE_COHANALYSIS.
%    VSCOPE_COHPLOTPOLAR(coh, key, value, ...) specifies additional 
%    parameters:
%       qpt - filename for QPlot figure (empty for current)
%       width - width of QPlot figure (inch)
%       height - height of QPlot figure
%       nolabels - suppress annotating with ROI IDs

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

kv = getopt('qpt=''/tmp/vscope_coh_polar'' width=5 height=5 nolabels=0', ...
    varargin);
if ~isempty(kv.qpt)
  qfigure(kv.qpt, kv.width, kv.height);
end

vscope_plotpolargrid();

N = length(coh.coh);

% Draw threshold
ph = [0:.01:2*pi];
qpen k 0 dash
if isfield(coh, 'thr')
  qplot(coh.thr*cos(ph), coh.thr*sin(ph));
end

qpen k 0 solid

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
qalign left top
for k=1:N
  if ~isnan(coh.coh(k))
    qpen(coh.cc(k,:));
    x = coh.mag(k) .* cos(coh.phase(k));
    y = coh.mag(k) * -sin(coh.phase(k));
    % Draw point
    qmark(x, y);
    if ~kv.nolabels
      % Draw label
      qat(x, y);
      qtext(3, 3, vscope_roiid(k));
    end
  end
end

% Enforce equal x and y scale
qshrink 1 1

