function coh = vscope_coherence(tt, y_ref, y_sig, varargin)
% VSCOPE_COHERENCE - Multitaper coherence estimate for vscope
%    coh = VSCOPE_COHERENCE(tt, y_ref, y_sig) calculates multitaper coherence
%    estimates for the signals in the columns of Y_SIG wrt the reference
%    signal in Y_REF.
%    coh = VSCOPE_COHERENCE(tt, y_ref, y_sig, key, value, ...) specifies
%    additional parameters:
%       df - frequency resolution (in Hz if tt is in seconds). Default is
%            0.667 Hz.
%       ci - confidence interval in units of sigma. Default is 1. Set ci
%            to ci to 1.96 for 95% confidence interval. Set ci to [] for
%            no confidence interval calculations.
%       f - single frequency at which to evaluate. Default is [] for
%           all frequencies.
%    Return value is a structure with fields:
%       f - frequency vector (in Hz if t_sig is in seconds) (Fx1 vector)
%       coh - coherence estimates for each of the signals (FxN complex matrix)
%       mag - absolute values of above
%       phase - phase of above
%       mag_lo - low end of magnitude confidence interval (iff ci<1)
%       mag_hi - high end of same (ditto)
%       phase_lo - low end of phase confidence interval (ditto)
%       phase_hi - high end of same (ditto)
%   For this function, y_ref must be specified at the same time points as
%   y_sig. See VSCOPE_EPHYSATCCD if your reference signal is naturally
%   defined at other time points.

kv = getopt('df=2/3 ci=1 f=[]', varargin);

if isempty(kv.f) 
  if isempty(kv.ci)
    [coh.f, coh.mag, coh.phase] = coh_mtm0(tt, y_sig, y_ref, kv.df);
  else
    alpha_ci = 2*(1-normcdf(kv.ci));
    [coh.f, coh.mag, coh.phase, ...
      coh.mag_lo, coh.phase_lo, coh.mag_hi, coh.phase_hi] ...
	= coh_mtm0(tt, y_sig, y_ref, kv.df, alpha_ci);
  end
else
  coh.f = kv.f;
  if isempty(kv.ci)
    [coh.mag, coh.phase] = coh_mtm0(tt, y_sig, y_ref, kv.df, 1, kv.f);
  else
    alpha_ci = 2*(1-normcdf(kv.ci));
    [coh.mag, coh.phase, ...
      coh.mag_lo, coh.phase_lo, coh.mag_hi, coh.phase_hi] ...
	= coh_mtm0(tt, y_sig, y_ref, kv.df, alpha_ci, kv.f);
  end
end

coh.coh = coh.mag .* exp(i*coh.phase);
