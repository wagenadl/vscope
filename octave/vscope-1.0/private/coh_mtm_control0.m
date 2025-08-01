function [C_mag_thresh, sams]=coh_mtm_control0(t,roi,ref,f_res,f_star,alpha_thresh,PLOTFLAG)
if nargin<7
  PLOTFLAG=0;
end

% get dims

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
N=length(t);
N_signals=size(roi,2);

% figure out number of perms to do
N_perms_approx=100/alpha_thresh;
N_passes=ceil(N_perms_approx/N_signals);
N_perms=N_passes*N_signals;

% do the perms
ref=repmat(ref,[1 N_signals]);
C_mag_samples=zeros([1 N_signals N_passes]);
%figure;
for j=1:N_passes
  %  fprintf(1,'.');  if mod(j,50)==0 fprintf(1,'\n'); end
  perm=randperm(N);
  shuffled_roi=roi(perm,:);
  %plot(t,shuffled_roi);
  C_mag_samples(:,:,j), pha = ...
    coh_mtm0(t,shuffled_roi,ref,f_res,[],f_star);
end
if mod(j,50)~=0 fprintf(1,'\n'); end

if nargout>=2
  sams = C_mag_samples;
end

% collect the samples into a vector
C_mag_samples=C_mag_samples(~isnan(C_mag_samples));
N_perms = length(C_mag_samples);

% plot the histo
if PLOTFLAG
  [n_C_mag_samples,C_mag_grid]=hist(C_mag_samples,100);
  figure;
  h=bar(C_mag_grid,n_C_mag_samples);
  set(h,'EdgeColor','none');
  xlabel('coh mag');
  ylabel('number');
  title('shuffled control histogram');
end

% compute the significance threshold
C_mag_samples_sorted=sort(C_mag_samples);
C_mag_thresh=interp1(linspace(0,1,N_perms),...
                     C_mag_samples_sorted,...
                     1-alpha_thresh,...
                     'linear');
