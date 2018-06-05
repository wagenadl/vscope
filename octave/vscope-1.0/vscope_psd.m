function psd = vscope_psd(t_sig, y_sig, varargin)
% VSCOPE_PSD - Multitaper power spectrum estimate for vscope
%    psd = VSCOPE_PSD(t_sig, y_sig) calculates multitaper power
%    spectral density estimates for the signals in the columns of Y_SIG. 
%    psd = VSCOPE_PSD(t_sig, y_sig, key, value, ...) specifies
%    additional parameters:
%       df - frequency resolution (in Hz if t_sig is in seconds). Default
%            is 0.333 Hz.
%    Return value is a structure with fields:
%       f - frequency vector (in Hz if t_sig is in seconds) (Fx1 vector)
%       psd - power estimates for each of the signals (FxN matrix) 
%       sd - uncertainty on those estimates (FxN matrix)
%       estimates - raw estimates on which PSD and SD are based
%       fstar - frequency at which P is maximal (only computed if N=1)

% This file is part of VScope. (C) Daniel Wagenaar 2008-1018.

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
%
% This code was adapted from an original by Adam Taylor, whose
% contribution is gratefully acknowledged.

kv = getopt('df=1/3', varargin);

if numel(t_sig) ~= length(t_sig)
    error('T_SIG must be a vector');
end
if numel(y_sig) == length(y_sig)
    % Y_SIG is a vector, so let's relax about 1xT vs Tx1
    y_sig = y_sig(:);
end
t_sig = t_sig(:); % Force T_SIG to be Tx1

[psd.f, psd.psd, psd.estimates] = pds_mtm0(t_sig, y_sig, kv.df);
psd.sd = std(psd.estimates,[],3) ./ sqrt(size(psd.estimates,3));
psd.estimates = squeeze(psd.estimates);

if size(y_sig, 2)==1
  [~, idx] = max(psd.psd);
  psd.fstar = psd.f(idx);
end
