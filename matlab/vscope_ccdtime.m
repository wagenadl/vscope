function tt = vscope_ccdtime(x, id)
% VSCOPE_CCDTIME - Construct a vector of time points for CCD data
%    tt = VSCOPE_CCDTIME(x, id), where X is the result of VSCOPE_LOAD,
%    and ID is the name of a camera contained therein, returns
%    a vector of times (in seconds) corresponding to the frames in the
%    ccd data for that camera.
%    If ID is not given, the first camera is used.
%    For convenience, X may also be the ccd substructure itself.
%    IN THE CURRENT VERSION, ID IS IGNORED, BECAUSE VSCOPE_LOAD DOESN'T YET
%    KNOW ABOUT MULTIPLE CAMERAS WITH DIFFERENT TIMINGS.

if isfield(x,'ccd')
  t0 = x.ccd.info.delay_ms{1}/1e3;
  f = x.ccd.info.rate_hz{1};
  n = x.ccd.info.nframes{1};
else
  t0 = x.info.delay_ms{1}/1e3;
  f = x.info.rate_hz{1};
  n = x.info.nframes{1};
end

tt = [0:n-1]'/f + t0;
