function [N,nw,K] = ntapers_mtm0(t,f_res)
% NTAPERS_MTM0 - Calculate information about tapers that PDS_MTM0 would use
%   [N,nw,K] = NTAPERS_MTM0(tt,f_res) returns the number of time samples N,
%   as well as the bandwidth number NW and the number of tapers K.
%   (Actually, the half-bandwidth is W=NW/N.)

N=length(t);  % n of time samples
dt=(t(N)-t(1))/(N-1);
fs=1/dt;

% compute nw and K
nw=N*dt*f_res;
K=floor(2*nw-1);
