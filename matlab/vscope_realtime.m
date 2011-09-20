function [tstart, tend] = vscope_realtime(x)
% VSCOPE_REALTIME - Time of start and end of trial of vscope data
%    [tstart, tend] = VSCOPE_REALTIME(x), where X is the result of
%    VSCOPE_LOAD, returns the start and end times of the trial X
%    as Matlab time vectors (see CLOCK).

tstart = vsc_rt(x.info.starts);
if nargout>=2
  tend = vsc_rt(x.info.ends);
end	

function tv = vsc_rt(s)
tv = [ atoi(s(1:2))+2000 atoi(s(3:4)) atoi(s(5:6)) ...
	atoi(s(8:9)) atoi(s(10:11)) atoi(s(12:13)) ];

