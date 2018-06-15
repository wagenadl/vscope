function x = vscope_load_continuous(x, pre_margin_s, post_margin_s)
% VSCOPE_LOAD_CONTINUOUS - Loads data from continuous ephys
%   x = VSCOPE_LOAD_CONTINUOUS(x), where X is from VSCOPE_LOAD, adds 
%   analog and digital data from continuous ephys recorded during the
%   trial. If there was no continuous ephys going on, an error is raised.
%   This also fills in the CCD.INFO.FRAME_XXX info, so VSCOPE_CCDTIME
%   will work.
%   Because there can be a slight lag between the trial start recorded
%   in the log and the actual start of acquisition, this function loads
%   two seconds of extra data. Under normal circumstances, that should 
%   make sure that all data pertaining to the trial is loaded. Use
%   x = VSCOPE_LOAD_CONTINUOUS(x, premargin_s, postmargin_s) to get more
%   (or less) margin.

if nargin<2
  pre_margin_s = 0;
end
if nargin<3
  post_margin_s = 2;
end

if x.info.contephystrial<=0
  error('No continuous ephys for trial');
end

contx = vscope_load(x.info.contephystrial, 'i'); % Load info only
tridx = find(contx.info.trials.id == x.info.trial);
digi0 = contx.info.trials.dscan(tridx);
ana0 = contx.info.trials.dscan(tridx);
if digi0<=0
  error('No trial start info found in cont.ephys. xml');
end

ifn = x.info.filename;
ifn = strrep(ifn, sprintf('%03i.xml', x.info.trial), ...
              sprintf('%03i.xml', x.info.contephystrial));

N = contx.digital.info.rate_hz ...
    * (x.info.dur_ms/1e3 + pre_margin_s + post_margin_s);
M0 = contx.digital.info.rate_hz*pre_margin_s;
digi0 = digi0 - M0;

x.digital.dat = vscope_load_digital_subset(ifn, ...
                                           contx.digital.info, ...
                                           digi0, N);
x.digital.info = contx.digital.info;

[x.ccd.info.frame_s, x.ccd.info.framestart_s, ...
 x.ccd.info.frameend_s] = vsdl_frametimes(x);
x.info.start_in_cont_s = digi0 / contx.digital.info.rate_hz;
x.info.end_in_cont_s = x.info.start_in_cont_s ...
    + length(x.digital.dat) / contx.digital.info.rate_hz;
x.info.dur_ms = 1e3 * length(x.digital.dat) / contx.digital.info.rate_hz;

if ana0>0
  N = contx.analog.info.rate_hz ...
      * (x.info.dur_ms/1e3 + pre_margin_s + post_margin_s);
  M0 = contx.analog.info.rate_hz*pre_margin_s;
  ana0 = ana0 - M0;
  x.analog.dat = vscope_load_analog_subset(ifn, ...
                                            contx.analog.info, ...
                                            ana0, N);
  x.analog.info = contx.analog.info;
else
  x.analog.dat = [];
  x.analog.info = [];
end

function [t, t0, t1] = vsdl_frametimes(dat)
% Directly copied from VSCOPE_LOAD
t = cell(0,0);
t0 = t;
t1 = t;
if ~isfield(dat, 'ccd')
  return;
end
if ~isfield(dat, 'digital')
  return;
end

ids = dat.ccd.info.camid;
lines = dat.digital.info.lineid;
fhz = dat.digital.info.rate_hz;

C = length(ids);
K = length(lines);
t = cell(C,1);
t0 = cell(C,1);
t1 = cell(C,1);

for c=1:C
  idx = find(strcmp(['Frame:' ids{c}], lines));
  if length(idx)==1
    dd = bitand(dat.digital.dat, uint32(2^(idx-1))) > 0;
    [iup, idn] = schmitt(dd, [], [], 2);
    imid = (iup+idn-1)/2;
    t{c} = (imid-1)/fhz;
    t0{c} = (iup-1)/fhz;
    t1{c} = (idn-1)/fhz;
  end
end
