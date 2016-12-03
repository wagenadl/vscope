function tapers = dpss0(N,nw,K)
% DPSS0 - Load previously created DPSS tapers
%    tapers = DPSS0(N,nw,K). 
%    See matlab's DPSS.

try
  tapers = dpss(N, nw, K);
  return;
catch
  printf('Failure to use dpss directly\n');
end

if ~isempty(ver('Octave'))
  base = [ getenv('HOME') filesep 'matlab' filesep '.dpss' filesep ];
elseif filesep=='/'
  % unix
  base = '/home/wagenaar/matlab/.dpss/';
else
  base = 'c:\work\daniel\matlab\tapers\';
end

ifn = sprintf('%stapers-%i-%.2f-%i.mat', base, N, nw, K);

if ~exist(base)
  mkdir(base)
end
if exist(ifn)
  load(ifn);
else
  fprintf(1, ...
      'DPSS0: No previously created tapers available for %i,%.2f,%i.\n', ...
      N, nw, K);
  dpss0_prepare(N, nw, K, ifn);
  if exist(ifn)
    load(ifn);
  else
    error('DPSS0: Failed to load tapers');
  end
end
