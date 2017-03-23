function dat = fread_interleaved(ifd,ichan,nchans,typ)
istart = ftell(ifd);
dd=fread(ifd,[1 1],typ); % Read one sample to know size
if isempty(dd)
  dat = [];
  return;
end
istep = ftell(ifd);
fseek(ifd,0,'eof');
iend = ftell(ifd);
fseek(ifd,istart,'bof');

nn = (iend-istart) / (istep-iend);

dat = zeros(nn,1);

i0=0;
while 1
  dd=fread(ifd,[nchans 16*1024*1024/nchans],typ);
  di=size(dd,2);
  if isempty(dd)
    break;
  else
    dat(i0+1:i0+di)=dd(ichan,:)';
    i0=i0+di;
    fprintf(1,'fread_interleaved: +%i -> %i\n',di,i0);
  end
end

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
