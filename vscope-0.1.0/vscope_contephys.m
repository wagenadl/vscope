function x=vscope_contephys(ifn, tri, wht)
% VSCOPE_CONTEPHYS - Load continuous e'phys data from vscope
%    dat = VSCOPE_LOAD(ifn, tri) extracts continuous e'phys from IFN as it
%    pertains to trial TRI.
%    dat = VSCOPE_LOAD(ifn, tri, letter), where LETTER is 'a' or 'd'
%    loads only analog or only digital data

if nargin<3
  wht='ad';
end
if ~ischar(ifn)
  ifn = sprintf('%03i.xml',ifn);
end

x = vscope_load(ifn,''); % Load information
if ~isfield(x.info,'trials')
  error('vscope_contephys: cannot read trial info');
end

K=length(x.info.trials.id);
k0=[];
for k=1:K
  if x.info.trials.id(k)==tri
    k0=k;
    break;
  end
end
if isempty(k0)
  error(sprintf('vscope_contephys: Trial %03i is not in %s',tri,ifn));
end

ascan = x.info.trials.ascan(k0);
dscan = x.info.trials.dscan(k0);

ifntri = regexprep(ifn,'\d\d\d\.xml',sprintf('%03i.xml',tri));
y = vscope_load(ifntri,'');
dur_ms = y.info.dur_ms;

if any(wht=='a')
  ifd = fopen(strrep(ifn,'.xml','-analog.dat'));
  if ifd<0
    error('vscope_contephys: Cannot read analog data');
  end
  nscans = round(dur_ms*x.analog.info.rate_hz/1e3);
  nchans = length(x.analog.info.channo);
  ntypebyt = x.analog.info.typebytes;
  x.analog.dat=zeros(nscans,nchans);
  K=length(x.analog.info.chunk);
  k=1;
  while x.analog.info.chunk(k).iend<ascan
    k=k+1;
  end
  % We have something to read from chunk k
  iscan=0;
  while nscans>0
    nnow = min(nscans, x.analog.info.chunk(k).iend+1-ascan);
    fseek(ifd,ascan*length(x.analog.info.channo)*ntypebyt,'bof');
    if ntypebyt==2
      dat=double(fread(ifd,[nchans nnow],'int16'))';
    else
      error('vscope_contephys: can only read 16-bit integers for analog');
    end
    for c=1:nchans
      dat(:,c) = dat(:,c)*x.analog.info.chunk(k).scale(c);
    end
    x.analog.dat(iscan+1:iscan+nnow,:) = dat;
    iscan = iscan + nnow;
    nscans = nscans - nnow;
    k = k + 1;
  end
  fclose(ifd);
end

if any(wht=='d')
  ifd = fopen(strrep(ifn, '.xml', '-digital.dat'));
  if ~ifd
    error('vscope_contephys: Cannot read digital data');
  end
  nscans = round(dur_ms*x.digital.info.rate_hz/1e3);
  ntypebyt = x.digital.info.typebytes;
  fseek(ifd,dscan*ntypebyt,'bof');
  if ntypebyt==4
    x.digital.dat=fread(ifd, [nscans 1], 'uint32');
  else
    error('vscope_contephys: Can only read uint32 for digital');
  end  
  fclose(ifd);
end

