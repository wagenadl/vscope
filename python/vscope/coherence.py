#!/usr/bin/python3

import numpy as np
import numbers
from scipy.signal.windows import dpss

from . import rois
from . import debleach
from . import utils

def pds_mtm(t, x, f_res):
    '''PDS_MTM - Multi-taper spectral estimate
    This is DW's adaptation of Adam Taylor's PDS_MTM code
    ff,Pxx = PDS_MTM(tt,xx,fres) calculates one-side multi-taper
    spectrogram.

      TT [T] indicates time points.
      XX [T] is the (optical) data.
      FRES [scalar] is the half-width of the transform of the tapers used;
                    It must be in reciprocal units of those of TT.

      FF [F] is the resulting (one-sided) frequency base.
      Pxx [FxK] are the spectral estimates from each individual taper

    Note that the nature of the beast is that the output Pxx has a 
    full width of 2*FRES even if the signal XX is perfectly sinusoidal.'''

    # From Adam's comments:
    # t is a col vector
    # elements of t are evenly spaced and increasing
    # x is a real matrix with the same number of rows as t
    # f_res is the half-width of the transform of the tapers used
    #   it must be in reciprocal units of those of t
    # N_fft is the length to which data is zero-padded before FFTing
    # this works on the columns of x independently
    #
    # f is the frequncy base, which is one-sided
    # Pxx's cols are the the one-sided spectral estimates of the cols of x
    # Pxxs is 3D, (frequency samples)x(cols of x)x(tapers), gives the spectrum
    #   estimate for each taper
    #
    # we assume that x is real, and return the one-side periodogram
    
    tapers = None
    N_fft = 2**np.ceil(np.log2(len(t)))

    N = len(t)  # number of time samples
    dt  =(t[N]-t[1])/(N-1)
    fs=1/dt

    # compute nw and K
    nw = N*dt*f_res
    K = np.floor(2*nw-1)

    tapers = dpss(N,nw,K)
    tapers = np.reshape(tapers, [N, 1, K])

% zero-pad, taper, and do the FFT
x_tapered=repmat(x,[1 1 K]).*repmat(tapers,[1 N_signals 1]);
X=fft(x_tapered,N_fft);

% convert to PDSs by squaring and normalizing appropriately
Pxxs=(abs(X).^2)/fs;

% fold the positive and negative frequencies together
[Pxxs,f]=sum_pos_neg_freqs(Pxxs);
f=fs*f;

% average all the spectral estimates together
Pxx=mean(Pxxs,3);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Pxxs_os,f_os] = sum_pos_neg_freqs(Pxxs_ts)

% turns a two-sided PSD into a one-sided
% works on the the cols of Pxx_ts
% doesn't work for ndims>3

% get the dims of Pxx_ts
[N,N_signals,K]=size(Pxxs_ts);

% fold the positive and negative frequencies together
% hpfi = 'highest positive frequency index'
% also, generate frequency base
hpfi=ceil(N/2);
if mod(N,2)==0  % if N_fft is even
  Pxxs_os=[Pxxs_ts(1:hpfi,:,:) ; zeros(1,N_signals,K) ]+...
          [zeros(1,N_signals,K) ; flipdim(Pxxs_ts(hpfi+1:N,:,:),1) ];
  f_os=(0:hpfi)'/N;
else
  Pxxs_os=Pxxs_ts(1:hpfi,:,:)+...
          [zeros(1,N_signals,K) ; flipdim(Pxxs_ts(hpfi+1:N_fft,:,:),1)];
  f_os=(0:(hpfi-1))'/N;
end
    

def psd(tt, yy, df=1/3):
    '''PSD - Multitaper power spectrum estimate for vscope
    f, p = PSD(t_sig, y_sig, df) calculates multitaper power
    spectral density estimates for the signal YY (a.u.) sampled at 
    times TT (s), with a final frequency resolution of DF (Hz). 
    Returns:
      - F: frequency vector (in Hz if t_sig is in seconds)
      - P: power estimates at each frequency'''    
   

def globaltrend(x, cam, outsiderois=False):
    '''GLOBALTREND - Extract global trend from VSD data
    y = GLOBALTREND(x, cam) returns the global trend of the VSD image 
    sequence in the vscope data X (which must be a VScopeFile from LOAD)
    for the given camera.

    Optional arguments:
      - OUTSIDEROIS: Set to True to calculate the trend based only on
                     those areas of the VSD image sequence that are
                     not in any ROIs.

    Returns: 
      - Y: vector of global trend data
    '''
    dat = x.ccd.data[cam]
    T, Y, X = dat.shape
    KY = Y//32
    KX = X//32
    if outsiderois:
        msk = np.logical_not(rois.allroimask(x, cam))
    else:
        msk = np.ones((Y,X), bool)
    msk[:KY,:] = False
    msk[Y-KY:,:] = False
    msk[:,:KX] = False
    msk[:,X-KX:] = False
    msk = np.nonzero(msk)
    res = np.zeros(T)
    yy = np.zeros(T)
    for t in range(T):
        frm = dat[t,:,:]
        yy[t] = np.mean(frm[msk])
    return yy

class Reference:
    def __init__(self):
        self.typ = None
    def instantiate(self, x, cam):
        tt = utils.ccdtime(x, cam)
        return np.zeros(len(tt))

class SineRef(Reference):
    def __init__(self, freq_Hz, t0=0):
        '''Construct a sinusoidal reference signal at given frequency.
        By default, phase is aligned with t=0 in the source signal;
        optional argument T0 changes that.'''
        self.typ = 'sine'
        self.freq_Hz = freq_Hz
        self.t0 = t0
    def instantiate(self, x, cam):
        tt = utils.ccdtime(x, cam)
        return np.sin(2*np.pi*(tt-self.t0)*self.freq_Hz)

class AnalogRef(Reference):
    def __init__(self, channel, func=np.mean):
        '''Construct a reference based on an analog input. 
        CHANNEL may be either the number or the name of an analog input
        channel.
        By default, analog samples are averaged across the duration of
        a camera frame using NP.MEAN. Optional argument FUNC specifies
        a function or lambda to change that.'''
        self.typ = 'analog'
        self.channel = channel
        self.func = func
    def instantiate(self, x, cam):
        if isinstance(self.channel, numbers.Number):
            idx = self.channel
        else:
            idx = x.analog.cids.index(self.channel)
        return utils.ephysatccd(x, cam, x.analog.data[:,idx], self.func)

class OpticalRef(Reference):
    def __init__(self, roi):
        '''Construct a reference based on an ROI in the optical data.
        ROI must be the name of an ROI that is visible on the camera
        for which you will be calculating the coherence.'''
        self.typ = 'optical'
        self.roi = roi
    def instantiate(self, x, cam):
        return rois.extract(x, self.roi, cam)
class DirectRef(Reference):
    def __init__(self, data):
        '''Construct a reference based on direct data. 
        The DATA must have the same number of time points as the
        total number of optical frames in the recording.'''
        self.typ = 'direct'
        self.data = data
    def instantiate(self, x, cam):
        return self.data
        

def cohanalysis(x, cam, ref,
                df_psd=1/3, df_coh=2/3,
                f_star=None,
                ci=1, pthresh=0.01,
                t0=None, t1=None,
                sig=None,
                debleach=None,
                trend=None):
    '''ANALYSIS - Perform coherence analysis on VSD data
    res = ANALYSIS(x, cam, ref) performs coherence analysis on a VScope
    trial. 
    X must be a VScopeFile from LOAD.
    CAM must be a camera ID.
    REF must be a SineRef, an AnalogRef, an OpticalRef, or a DirectRef.

    Optional arguments:
      - DF_PSD: frequency resolution for power spectral density. Default
                is 0.333 Hz.
      - DF_COH: frequency resolution for coherence. Default is 0.667 Hz.
      - F_STAR: override frequency at which to measure coherence. (Default
                is to find peak in reference signal; ignored if comparing
                to sine wave.)
      - CI: confidence interval (units of sigma) for coherence
      - PTHRESH: threshold for significance. Use None to omit calculations.
      - T0: start time of analysis. Default to 3rd frame of optical trace.
      - T1: end time of analysis. Default to penultimate frame of optical
            trace.
      - SIG: override what signals to use. Default is extracted ROI data.
      - DEBLEACH: A subclass of Debleach, i.e., SalpaDebleach, PolyDebleach,
                  or ExpDebleach. Debleaching is never done on override signals.
      - TREND - trend data to subtract, e.g., from GLOBALTREND.

    Returns: RES: a dict with members:
      - F: frequency at which results where computed (Hz)
      - PSD: power spectrial densities of all signals at that frequency
      - COH: complex coherence of each signal (dict with ROI numbers as keys)
      - MAG: coherence magnitudes (dict)
      - PHASE: coherence phases (-π to +π; dict)
      - MAG_LO and MAG_HI: confidence limits on MAG (dicts)
      - PHASE_LO and PHASE_HI: confidence limits on PHASE (dicts)
      - THR: threshold for significance of MAG
      - CC: color map for plotting (signals with MAG<THR will be gray; dict)
      - EXTRA: dict containing more information
          - FF: frequency vector (Hz)
          - PSD: power spectral densities of all signals at all freqs
                 (dict of channel IDs to vectors)
          - REFPSD: PSDs of reference at all frequencies
          - TT: time vector
          - SIG: detrended and debleached signals at times TT
                 (dict of channel IDs to vectors)
          - REF: detrended reference at times TT
          - TT0: time vector before clipping to T0...T1 (see below)
          - SIG0: debleached but not detrended signals at times TT0
                  (dict of channel IDs to vectors)
          - REF0: not detrended signals at times TT0
          - IMG: raw data for first frame within the time window
          - ROIS: original ROI coordinates, transformed into space of IMG

    Since we usually do multiple comparisons, either pthresh should be 
    chosen conservatively, or, more cleverly, put in pthresh=-0.05 (or -p 
    in general), and we will automatically find the highest value of N 
    such that there are  N signals significant at |pthresh|/N. In this
    case, RES['pthr'] will end up being the p-value ultimately used.'''

    t_on, t_off, ok = vscope.ccdtime(x, cam)

    res = { 'extra': { } }

    res['rois'] = {}
    for k, roi in x.rois.items():
        if cam in roi['cams']:
            res['extra']['here'].add(k)
            res['rois'][k] = rois.outline(x, k, cam)

    ccd = x.ccd.data[cam]
    T, Y, X = ccd.shape

    if t0 is None:
        res['extra']['img'] = ccd[max(2,T), :, :]
        skip0 = 2
    else:
        idx = np.nonzero(t_on >= t0)
        res['extra']['img'] = ccd[idx[0], :, :]
        skip0 = np.sum(t_off<t0)
    if t1 is None:
        skip1 = 1
    else:
        skip1 = np.sum(t_on>t1)
        
    if sig is None:
        sig = rois.extractall(x, cam)
        if trend is not None:
            # Octave version does the equivalent of:
            trend = trend - 1000
            trend /= np.mean(trend)
            # I do not know why
            for k in sig:
                sig[k] = sig[k] / trend
        if debleach is not None:
            sig = debleach.apply(sig, skip0, skip1)

    idx = np.arange(skip0, T - skip1, int)
    tt0 = (t_on + t_off)/2
    tt = tt0[idx]
    y_sig = {}
    dblch = debleach.PolyDebleach(1)
    for k, v in sig.items():
        y_sig[k] = dblch.apply(v[idx])
    y_ref = dblch.apply(ref[idx])
    res['extra']['tt'] = tt
    res['extra']['sig'] = y_sig
    res['extra']['ref'] = y_ref
    res['extra']['tt0'] = tt0
    res['extra']['sig0'] = sig
    res['extra']['ref'] = ref

    print('Calculating PSDs')
    refpsd = psd(tt, y_ref, df=df_psd)
    if f_star is None:
        f_star = refpsd.fstar
