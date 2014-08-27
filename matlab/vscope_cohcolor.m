function cc = vscope_cohcolor(mag, phase)
% VSCOPE_COHCOLOR - Color values for coherence analysis
%    cc = VSCOPE_COHCOLOR(mag, phase) returns a palette of colors to be
%    used to represent coherence values specified by magnitude and phase.
%    cc = VSCOPE_COHCOLOR(coh) does the same for complex coherence values.

if nargin<2
  phase = angle(mag);
  mag = abs(mag);
end

N = length(mag);

ph = mod(phase(:) / (2*pi), 1);
mg = mag(:);
mg(mg<0) = 0;
mg(mg>1) = 1;

cc = ones(N, 1, 3);
cc(:, 1, 1) = ph;
cc(:, 1, 2) = mg;
cc(isnan(ph) | isnan(mg), 1, :) = 0;

cc = squeeze(colorspace('rgb<-hsv', cc));
