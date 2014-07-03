function dpss0_prepare(N,nw,K,ofn)
% DPSS0_PREPARE - Use matlab's DPSS to prepare tapers for DPSS0
%    DPSS0_PREPARE(N,nw,K) prepares tapers and stores them in a file called
%       tapers-N-nw-K.mat
%    DPSS0_PREPARE(N,nw,K,ofn) specifies an alternative output file.

if nargin<4
  ofn = sprintf('tapers-%i-%.2f-%i.mat',N,nw,K);
end

if exist('dpss')
  tapers = dpss(N,nw,K);
else
  if unix(sprintf('dpss0_prepare %i %.2f %i %s',N,nw,K,ofn))
    fprintf(1,'Please find a Matlab installation that has DPSS from the\n');
    fprintf(1,'Signal Processing Toolbox, run:\n\n');
    fprintf(1,'  dpss0_prepare(%i,%.2f,%i)\n\n',N,nw,K);
    fprintf(1,'and copy the result to "%s"\n',ofn);
    error('DPSS0: Tapers not available.');
  else
    return
  end
end
 
fprintf(1,'Saving tapers for (%i,%.2f,%i) into %s\n',N,nw,K,ofn);

save(ofn,'tapers');
