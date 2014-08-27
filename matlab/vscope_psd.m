function psd = vscope_psd(t_sig, y_sig, varargin)
% VSCOPE_PSD - Multitaper power spectrum estimate for vscope
%    psd = VSCOPE_PSD(t_sig, y_sig) calculates multitaper power spectral
%    density estimates for the signals in the columns of Y_SIG. 
%    psd = VSCOPE_PSD(t_sig, y_sig, key, value, ...) specifies additional
%    parameters:
%       df - frequency resolution (in Hz if t_sig is in seconds). Default
%            is 0.333 Hz.
%    Return value is a structure with fields:
%       f - frequency vector (in Hz if t_sig is in seconds) (Fx1 vector)
%       psd - power estimates for each of the signals (FxN matrix) 
%       fstar - frequency at which P is maximal (only computed if N=1)

kv = getopt('df=1/3', varargin);

[psd.f, psd.psd] = pds_mtm0(t_sig, y_sig, kv.df);

if size(y_sig, 2)==1
  psd.fstar = psd.f(argmax(psd.psd));
end
