function [vv, tt] = vscope_reconstim(settings, ch)
% VSCOPE_RECONSTIM - Reconstruct stimulus waveform
%   vv = VSCOPE_RECONSTIM(settings, channel) reconstructs the stimulus delivered
%   to the named CHANNEL according to the given SETTINGS, which must be either
%   the "settings" field of the result of VSCOPE_LOAD.
%   [vv, tt] = VSCOPE_RECONSTIM(...) also returns a vector of time stamps.
%   VSCOPE_RECONSTIM also accepts the result of VSCOPE_LOAD directly.
%   VSCOPE_RECONSTIM does not need the actual data from a trial, so the 
%   following example works and avoid loading unnecessary data:
%     x = vscope_load('/some/expt/trial.xml', '-');
%     [vv, tt] = vscope_reconstim(x, 'AO0');

if isfield(settings, 'settings')
  settings = settings.settings;
end

f_Hz = units(settings.acqEphys.acqFreq, 'Hz');
T_s =  units(settings.acqEphys.acqTime, 's');
stm = settings.stimEphys.channel.(ch);

enable = strcmp(stm.enable, 'true');
delay_s = units(stm.delay, 's');
nPulses = units(stm.nPulses, '');
nTrains = units(stm.nTrains, '');
pulseAmp_V = units(stm.pulseAmp, 'V');
pulseAmp2_V = units(stm.pulseAmp2, 'V');
pulseDur_s = units(stm.pulseDur, 's');
pulseDur2_s = units(stm.pulseDur, 's');
pulsePeriod_s = units(stm.pulsePeriod, 's');
typ = stm.pulseType;
trainPeriod_s = units(stm.trainPeriod, 's');

N = round(f_Hz * T_s);

vv = zeros(N,1);
if nargout>1
  tt = [0:N-1]' / f_Hz;
end

if ~enable
  return
end

pulsedur = round(f_Hz * pulseDur_s);
pulsedur2 = round(f_Hz * pulseDur2_s);

for tr=1:nTrains
  for pu=1:nPulses
    t0_s = (pu-1) * pulsePeriod_s + (tr-1) * trainPeriod_s + delay_s;
    idx0 = round(f_Hz * t0_s);
    switch typ
      case 'Square'
        idx1 = min(N, idx0 + pulsedur);
        vv(idx0:idx1) = pulseAmp_V;
      case 'Biphasic'
        idx1 = min(N, idx0 + pulsedur);
        vv(idx0+1:idx1) = pulseAmp_V;
        idx2 = min(N, idx1 + pulsedur2);
        vv(idx1+1:idx2) = pulseAmp2_V;
      otherwise
        error(sprintf('Stimulus type %s not yet supported', typ));
    end
  end
end
