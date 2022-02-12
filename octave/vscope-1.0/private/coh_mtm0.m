function varargout = coh_mtm0(t,x,y,f_res,alpha_ci,f_star,N_fft,tapers)
% COH_MTM0 - Multi-taper coherence estimate
%    This is DW's adaptation of Adam Taylor's COH_MTM code
%    [ff, mag, phase] = COH_MTM0(tt, xx, yy, fres)
%    calculates the coherence of the signals XX (TxN) wrt the 
%    signals YY (Tx1 or TxN), defined at times TT (Tx1) (evenly spaced
%    and increasing).
%    The coherence is calculated with frequency resolution FRES (which 
%    must be in reciprocal units of those of TT). Results are:
%
%      FF (Fx1): frequency base (one-sided).
%      MAG (FxN): magnitude of coherence (normalized)
%      PHASE (FxN): phase of coherence (-pi..pi). Phase is positive
%                   if YY lags XX. (e.g. if XX=sin(TT), and YY=sin(TT-0.1),
%                   phase at the peak will be +0.1.)
%
%    [ff, mag, phase, mag0, ph0, mag1, ph1] 
%       = COH_MTM0(tt, xx, yy, fres, A_ci) additionally calculates
%    confidence intervals; set P_CI = 0.317 for one-sigma error bars.
%    (In general, set P_CI = 2-2*normcdf(K) for K*sigma error bars, 
%    or P_CI = p for an interval with confidence 1-p; e.g. P_CI=0.05 for
%    95% confidence and 2.5% on either extreme.) 
%
%    Extra results are:
%
%      MAG0, MAG1 (FxN): lower and upper confidence bounds for MAG.
%      PH0, PH1 (FxN): confidence bounds for PHASE.
%
%    COH_MTM0(..., f_star, PadFFT, tapers) specifies additional
%    parameters:
%
%      F_STAR: calculate only at the frequency F_STAR.
%      PADFFT: length to which data is padded before fourier transform
%      TAPERS: supply pre-calculated tapers directly.
%
%    If F_STAR is specified, output is [mag, phase, mag0, mag1, ph0, ph1],
%    that is, FF is not returned.
%
%    If nargout = 1, results are returned in a struct instead.
%    This struct additionally contains the K individual taper estimates
%    Pxx, Pyy, and Pxy for the self and cross spectra.

% This file is part of VScope. (C) Daniel Wagenaar 2008-1017.

% VScope is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% VScope is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with VScope.  If not, see <http://www.gnu.org/licenses/>.


t=t(:);
[T N]=size(x);
if T==1 && N>1
  if prod(size(y))==length(y)
    x=x(:);
    y=y(:);
    [T N]=size(x);
  end
end
if size(y,2)==1
  y=repmat(y,[1 N]);
end

% t must be a col vector
% elements of t must be evenly spaced and increasing
% x must be a matrix with the same number of rows as t
% y must be a matrix with the same number of rows as t
% f_res is the half-width of the transform of the tapers used
%   it must be in reciprocal units of those of t
% N_fft is the length to which data is zero-padded before FFTing
% if f_star is given, coherence is estimated only at f=f_star
% this works on the columns of x and y independently
%
% f is the frequncy base, which is one-sided
% the varargouts are the sigmas

% process args
if nargin<5
  alpha_ci=1;
elseif isempty(alpha_ci)
  alpha_ci=1;
end
if nargin<6
  multiple_f=1;
elseif isempty(f_star)
  multiple_f=1;
else
  multiple_f=0;
end
if nargin<7
  N_fft=2^ceil(log2(length(t)));
elseif isempty(N_fft)
  N_fft=2^ceil(log2(length(t)));
end
if nargin<8
  tapers=[];
end

% get the timing info, calc various scalars of interest
N=size(x,1);
N_signals=size(x,2);
dt=(t(N)-t(1))/(N-1);
fs=1/dt;

% compute nw and K
nw=N*dt*f_res;
K=floor(2*nw-1);

fprintf(1,'coh_mtm0: dt=%g T=%g df=%g nw=%g K=%i\n',...
    mean(diff(t)),t(end)-t(1)+mean(diff(t)),f_res,nw,K);

tapers=dpss(N,nw,K);

tapers=reshape(tapers,[N 1 K]);

% taper and do the FFTs
x_tapered=repmat(x,[1 1 K]).*repmat(tapers,[1 N_signals 1]);
y_tapered=repmat(y,[1 1 K]).*repmat(tapers,[1 N_signals 1]);
if multiple_f
  X=fft(x_tapered,N_fft);
  Y=fft(y_tapered,N_fft);
  % drop the negative frequencies
  % all computations from here on are 'frquency-wise', do this does no 
  % harm, and saves time
  X=drop_neg_freqs(X);
  Y=drop_neg_freqs(Y);
else
  phi_star=f_star/fs;
  k=(0:(N-1))';
  w=repmat(exp(-i*2*pi*phi_star*k),[1 N_signals K]);
  X=sum(x_tapered.*w,1);
  Y=sum(y_tapered.*w,1);
end

% generate the frequency base
% hpfi = 'highest positive frequency index'
if multiple_f
  hpfi=ceil(N_fft/2);
  f=fs*(0:(hpfi-1))'/N_fft;
else
  f=f_star;
end

% convert to PSDs
Pxxs=(abs(X).^2)/fs;
Pyys=(abs(Y).^2)/fs;
Pxys=(X.*conj(Y))/fs;


% _sum_ across tapers (keep these around in case we need to calculate
% the take-away-one spectra for error bars)
PxxK=sum(Pxxs,3);
PyyK=sum(Pyys,3);
PxyK=sum(Pxys,3);

% convert the sum across tapers to an averge; these are our 'overall'
% spectral estimates
Pxx=PxxK/K;
Pyy=PyyK/K;
Pxy=PxyK/K;


% calculate coherence
Cxy=Pxy./sqrt(Pxx.*Pyy+1e-50);
Cxy_mag=abs(Cxy);
Cxy_phase=angle(Cxy);

if nargout==1
  res = struct();
  res.f = f;
  res.coh = Cxy;
  res.mag = Cxy_mag;
  res.phase = Cxy_phase;
  res.Pxx = Pxxs;
  res.Pyy = Pyys;
  res.Pxy = Pxys;
end

% calc the sigmas
if alpha_ci<1
  % calculate the transformed coherence
  Cxy_mag_xf=algcsqr(Cxy_mag);
  
  % calculate the take-away-one spectra
  Pxxs_tao=(repmat(PxxK,[1 1 K])-Pxxs)/(K-1);
  Pyys_tao=(repmat(PyyK,[1 1 K])-Pyys)/(K-1);
  Pxys_tao=(repmat(PxyK,[1 1 K])-Pxys)/(K-1);

  % calculate the take-away-one coherence
  Cxy_tao=Pxys_tao./sqrt(Pxxs_tao.*Pyys_tao+1e-50);
  Cxy_tao_mag=abs(Cxy_tao);
  Cxy_tao_mag_xf=algcsqr(Cxy_tao_mag);
  Cxy_tao_phase=angle(Cxy_tao);

  % calculate the magnitude sigmas
  Cxy_tao_mag_xf_mean=mean(Cxy_tao_mag_xf,3);
  Cxy_mag_xf_sigma=sqrt((K-1)/K * ...
      sum((Cxy_tao_mag_xf-repmat(Cxy_tao_mag_xf_mean,[1 1 K])).^2,3));  
  
  % calculate the phase sigma
  Cxy_tao_hat=Cxy_tao./(Cxy_tao_mag+1e-50);
  Cxy_tao_hat_mean=mean(Cxy_tao_hat,3);
  Cxy_phase_sigma=sqrt(2*(K-1)*(1-abs(Cxy_tao_hat_mean)));

  % calculate the confidence interval bounds
  ci_factor=norminv(1-alpha_ci/2);
  Cxy_mag_ci_lo=sqrtlgc(Cxy_mag_xf-ci_factor*Cxy_mag_xf_sigma);
  Cxy_mag_ci_hi=sqrtlgc(Cxy_mag_xf+ci_factor*Cxy_mag_xf_sigma);
  Cxy_phase_ci_lo=Cxy_phase-ci_factor*Cxy_phase_sigma;
  Cxy_phase_ci_hi=Cxy_phase+ci_factor*Cxy_phase_sigma;

  % assign the return values
  if nargout==1
    res.mag_lo = Cxy_mag_ci_lo;
    res.mag_hi = Cxy_mag_ci_hi;
    res.phase_lo = Cxy_phase_ci_lo;
    res.phase_hi = Cxy_phase_ci_hi;
    varargout = { res };
  else
    if multiple_f
      varargout = { f Cxy_mag Cxy_phase ...
	  Cxy_mag_ci_lo Cxy_phase_ci_lo ...
	  Cxy_mag_ci_hi Cxy_phase_ci_hi };
    else
      varargout = { Cxy_mag Cxy_phase ...
	  Cxy_mag_ci_lo Cxy_phase_ci_lo ...
	  Cxy_mag_ci_hi Cxy_phase_ci_hi };
    end
  end
else
  % assign the return values
  if nargout==1
    varargout = { res };
  else
    if multiple_f
      varargout = {f Cxy_mag Cxy_phase };
    else
      varargout = {Cxy_mag Cxy_phase };
    end
  end
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Cxy_os] = drop_neg_freqs(Cxy_ts)

% turns a two-sided coherence into a one-sided
% (i.e. it drops the negative frequencies)
% works on the the cols of Cxy_ts (i.e. along the first dimension)

% huge rigamarole to do something MATLAB should have a way to do easily
N_dims=ndims(Cxy_ts);
rest_o_dims=cell(1,N_dims-1);
for j=1:N_dims-1
  rest_o_dims{j}=':';
end
% end of huge rigamarole

N=size(Cxy_ts,1);
Cxy_os=Cxy_ts(1:ceil(N/2),rest_o_dims{:});
