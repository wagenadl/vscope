function coh = vscope_cohanalysis(x, varargin)
% VSCOPE_COHANALYSIS - Comprehensive coherence analysis for vscope
%    coh = VSCOPE_COHANALYSIS(x) performs coherence analysis on a vscope 
%    trial. X must be from VSCOPE_LOAD.
%    coh = VSCOPE_COHANALYSIS(x, key, value, ...) specifies additional
%    parameters:
%
%       camera - camera name or number. Defaults to first active camera.
%       frequency - use sine wave with given frequency in Hz as reference.
%       analog - use named or numbered analog channel as reference.
%       direct - use directly specified reference (value must be a vector
%                defined at the "ccd times" of the trial).
%       df_psd - frequency resolution for power spectral density. Default
%                is 0.333 Hz.
%       df_coh - frequency resolution for coherence. Default is 0.667 Hz.
%       f_star - override frequency at which to measure coherence. (Default
%                is to find peak in reference signal; ignored if comparing
%                to sine wave.)
%       ci - confidence interval (units of sigma) for coherence
%       pthresh - threshold for significance. Use [] to omit calculations.
%       t0 - start time of analysis. Default to 3rd frame of optical trace.
%       t1 - end time of analysis. Default to penultimate frame of optical
%            trace.
%       sig - override what signals to use. Default is extracted ROI data.
%       debleach - order VSCOPE_DEBLEACH. Default is 2. Use [] for no 
%                  debleaching. Debleaching is never done on override signals.
%       func - function to apply to the named analog channel to average
%              its values for each frame. Default is "mean".
%
%    Return value is a structure with members:
%
%       f - frequency at which results were computed
%       psd - power spectral densities of all signals at that frequency
%       coh - complex coherence
%       mag - coherence magnitudes (ditto)
%       phase - coherence phases (ditto)
%       mag_lo and mag_hi - low and high end of confidence intervals for mag
%       phase_lo and phase_hi - ditto for phase
%       thr - threshold for significance
%       cc - color map (signals with mag<thr will be gray)
%       extra.f - frequency vector
%       extra.psd - power spectral densities of all signals at all freqs
%       extra.refpsd - psds of reference at all frequencies
%       extra.tt - time vector
%       extra.sig - detrended debleached signals at those times
%       extra.ref - detrended reference at those times
%       extra.tt0 - time vector before clipping to t0...t1
%       extra.sig0 - undetrended debleached signals at those times
%       extra.ref0 - undetrended reference at those times
%       extra.img - raw data for first frame within the time window
%       extra.xx and extra.yy - transformed coordinates for that image. (Use
%                               these if you want to overlay the image with
%                               VSCOPE_ROIOUTLINE results.)

kv = getopt([ 'camera=[] frequency=[] analog=[] direct=[] ' ...
      'df_psd=1/3 df_coh=2/3 f_star=[] ci=1 pthresh=0.01 ' ...
      't0=[] t1=[] sig=[] debleach=2 func=''mean'''], varargin);

% Determine camera ID and frame times
if isempty(kv.camera)
  for n=1:x.ccd.info.ncams
    if x.ccd.info.nframes{n}>0
      kv.camera = n;
      break;
    end
  end
  if isempty(kv.camera)
    error('No camera with frames');
  end
end
[t_on, t_off] = vscope_ccdtime(x, kv.camera);

% Extract reference image
[Y X C T] = size(x.ccd.dat);
if isempty(kv.t0)
  coh.extra.img = x.ccd.dat(:, :, kv.camera, max(3, T));
else
  coh.extra.img = x.ccd.dat(:, :, kv.camera, find(t_on>=kv.t0, 1));
end
xform = x.ccd.info.xform{kv.camera};
coh.extra.xx = [0:X-1]*xform.ax + xform.bx; % Should that be [0.5:X] ?
coh.extra.yy = [0:Y-1]*xform.ay + xform.by; % (ditto)

% Extract signals from ROIs unless prespecified
if isempty(kv.sig)
  sig = vscope_extractrois(x);
  sig = sig(:, :, kv.camera);
  if ~isempty(kv.debleach)
    if isempty(kv.t0) || isempty(kv.t1)
      sig = vscope_debleach(sig, kv.debleach, 2, 1);
    else
      sig = vscope_debleach(sig, kv.debleach, ...
	  sum(t_off<kv.t0), sum(t_on>kv.t1));
    end
  end
end

% Determine reference signal
if ~isempty(kv.direct)
  ref = detrend(kv.direct);
elseif ~isempty(kv.analog)
  if ischar(kv.analog)
    kv.analog = find(strcmp(kv.analog, x.analog.info.chanid));
    if length(kv.analog)~=1
      error('Cannot match analog channel name');
    end
  end
  ref = x.analog.dat(:, kv.analog);
  ref = vscope_ephysatccd(x, ref, 'func', kv.func);
else 
  if isempty(kv.frequency)
    kv.frequency = 1;
  end
  ref = sin(2*pi*kv.frequency*(t_on+t_off)/2);
  kv.f_star = kv.frequency;
end

% Clip signals to specified time window and store results
if isempty(kv.t0) || isempty(kv.t1)
  idx = [3:length(t_on)-1];
else
  idx = find(t_on>=kv.t0 & t_off<=kv.t1);
end
tt = (t_on(idx) + t_off(idx)) / 2;
y_sig = detrend(sig(idx, :));
y_ref = detrend(ref(idx, :));
coh.extra.tt = tt;
coh.extra.sig = y_sig;
coh.extra.ref = y_ref;
coh.extra.tt0 = (t_on + t_off) / 2;
coh.extra.sig0 = sig;
coh.extra.ref0 = ref;

% Calculate power spectrum of reference and signal and store results
refpsd = vscope_psd(tt, y_ref, 'df', kv.df_psd);
if isempty(kv.f_star)
  kv.f_star = refpsd.fstar;
end

coh.f = kv.f_star;
coh.extra.f = refpsd.f;
coh.extra.refpsd = refpsd.psd;

sigpsd = vscope_psd(tt, y_sig, 'df', kv.df_psd);
coh.extra.psd = sigpsd.psd;
coh.psd = interp1(sigpsd.f, sigpsd.psd, coh.f, 'linear');

% Calculate coherence (at peak frequency) and store results
cohe = vscope_coherence(tt, y_ref, y_sig, ...
    'df', kv.df_coh, 'ci', kv.ci, 'f', kv.f_star);
coh.coh = cohe.coh;
coh.mag = cohe.mag;
coh.phase = cohe.phase;
if isfield(cohe, 'mag_lo')
  coh.mag_lo = cohe.mag_lo;
  coh.mag_hi = cohe.mag_hi;
  coh.phase_lo = cohe.phase_lo;
  coh.phase_hi = cohe.phase_hi;
end

% Calculate threshold for significance
if ~isempty(kv.pthresh)
  cohc = vscope_cohcontrol(tt, y_ref, y_sig, ...
      'df', kv.df_coh, 'p', kv.pthresh, 'f', kv.f_star);
  coh.thr = cohc.thr;
end

% Calculate colors for coherence representation
coh.cc = vscope_cohcolor(coh.mag, coh.phase);
if isfield(coh, 'thr')
  coh.cc(coh.mag<coh.thr, :) = 0.75;
end
