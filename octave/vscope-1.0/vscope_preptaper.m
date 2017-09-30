function preptaper(N,nw,odir)
% PREPTAPER - Prepare tapers for PSDEst and CohEst
%    PREPTAPER(N, nw, odir) prepares a taper.

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

if ischar(N)
  if nargin>=2
    odir=nw;
  else
    odir=[];
  end
  [res,ok] = sscanf(N,'%i-%g');
  if ok<2
    error('PREPTAPER: argument error');
  end
  N=res(1);
  nw=res(2);
else
  if nargin<3
    odir=[];
  end
end

if isempty(odir)
  odir = getenv('VSCOPEPATH');
  if isempty(odir)
    odir = getenv('HOME');
  end
  odir = [odir filesep '_tapers'];
end

K = floor(2*nw-1);
if exist('dpss')
  tapers = dpss(N,nw,K);
else
  fprintf(1,'Please find a Matlab installation that has DPSS from the\n');
  fprintf(1,'Signal Processing Toolbox, run:\n\n');
  fprintf(1,'  preptaper(''%s'',%i,%.2f)\n\n',odir,N,nw);
  fprintf(1,'and copy the result to "%s"\n',odir);
  error('PREPTAPER: Tapers not available.');
end

if ~exist(odir)
  mkdir(odir);
end

fn = sprintf('%s%s%i-%.2f.taper',odir,filesep,N,nw);
fprintf(1,'Writing to "%s"\n',fn);
fd = fopen(fn,'wb');
for k=1:K
  dat = tapers(:,k);
  fwrite(fd,dat,'double');
end
fclose(fd);
