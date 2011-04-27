function str=vscope_cohere(x, ref, dff)
% VSCOPE_COHERE - Calculate coherence between signals and reference
%    VSCOPE_COHERE(x, f_hz) calculates and displays coherence
%    for trial X to a sine wave of the given frequency.
%    VSCOPE_COHERE(x, 'ME1_10Vm') calculates and displays coherence
%    for trial X relative to the named e'phys signal.
%    X may be either a number or a filename, in which case the trial is
%    loaded using VSCOPE_LOAD, or a structure loaded by VSCOPE_LOAD.
%    VSCOPE_COHERE(x, ref, dff) prespecifies the signals. Otherwise, they
%    are extracted using VSCOPE_RATIO with debleach set to 2.
%    It is also allowed for REF to be a vector specifying a reference signal
%    directly. In that case, REF must be the same length as the columns of
%    DFF.

frange=[.5 inf 1];
f0=frange(1); f1=frange(2); fpre=frange(3);
ofnbase = '';

if ~isstruct(x)
  x = vscope_load(x);
end

if ischar(ref)
  chanidx = strmatch(ref, x.analog.info.chanid, 'exact');
  if length(chanidx)~=1
    error('Unknown reference channel');
  end
  f_star_hz = [];
elseif length(ref)==1
  chanidx = [];
  f_star_hz = ref;
else
  chanidx = [];
  f_star_hz = [];
end

if nargin<3
  dff = vscope_ratio(x.ccd, x.rois, 2);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Parameters
% ----------
alpha_ci = .31; % one-sigma error bars
%alpha_ci = 1; % Do not show error bars
alpha_thresh_single = .01; % Really, should be 10^-4 to avoid slew of false positives.
%alpha_thresh_single=1;
phase_delay_s = 0; %.10;

show_rois = 1;
show_polar = 1;
show_signals = 1;
title_string = sprintf('%s #%03i',x.info.expt, x.info.trial);
e_phys_plot_tog=zeros(1,length(x.analog.info.channo));
e_phys_plot_tog(chanidx)=1;
common_scale_flag = 0;
filter_flag = 1;

N=length(x.rois);
sig_labels=cell(N,1);
for n=1:N
  if isnan(dff(1,n))
    sig_labels{n} = '';
  else
    sig_labels{n} = base26(n);
  end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

y_sigs = dff(2:end,:);

if filter_flag
  [b,a] = butterhigh1(.05);
  for n=1:N
    y_sigs(:,n) = filtfilt(b,a,y_sigs(:,n));
  end
end

tt = vscope_ccdtime(x); tt = tt(2:end);
te = vscope_ephystime(x);

if ~isempty(chanidx)
  ephys_opt_freq = mean(diff(te)) / mean(diff(tt));
  [b,a] = butterlow1(3*ephys_opt_freq);
  y_ref = interp1(te,filtfilt(b,a,x.analog.dat(:,chanidx)),tt,'linear');
elseif ~isempty(f_star_hz)
  y_ref = sin(2*pi*tt*f_star_hz);
else
  y_ref = ref(2:end);
end


% pick the frequency resolutions
f_res_pow=1/3;  % Hz
f_res_coh=2/3;  % Hz

% estimate the spectra
[f_ref,P_ref]=pds_mtm0(tt,y_ref,f_res_pow);
[f_sigs,P_sigs]=pds_mtm0(tt,y_sigs,f_res_pow);

f_star = f_ref(argmax(P_ref))
if f_star<f0 | f_star>f1
  f_star=fpre
end
phase_offset = 2*pi*phase_delay_s*f_star;
figure(4); clf
plot(f_ref,P_ref);
hold on
plot(f_star,max(P_ref),'r.');

rndn=randn(size(y_sigs))*1e-18;
y_sigs(isnan(y_sigs))=rndn(isnan(y_sigs));

% estimate the coherence
[f_o,C_mag,C_phase,C_mag_lo,C_phase_lo,C_mag_hi,C_phase_hi]=...
  coh_mtm0(tt,y_sigs,repmat(y_ref,[1 N]),f_res_coh,alpha_ci);

% calc the coherence magnitude threshold
C_mag_thresh=...
  coh_mtm_control0(tt,y_sigs,y_ref,...
  f_res_coh,f_star,alpha_thresh_single);

	      

% extract the coherence and CIs for f=f_star
C_mag_star=interp1(f_o,C_mag,f_star,'*linear');
C_phase_star=interp1(f_o,C_phase,f_star,'*linear');
C_mag_lo_star=interp1(f_o,C_mag_lo,f_star,'*linear');
C_phase_lo_star=interp1(f_o,C_phase_lo,f_star,'*linear');
C_mag_hi_star=interp1(f_o,C_mag_hi,f_star,'*linear');
C_phase_hi_star=interp1(f_o,C_phase_hi,f_star,'*linear');

% sort the sigs in order of their coherence magnitude
[dummy,sort_indices]=sort(C_mag_star);
sort_indices=flipdim(sort_indices,2);  % want descending
sig_labels_sorted=sig_labels(sort_indices);
sigs_sorted=y_sigs(:,sort_indices);

% correct the coherence phases for the dye lag, for optical sigs
C_phase_star_corrected=C_phase_star+phase_offset;
C_phase_lo_star_corrected=C_phase_lo_star+phase_offset;
C_phase_hi_star_corrected=C_phase_hi_star+phase_offset;

% useful things for later
clr_subthresh=0.75*[1 1 1];
% mean_frame=mean(optical,3);
significant_rois=C_mag_star(1:N)>C_mag_thresh;


% plot the ROI frame
if show_rois
  % plot both mag and phase on the ROIs
  mean_frame = mean(x.ccd.dat(:,:,1,:),4);
  fig_roi_coh0(mean_frame,[],...
      C_mag_star,...
      C_phase_star_corrected,...
      C_mag_thresh,...
      clr_subthresh,...
      title_string,x.rois,sig_labels);
end

% do the polar plot of coherence at f_star
if show_polar
  fig_coh_polar0(C_mag_star,C_phase_star_corrected,...
      C_mag_lo_star,C_phase_lo_star_corrected,...
      C_mag_hi_star,C_phase_hi_star_corrected,...
      1:N,sig_labels,...
      0,C_mag_thresh,...
      1,...
      clr_subthresh);    
  text(.95,-.9,sprintf('f* = %.2f Hz',f_star),'horizontala','right');
end

if show_signals
  if ~filter_flag
    [b,a] = butterhigh1(.05);
    for n=1:N
      sigs_sorted(:,n) = filtfilt(b,a,sigs_sorted(:,n));
    end
  end
  fig_proof_sheet0(tt,sigs_sorted,te,x.analog.dat,e_phys_plot_tog,...
                  title_string,common_scale_flag,sig_labels_sorted,...
                  C_mag_star(sort_indices)>C_mag_thresh);  
end

if ~isempty(ofnbase)
  for f=1:3
    figure(f);
    printto(sprintf('%s-%i.eps',ofnbase,f));
  end
  fn=tempname;
  fd=fopen(fn,'w');
  fprintf(fd,'panel "A" l=.4in b=.7in {\n  image %s-3.eps\n}\n',ofnbase); 
  fprintf(fd,'panel "C" A.b l=A.r {\n  image %s-2.eps\n}\n',ofnbase);
  fprintf(fd,'panel "B" b=C.t+.5in C.l {\n  image %s-1.eps\n}\n',ofnbase);
  fclose(fd);
  unix(sprintf('panelist %s > %s.eps\n',fn,ofnbase));
  unix(sprintf('unlink %s',fn));
  unix(sprintf('unlink %s-1.eps',ofnbase));
  unix(sprintf('unlink %s-2.eps',ofnbase));
  unix(sprintf('unlink %s-3.eps',ofnbase));
end
  
str.mag_thresh = C_mag_thresh;
str.f_star = f_star;
str.mag_star = C_mag_star;
str.mag_lo_star = C_mag_lo_star;
str.mag_hi_star = C_mag_hi_star;
str.phase_star = C_phase_star_corrected;
str.phase_lo_star = C_phase_lo_star_corrected;
str.phase_hi_star = C_phase_hi_star_corrected;

str.t_o = tt;
str.y_o = y_sigs;
str.rois = x.rois;

str.t_e = te;
str.y_e = x.analog.dat;
str.y_ref = y_ref;

str.f_ref = f_ref;
str.P_ref = P_ref;
str.P_sigs = P_sigs;
str.f_coh = f_o;
str.mag_coh = C_mag;
str.phase_coh = C_phase;
str.mag_coh_lo = C_mag_lo;
str.phase_coh_lo = C_phase_lo;
str.mag_coh_hi = C_mag_hi;
str.phase_coh_hi = C_phase_hi;
