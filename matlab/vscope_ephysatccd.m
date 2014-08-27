function zz = vscope_ephysatccd(x, yy, varargin)
% VSCOPE_EPHYSATCCD - Interpolate ephys data at camera frame times
%    zz = VSCOPE_EPHYSATCCD(x, yy) takes the values of the signal YY,
%    which must be measured at the ephys times of the vscope trial X 
%    calculates each average over each of the frames of the first camera.
%    zz = VSCOPE_EPHYSATCCD(x, yy, key, value, ...) specifies additional
%    parameters:
%       camera - camera number
%       func - alternative function to apply to the data within a frame
%              (the default being 'mean')
%    YY may be a Tx1 vector or a TxN matrix, in which case the columns
%    are processed independently.

kv = getopt('camera=[] func=''mean''', varargin);

[t_on, t_off] = vscope_ccdtime(x, kv.camera);
t_e = vscope_ephystime(x);

S = size(yy);
T = length(t_on);
N = prod(S(2:end));
yy = reshape(yy, [S(1) N]);
zz = zeros(T, N);
for t = 1:T
  idx = find(t_e>=t_on(t) & t_e<=t_off(t));
  if isempty(idx)
    z = zeros(1,N) + nan;
  else
    y = yy(idx, :);
    y = reshape(y, [length(idx) S(2:end)]);
    z = feval(kv.func, y);
  end
  zz(t,:) = z(:);
end

zz = reshape(zz, [T S(2:end)]);
