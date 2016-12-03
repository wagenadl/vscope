function cc = vscope_cohcolor(mag, phase)
% VSCOPE_COHCOLOR - Color values for coherence analysis
%    cc = VSCOPE_COHCOLOR(mag, phase) returns a palette of colors to be
%    used to represent coherence values specified by magnitude and phase.
%    cc = VSCOPE_COHCOLOR(coh) does the same for complex coherence values.
%    Call without arguments for a test picture

if nargin==0
  [xx, yy] = xxyy(400);
  xx = 2*xx - 1;
  yy = 2*yy - 1;
  mag = sqrt(xx.^2 + yy.^2);
  pha = atan2(yy, xx);
  cc = vscope_cohcolor(mag(:), pha(:));
  cc(mag(:)>1, :) = 1;
  cc = reshape(cc, [400 400 3]);
  qfigure('/tmp/vscope_cohcolor', 5, 5);
  qimage(cc);
  qshrink 1 1
  
  clear cc
  return;
end

if nargin<2
  phase = angle(mag);
  mag = abs(mag);
end

N = length(mag);

ph = mod(phase(:) + .34*pi, 2*pi);
mg = mag(:);
mg(mg<0) = 0;
mg(mg>1) = 1;

cc = colorconvert(dhsv(360), 'from', 'srgb', 'to', 'linearrgb');

cc = interp1([0:359]/359*360, cc, ph*180/pi);
a = repmat(mg.^1.5, [1 3]);
b = 1 - a;
cc = a.*cc + b.*.6;
cc(isnan(ph) | isnan(mg), :) = 0;

cc = colorconvert(cc, 'from', 'linearrgb', 'to', 'srgb', 'clip', 1);
