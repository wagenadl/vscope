function coh = vscope_cohcontrol(tt, y_ref, y_sig, varargin)
% VSCOPE_COHCONTROL - Multitaper coherence estimate control data for vscope
%    coh = VSCOPE_COHCONTROL(tt, y_ref, y_sig) synthesizes control data
%    for coherence analysis for vscope.
%    coh = VSCOPE_COHCONTROL(tt, y_ref, y_sig, key, value, ...) specifies
%    additional parameters:
%       df - frequency resolution. Default is 0.667 Hz.
%       f - frequency at which to evaluate. Default is 1 Hz.
%       p - p-value for calculated thresholds. Default is 0.01 (i.e., 1%).
%    Return value is a structure with fields:
%       thr - threshold values for coherence magnitudes for each signal
%             at the given confidence level (as specified by p-value).

kv = getopt('df=2/3 f=1 p=0.01', varargin);

coh.thr = coh_mtm_control0(tt, y_sig, y_ref, kv.df, kv.f, kv.p);
