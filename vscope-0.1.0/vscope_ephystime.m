function tt = vscope_ephystime(x)
% VSCOPE_EPHYSTIME - Construct a vector of time points for ephys data
%    tt = VSCOPE_EPHYSTIME(x) where X is the result of VSCOPE_LOAD returns
%    a vector of times (in seconds) corresponding to the samples in the
%    analog data contained in X.analog.dat
%    For convenience, X may also be the analog substructure itself.

if isfield(x,'analog')
  tt = [0:size(x.analog.dat,1)-1]'/x.analog.info.rate_hz;
else
  tt = [0:size(x.dat,1)-1]'/x.info.rate_hz;
end
